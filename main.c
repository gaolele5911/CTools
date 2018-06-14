#include "Ping/ping.h"

int main(int argc, char *argv[])
{
	int i = pingIP("www.baidu.com");
    printf("**** ping i = %d \r\n",i);
	return 0;
}
