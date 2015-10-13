#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <syslog.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "epoll_ctl.h"
#include "packet_ctl.h"
#include "ccatn.h"
#include "log.h"


int log_level = LOG_LEVEL_DEFAULT;
static int become_daemon = 1;
int execed = 0;
static struct epoll_event_handler packet_handler_1;
static struct epoll_event_handler packet_handler_2;

int main(int argc, char *argv[])
{
	int c;
	int _argc = 0;
	unsigned long packet_cnt = 0;
	char *_argv[3];
	pid_t pid;
	static User_Linker *usrinfo = NULL;
	while ((c = getopt(argc, argv, "dDv:")) != -1) {
		switch (c) {
		case 'd':
			become_daemon = 0;
			break;
		case 'D':
			execed = 1;
			break;
		default:
			return -1;
		}
	}
	if (become_daemon) {
		if (!execed) {
			if ((pid = vfork()) < 0) {
				fprintf(stderr, "vfork failed\n");
				exit(1);
			} else if (pid != 0) {
				exit(0);	//parent progress exit
			}

			_argv[_argc++] = argv[0];
			_argv[_argc++] = "-D";
			_argv[_argc++] = NULL;
			execv(_argv[0], _argv);
			/* Not reached */
			fprintf(stderr, "Couldn't exec\n");
			_exit(1);

		} else {
			setsid();
			chdir("/");
			umask(0);
			close(0);
			close(1);
			close(2);
		}
		FILE *f = fopen("/var/run/lldpd.pid", "w");
		if (!f) {
			ERROR("can't open /var/run/lldpd.pid");
			return -1;
		}
	}
	INFO("lldp start");
	/*Create epoll*/
	TST(init_epoll()==0,-1);
	/*Initialize client control socket*/
	//TST(ctlserver_socket_init()==0,-1);
	TST(packet_socket_init(SVR_PORT_SD, &packet_handler_1, &usrinfo)==0, -1);
	TST(packet_socket_init(SVR_PORT_FT, &packet_handler_2, &usrinfo)==0, -1);
	epoll_main_loop(usrinfo, &packet_cnt);
	//ctlserver_socket_cleanup();
	packet_socket_cleanup(&packet_handler_1);
	packet_socket_cleanup(&packet_handler_2);
	free_user_link_linker(usrinfo);
	return 0;
}

char logbuf[2048];
void vDprintf(int level, const char *fmt, va_list ap)
{
	time_t clock;
	struct tm *local_tm;

	int l;
	if (level > log_level)
		return;
	time(&clock);
	logbuf[2047] = 0;
	local_tm = localtime(&clock);
	l =  strftime(logbuf, sizeof(logbuf) - 1, "%F %T ", local_tm);
	vsnprintf(logbuf + l, sizeof(logbuf) - l - 1, fmt, ap);
	if(become_daemon){
		syslog((level <= LOG_LEVEL_INFO) ? LOG_INFO : LOG_DEBUG, logbuf);
	}else{
		printf("%s\n", logbuf);
	}
}

void Dprintf(int level, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vDprintf(level, fmt, ap);
	va_end(ap);
}
#define VERSION 0.4
void usage()
{
	printf("Usage: lldpd \nversion v0.4\n");
}
