#include "Ping/ping.h"
#include "CharToLongLong/CharToLongLong.h"

int main(int argc, char *argv[])
{
//    int i = pingIP("www.baidu.com");
//    printf("**** ping i = %d \r\n",i);
    char aa[8] = {0,0,0,0,0,0,0,5};
    long long index = charArrayToLongLong2(aa);
    printf("**** ping 22 index = %lld \r\n",index);

	return 0;
}
