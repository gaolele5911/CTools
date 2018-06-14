main:CharToLongLong.o ping.o main.o
	gcc -o main CharToLongLong.o ping.o main.o -lm
CharToLongLong.o:CharToLongLong/CharToLongLong.c
	gcc -c CharToLongLong/CharToLongLong.c -o CharToLongLong.o -lm
ping.o:Ping/ping.c
	gcc -c Ping/ping.c -o ping.o -lm
main.o:main.c
	gcc -c main.c -o main.o -lm
.PHONY:clean
clean:
	rm -rf CharToLongLong.o ping.o main.o main
