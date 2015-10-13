#ifndef PACKET_CTL_H_
#define PACKET_CTL_H_
#include "data_st.h"

#define SVR_PORT_FT	4091
#define SVR_PORT_SD	4096

int packet_socket_init(int port, struct epoll_event_handler *hp, User_Linker **userinfo);
void packet_socket_cleanup(struct epoll_event_handler *hp);
void ccatn_packet_rcv(uint32_t events, struct epoll_event_handler *h, User_Linker **userinfo, unsigned long **packet_cnt);

#endif /* PACKET_CTL_H_ */
