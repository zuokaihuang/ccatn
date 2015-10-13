#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include "epoll_ctl.h"
#include "packet_ctl.h"
#include "ccatn.h"
#include "log.h"


int packet_socket_init(int port, struct epoll_event_handler *hp, User_Linker **userinfo)
{
	int s;
	struct sockaddr_in addr;
	s = socket(PF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		ERROR("socket failed: %m");
		return -1;
	}
	bzero(&addr, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	if(bind(s, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) {
		fprintf(stderr,"bind error:%s\n",strerror(errno));
		return -1;
	}
	*userinfo = add_user_link(*userinfo, port, s);
	if (fcntl(s, F_SETFL, O_NONBLOCK) < 0)
		ERROR("fcntl set nonblock failed: %m");

	else {
		hp->fd = s;
		hp->handler = ccatn_packet_rcv;
		if (add_epoll(hp) == 0)
			return 0;
	}
	close(s);
	return -1;
}

void packet_socket_cleanup( struct epoll_event_handler *hp)
{
	remove_epoll(hp);
	close(hp->fd);
}

void ccatn_packet_rcv(uint32_t events, struct epoll_event_handler *h, User_Linker **userinfo, unsigned long **packet_cnt)
{
	int cc;
	int dfd;
	socklen_t addrlen;
	struct sockaddr_in client;
	struct sockaddr_in peer;
	Chat_st user;
	bzero(&user, sizeof(user));
	addrlen = sizeof(client);
	cc = recvfrom(h->fd, &user, sizeof(user), 0, (struct sockaddr *)&client, &addrlen);
	//fprintf(stdout,"client %s-%d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
	*userinfo = add_user_link_addr(*userinfo, user.srcid, &client, addrlen);
	if (cc <= 0) {
		ERROR("read failed: %m");
		return;
	}else{
		fprintf(stdout,"Receive from %s-%d :"
				" sid=%d,did=%d,data=%s \tpacket_cnt=%u\n",
				inet_ntoa(client.sin_addr),ntohs(client.sin_port),
				user.srcid,user.dstid,user.data,**packet_cnt);
	}
	if(strncmp(user.data, CHAT_FLAG, 9) != 0) {
		dfd = search_user_link_fd(*userinfo,user.dstid, &peer);
		//print_user_linker_table(*userinfo);
		//printf("peer:%s-%d\n",inet_ntoa(peer.sin_addr),ntohs(peer.sin_port));
		if(sendto(dfd, &user, sizeof(user), 0, (struct sockaddr *)&peer, addrlen) <= 0) {
			fprintf(stderr,"Send :%s\n",strerror(errno));
		}
	}
	**packet_cnt += 2;
	//sleep(2);
}
