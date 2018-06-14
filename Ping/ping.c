#include "ping.h"

#define WAIT_TIME 5

char SendBuffer[SEND_BUFFER_SIZE];
char RecvBuffer[RECV_BUFFER_SIZE];
int nRecv = 0;    //实际接收到的报文数
struct timeval FirstSendTime;    //用以计算总的时间
struct timeval LastRecvTime;
double min = 0.0;
double avg = 0.0;
double max = 0.0;
double mdev = 0.0;

struct hostent * pHost = NULL;        //保存主机信息
int sock_icmp;                //icmp套接字
int nSend = 1;
char *IP = NULL;

//计算校验和
u_int16_t Compute_cksum(struct icmp *pIcmp)
{
    u_int16_t *data = (u_int16_t *)pIcmp;
    int len = ICMP_LEN;
    u_int32_t sum = 0;
    
    while (len > 1)
    {
        sum += *data++;
        len -= 2;
    }
    if (1 == len)
    {
        u_int16_t tmp = *data;
        tmp &= 0xff00;
        sum += tmp;
    }
    
    //ICMP校验和带进位
    while (sum >> 16)
        sum = (sum >> 16) + (sum & 0x0000ffff);
    sum = ~sum;
    
    return sum;
}

//设置ICMP报文
void SetICMP(u_int16_t seq)
{
    struct icmp *pIcmp;
    struct timeval *pTime;
    
    pIcmp = (struct icmp*)SendBuffer;
    
    /* 类型和代码分别为ICMP_ECHO,0代表请求回送 */
    pIcmp->icmp_type = ICMP_ECHO;
    pIcmp->icmp_code = 0;
    pIcmp->icmp_cksum = 0;        //校验和
    pIcmp->icmp_seq = seq;        //序号
    pIcmp->icmp_id = getpid();    //取进程号作为标志
    pTime = (struct timeval *)pIcmp->icmp_data;
    gettimeofday(pTime, NULL);    //数据段存放发送时间
    pIcmp->icmp_cksum = Compute_cksum(pIcmp);
    
    if (1 == seq)
        FirstSendTime = *pTime;
}

//发送ICMP报文
void SendPacket(int sock_icmp, struct sockaddr_in *dest_addr, int nSend)
{
    SetICMP(nSend);
    
    if (sendto(sock_icmp, SendBuffer, ICMP_LEN, 0,
               (struct sockaddr *)dest_addr, sizeof(struct sockaddr_in)) < 0)
    {
        perror("sendto");
        return;
    }
}

//计算往返时间
double GetRtt(struct timeval *RecvTime, struct timeval *SendTime)
{
    struct timeval sub = *RecvTime;
    
    if ((sub.tv_usec -= SendTime->tv_usec) < 0)
    {
        --(sub.tv_sec);
        sub.tv_usec += 1000000;
    }
    sub.tv_sec -= SendTime->tv_sec;
    
    return sub.tv_sec * 1000.0 + sub.tv_usec / 1000.0; //转换单位为毫秒
}

//剥去报头
int unpack(struct timeval *RecvTime)
{
    struct ip *Ip = (struct ip *)RecvBuffer;
    struct icmp *Icmp;
    int ipHeadLen;
    double rtt;
    
    ipHeadLen = Ip->ip_hl << 2;    //ip_hl字段单位为4字节
    Icmp = (struct icmp *)(RecvBuffer + ipHeadLen);
    
    //判断接收到的报文是否是自己所发报文的响应
    if ((Icmp->icmp_type == ICMP_ECHOREPLY) && Icmp->icmp_id == getpid())
    {
        struct timeval *SendTime = (struct timeval *)Icmp->icmp_data;
        rtt = GetRtt(RecvTime, SendTime);
        
        printf("%u bytes from %s: icmp_seq=%u ttl=%u time=%.1f ms\n",
               ntohs(Ip->ip_len) - ipHeadLen,
               inet_ntoa(Ip->ip_src),
               Icmp->icmp_seq,
               Ip->ip_ttl,
               rtt);
        
        if (rtt < min || 0 == min)
            min = rtt;
        if (rtt > max)
            max = rtt;
        avg += rtt;
        mdev += rtt * rtt;
        
        return 0;
    }
    
    return -1;
}


