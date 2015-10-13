#ifndef DATA_ST_H_
#define DATA_ST_H_
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CHAT_FLAG "CHAT-FLAG"
typedef struct chat_struct {
	int srcid;
	int dstid;
	char data[1024];
} Chat_st;


typedef struct user_link_linker {
	int srcid;
	int fd;
	struct sockaddr_in addr;
	struct user_link_linker *next;
} User_Linker;

User_Linker *add_user_link(User_Linker *head, int srcid, int fd);
User_Linker *add_user_link_addr(User_Linker *head, int srcid,struct sockaddr_in *addr,const socklen_t len);
User_Linker *del_user_link(User_Linker *head, int fd);
int search_user_link_fd(User_Linker *head, int srcid, struct sockaddr_in *addr);
void print_user_linker_table(User_Linker *head);
void free_user_link_linker(User_Linker *head);
#endif /* DATA_ST_H_ */
