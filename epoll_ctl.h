#ifndef EPOLL_CTL_H_
#define EPOLL_CTL_H_
#include "data_st.h"
#include <sys/epoll.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

#define MAX_LINK_CNT 1024
#define EV_SIZE 8

struct epoll_event_handler {
	int fd;
	void *args;
	void (*handler)(uint32_t events, struct epoll_event_handler *p,User_Linker **user,unsigned long **packet_cnt);
	struct epoll_event *ref_ev;
};
int init_epoll(void);
int add_epoll(struct epoll_event_handler *h);
int remove_epoll(struct epoll_event_handler *h);
void clear_epoll(void);
int epoll_main_loop(User_Linker *userinfo,unsigned long *packet_cnt);
#endif /* EPOLL_CTL_H_ */