//统计信息1
void Statistics(int signo)
{
    double tmp;
    avg /= nRecv;
    tmp = mdev / nRecv - avg * avg;
    mdev = sqrt(tmp);
    
    if (NULL != pHost)
        printf("--- %s  ping statistics ---\n", pHost->h_name);
    else
        printf("--- %s  ping statistics ---\n", IP);
    
    printf("%d packets transmitted, %d received, %d%% packet loss, time %dms\n"
           , nSend
           , nRecv
           , (nSend - nRecv) / nSend * 100
           , (int)GetRtt(&LastRecvTime, &FirstSendTime));
    printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
           min, avg, max, mdev);
    
    close(sock_icmp);
}

//接收ICMP报文
int RecvePacket(int sock_icmp, struct sockaddr_in *dest_addr)
{
    int RecvBytes = 0;
    int addrlen = sizeof(struct sockaddr_in);
    struct timeval RecvTime;
    
    signal(SIGALRM, Statistics);
    alarm(WAIT_TIME);
    
    //设置为阻塞超时1S
    struct timeval tv_out;
    tv_out.tv_sec = 1;
    tv_out.tv_usec = 0;
    setsockopt(sock_icmp,SOL_SOCKET,SO_RCVTIMEO,&tv_out, sizeof(tv_out));

    if ((RecvBytes = recvfrom(sock_icmp, RecvBuffer, RECV_BUFFER_SIZE,
                              0, (struct sockaddr *)dest_addr, (socklen_t *)(&addrlen))) < 0)
    {
        perror("recvfrom");
        return 0;
    }
    
    gettimeofday(&RecvTime, NULL);
    LastRecvTime = RecvTime;
    
    if (unpack(&RecvTime) == -1)
    {
        return -1;
    }
    nRecv++;
}

//重置
void resetNRecv()
{
    nSend = 1;
    nRecv = 0;    //实际接收到的报文数
    min = 0.0;
    avg = 0.0;
    max = 0.0;
    mdev = 0.0;
}

int pingIP(char *ipAddress)
{
    struct protoent *protocol;
    struct sockaddr_in dest_addr;     //IPv4专用socket地址,保存目的地址
    int socket_times = 0;
    in_addr_t inaddr;        //ip地址（网络字节序）
    
    resetNRecv();
    
    if ((protocol = getprotobyname("icmp")) == NULL)
    {
        perror("getprotobyname");
        return 1;
    }
    
    /* 创建ICMP套接字 */
    //AF_INET:IPv4, SOCK_RAW:IP协议数据报接口, IPPROTO_ICMP:ICMP协议
    //    if ((sock_icmp = socket(PF_INET, SOCK_RAW, protocol->p_proto/*IPPROTO_ICMP*/)) < 0)
    sock_icmp = socket(PF_INET, SOCK_RAW, protocol->p_proto/*IPPROTO_ICMP*/);
    while((sock_icmp <= 2))
    {
        if(socket_times < 100)
        {
            sock_icmp = socket(PF_INET, SOCK_RAW, protocol->p_proto/*IPPROTO_ICMP*/);
            socket_times++;
            continue;
        }
        else
        {
            
            perror("socket");
            return 1;
        }
    }
    dest_addr.sin_family = AF_INET;
    
    /* 将点分十进制ip地址转换为网络字节序 */
    if ((inaddr = inet_addr(ipAddress)) == INADDR_NONE)
    {
        /* 转换失败，表明是主机名,需通过主机名获取ip */
        if ((pHost = gethostbyname(ipAddress)) == NULL)
        {
            printf("pingIP 444 return 3 \r\n");
            
            herror("gethostbyname()");
            return 1;
        }
        memmove(&dest_addr.sin_addr, pHost->h_addr_list[0], pHost->h_length);
    }
    else
    {
        memmove(&dest_addr.sin_addr, &inaddr, sizeof(struct in_addr));
    }
    
    IP = ipAddress;
    signal(SIGINT, Statistics);
    while (nSend <= SEND_NUM)
    {
        int unpack_ret;
        
        SendPacket(sock_icmp, &dest_addr, nSend);
        unpack_ret = RecvePacket(sock_icmp, &dest_addr);
        if (-1 == unpack_ret)    //（ping回环时）收到了自己发出的报文,重新等待接收
        {
            RecvePacket(sock_icmp, &dest_addr);
        }
        
        sleep(1);
        nSend++;
    }
    Statistics(0);  //输出信息，关闭套接字
    
    if (nRecv)
    {
        return 0;
    }
    else {
        return 1;
    }
}

