/*
 * Test WiFi Speed
 *
 * Copyright (C) 2014.10 Qian RunSheng
 * All Rights Reserved
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <math.h>

#define IpAddr              "192.168.1.10"//"192.168.1.127"//"192.168.1.126"//"192.168.1.10"//
#define PortNum             25000
#define SendDelay           13000//AP_C: 13ms default, STA_C: 13ms default, STA_S: 5ms default
#define ServerDownTimes     8192
#define ClientDownTimes     4096
#define CDownIpPacketBytes  1024
#define SDownIpPacketBytes  512

int main(int argc, char *argv[])
{
	int sockfd, con_fd, fd;
	int sin_size;
	int len, i;
	unsigned char outbuf[1200], inbuf[1200];
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	//struct hostent *host;
	struct timeval tpstart, tpend;
	float timeuse = 0;

	int stoptest = 1;

	int recvpackets = 0;
	int recvbytes = 0;
	int sendpackets = 0;
	int sendbytes = 0;

	char testmode = 0;
	char testnum = 0;
	int bits = 0;
	int downtimes = 0;

	/* 客户程序开始建立 sockfd描述符 */
	if ((sockfd = socket(AF_INET,SOCK_STREAM, 0))==-1) {// AF_INET:Internet;SOCK_STREAM:TCP
		fprintf(stderr,"Socket Error:%s\a\n",strerror(errno));
		exit(1);
	}

	/* 客户程序填充服务端的资料 */
	bzero(&server_addr, sizeof(server_addr)); // 初始化,置0
	server_addr.sin_family = AF_INET;          // IPV4
	server_addr.sin_port = htons(PortNum);  // (将本机器上的short数据转化为网络上的short数据)端口号
	//server_addr.sin_addr = *((struct in_addr *)host->h_addr); // IP地址,即服务器地址.两个结构体赋值
	server_addr.sin_addr.s_addr = inet_addr(IpAddr);

	printf("\n"
			"###################\n"
			"Select Test Mode:\n"
			"1  Server Test\n"
			"2  Client Test\n"
			"3  Exit Test\n"
			"###################\n");
	testmode = getchar();
	getchar();// read '\n'

	//select socket mode
	do {
		if (testmode == '1') {
			if (bind(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr))==-1) { 
				fprintf(stderr,"Bind error:%s\n\a",strerror(errno));
				exit(1);
			}

			/* 设置允许连接的最大客户端数 */
			if (listen(sockfd, 5) == -1) {//等待连接
				fprintf(stderr,"Listen error:%s\n\a",strerror(errno));
				exit(1);
			}

			printf("waiting client connection..\n");
			/* 服务器阻塞,直到客户程序建立连接 */
			sin_size = sizeof(struct sockaddr_in);
			if ((con_fd = accept(sockfd, (struct sockaddr *)(&client_addr), &sin_size))==-1) {
				fprintf(stderr, "Accept error:%s\n\a", strerror(errno));
				exit(1);
			}
			printf("Server get connection from %s\n", inet_ntoa(client_addr.sin_addr)); //将网络地址转换成.字符串
			fd = con_fd;

			break;
		}
		else if (testmode == '2') {
			/* 客户程序发起连接请求 */
			if (connect(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr))==-1) {
				fprintf(stderr, "Connect Error:%s\a\n", strerror(errno));
				exit(1);
			}
			printf("Socket connects Server sucessed\n\n");
			fd = sockfd;

			break;
		}
		else if (testmode == '3'){
			return 0;
		}
	} while (1);

	//select test item
	do {
		printf("\n\n"
			   "**************************\n"
			   "1  transimit test\n"
			   "2  up speed test\n"
			   "3  down speed test\n"
			   "4  exit test\n"
			   "**************************\n"
			   "Please Input num and Enter:\n");

		testnum = getchar();
		getchar();// read '\n'

		len = 0;
		recvbytes = 0;
		recvpackets = 0;
		sendbytes = 0;
		sendpackets = 0;
		timeuse = 0;
		bits = 0;

		if (testmode == '1') {
			bits = SDownIpPacketBytes;
			downtimes = ServerDownTimes;
		}
		else {
			bits = CDownIpPacketBytes;//client
			downtimes = ClientDownTimes;
		}

		switch (testnum) {
			case '1' : {
				printf("transimit test...\n");

				//strcpy(outbuf, "hello rak411");
				for (i = 0; i < bits; i++) {
					outbuf[i] = 'k';
				}
				outbuf[bits] = '\0';
				printf("send %d bytes data...\n", bits);
				write(fd, outbuf, strlen(outbuf));

				len = read(fd, inbuf, sizeof(inbuf));
				if (len == -1) {
					fprintf(stderr, "Read Error:%s\n", strerror(errno));
					exit(1);
				}
				inbuf[len] = '\0';
				printf("received  %d bytes data:\n%s\n", len, inbuf);
			} break;

			case '2' : {
				printf("start up speed test...\n");
				stoptest = 0;

				strcpy(outbuf, "up");
				write(fd, outbuf, strlen(outbuf));

				gettimeofday(&tpstart,NULL); // 开始时间
				do {
					len = read(fd, inbuf, sizeof(inbuf));
					if (len ==-1) {
						fprintf(stderr,"Read Error:%s\n",strerror(errno));
						exit(1);
					}
					recvpackets++;
					recvbytes += len;
					inbuf[len] = '\0';
					if (recvpackets == 4096) {
						printf("received  data :len = %d\n%s\n", len, inbuf);
					}

					if (inbuf[0] != 'u' || inbuf[1] != 'p') {
						gettimeofday(&tpend, NULL);   // 结束时间
						printf("stop up test...\n");
						stoptest = 1;
						recvbytes -= len;
						recvpackets -= 1;
					}
				} while (!stoptest);

				/* 计算执行时间 */
				timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec)
						+ (tpend.tv_usec - tpstart.tv_usec); //单位为微秒
				timeuse /= 1000000.0;//单位为秒

				printf("Received %d packets , %d KB\n", recvpackets, recvbytes / 1024);
				printf("Used Time: %.3f\n", timeuse);
				printf("the WiFi up speed %.3f KB/s\n", (recvbytes / 1024) / timeuse);
			} break;

			case '3': {
				printf("start down speed test...\n");
				stoptest = 0;

				strcpy(outbuf, "down");
				write(fd, outbuf, strlen(outbuf));

				len = read(fd, inbuf, sizeof(inbuf));
				if (len ==-1) {
					printf("Read Error:%s\n", strerror(errno));
					exit(1);
				}
				inbuf[len] = '\0';

				if (inbuf[0] != 'd' || inbuf[1] != 'o') {
					printf("haha\n");
					break;
				}

				for (i = 0; i < (bits - 3);) {
					outbuf[i] = 'd';
					outbuf[i + 1] = 'o';
					i += 2;
				}
				outbuf[bits - 3] = '1';
				outbuf[bits - 2] = '2';
				outbuf[bits - 1] = '3';
				outbuf[bits] = '\0';

				len = strlen(outbuf);
				printf("one data is %d bytes\n", len);
				while (sendpackets < downtimes) {
					write(fd, outbuf, len);
					++sendpackets;
					sendbytes += len;
					usleep(SendDelay);// it is very important
				}

				printf("stop down test...\n");
				outbuf[0] = 's';
				outbuf[1] = 't';
				write(fd, outbuf, len);

				len = read(fd, inbuf, sizeof(inbuf));
				if (len == -1) {
					printf("Read Error:%s\n",strerror(errno));
					exit(1);
				}
				inbuf[len] = '\0';
				i = 0;
				timeuse = 0;
				while(i < len) {
					timeuse += (int)(inbuf[i] - '0') * (int)pow(10, len - i - 1);
					i++;
				}

				timeuse = timeuse / 1000.0;

				printf("Send %d packet, %d KB\n", sendpackets, sendbytes / 1024);
				printf("Used Time: %.3f\n", timeuse);
				printf("the WiFi down speed %.3f KB/s\n", (sendbytes / 1024) / timeuse);
			} break;

			case '4':
				goto __exit;
				break;
			default:
				break;
		}
	} while (1);

__exit:
	/* 结束通讯 */
	close(con_fd);
	close(sockfd);
	exit(0);
}
