#include <sys/epoll.h>
#include <errno.h>
#include <sys/times.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include "epoll_ctl.h"
#include "log.h"

int epoll_fd = -1;
clock_t nexttimeout;
clock_t clks_per_sec;

int init_epoll(void)
{
	int r = epoll_create(MAX_LINK_CNT);
	if (r < 0) {
		return -1;
	}
	epoll_fd = r;
	clks_per_sec = (clock_t)sysconf(_SC_CLK_TCK);
	return 0;
}
int add_epoll(struct epoll_event_handler *h)
{
	struct epoll_event ev = {
		.events = EPOLLIN,
		.data.ptr = h,
	};
	h->ref_ev = NULL;
	int r = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, h->fd, &ev);
	if(r < 0){
		fprintf(stderr, "epoll_ctl_add: %m\n");
		return -1;
	}
	return 0;
}

int remove_epoll(struct epoll_event_handler *h)
{
	int r = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, h->fd, NULL);
	if(r < 0){
		fprintf(stderr, "epoll_ctl_del: %m\n");
		return -1;
	}
	if(h->ref_ev && h->ref_ev->data.ptr == h){
		h->ref_ev->data.ptr = NULL;
		h->ref_ev = NULL;
	}
	return 0;
}

void clear_epoll()
{
	if(epoll_fd > 0)
		close(epoll_fd);
}
void run_timeouts(void)
{
	/*period functions*/
	nexttimeout += clks_per_sec;
}


int epoll_main_loop(User_Linker *userinfo, unsigned long *packet_cnt)
{
	int r, i;
	int timeout;
	nexttimeout = times(NULL) + clks_per_sec;
	struct epoll_event ev[EV_SIZE];
	while (1) {
		clock_t now = times(NULL);
		long clocks_left = (long)(nexttimeout - now);
			if(clocks_left <= 0){
				run_timeouts();
				timeout = 0;
			}else{
				timeout = (clocks_left * 1000) / clks_per_sec;
			}
		r = epoll_wait(epoll_fd, ev, EV_SIZE, timeout);
		if (r < 0 && errno != EINTR) {
			fprintf(stderr, "epoll_wait: %m\n");
			return -1;
		}
		for (i = 0; i < r; i++) {
			struct epoll_event_handler *p = ev[i].data.ptr;
			if (p != NULL)
				p->ref_ev = &ev[i];
		}
		for (i = 0; i < r; i++) {
			struct epoll_event_handler *p = ev[i].data.ptr;
			if (p && p->handler)
				p->handler(ev[i].events, p, &userinfo, &packet_cnt);
		}
		for (i = 0; i < r; i++) {
			struct epoll_event_handler *p = ev[i].data.ptr;
			if (p != NULL)
				p->ref_ev = NULL;
		}
	}
	return 0;
}


