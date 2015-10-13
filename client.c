#include "data_st.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>


void udpc_req(int sockfd, const struct sockaddr_in *addr, const struct sockaddr_in *peer,const int sport, const int dport, socklen_t len)
{
	int pid;
	int cnt = 0;
	Chat_st user;
	while(1){
		cnt++;
		bzero(&user, sizeof(user));
		if((pid = fork())<0){
			continue;
		} else if(pid > 0) {
			bzero(&user, sizeof(user));
			if(recvfrom(sockfd, &user, sizeof(user), 0, (struct sockaddr *)peer, &len) < 0) {
				fprintf(stderr,"Recieve error:%s\n ",strerror(errno));
				return;
			}
			if(len == sizeof(peer) || memcmp((const void *)peer, (const void *)addr, len) == 0)
				fprintf(stdout,"\rFrom(%d): %s\n",user.srcid,user.data);
		} else if(pid == 0) {
			user.srcid = sport;
			user.dstid = dport;
			if(cnt != 1) {
				fprintf(stdout,"\rTo(%d)>",user.dstid);
				if(fgets(user.data, 1024, stdin) != NULL) {
					user.data[strlen(user.data) - 1] = 0;
					if(strlen(user.data) == 0) {
						fprintf(stdout,"Please input something !\n");
						continue;
					}
				}
			} else {
				fprintf(stdout,"cnt:%d\n",cnt);
				sprintf(user.data, CHAT_FLAG);
			}
			if(strlen(user.data) > 0) {
				sendto(sockfd, &user, sizeof(user), 0, (struct sockaddr*)addr, len);
			}
		}
	}
}


int main(int argc, char **argv)
{
	int sockfd;
	socklen_t addrlen;
	struct sockaddr_in addr,peer;
	if(argc < 4) {
		fprintf(stderr, "Usage: %s server_ip sport dport\n", argv[0]);
		return -1;
	}
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0) {
		fprintf(stderr, "Socket error:%s\n",strerror(errno));
		return -1;
	}
	bzero(&addr, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//addr.sin_port = htons(SVR_PORT_FT);
	if(inet_aton(argv[1], &addr.sin_addr) < 0) {
		fprintf(stderr, "IP error:%s\n",strerror(errno));
		return -1;
	}
	addr.sin_port = htons(atoi(argv[2]));
	addrlen = sizeof(addr);
	udpc_req(sockfd, &addr, &peer, (const int)(atoi(argv[2])), (const int)(atoi(argv[3])), addrlen);
	close(sockfd);
	return 0;
}
