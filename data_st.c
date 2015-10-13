#include "data_st.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>

User_Linker *add_user_link(User_Linker *head, int srcid, int fd)
{
	User_Linker *tmp_linker = NULL;
	User_Linker *new_linker = NULL;
	if((new_linker = (User_Linker *)calloc(1,sizeof(User_Linker))) == NULL) {
		fprintf(stderr,"Create new linker error:%s\n",strerror(errno));
		goto RESULT;
	} else {
		new_linker->srcid = srcid;
		new_linker->fd = fd;
		new_linker->next = NULL;
		if(head == NULL) {
			head = new_linker;
		} else {
			tmp_linker = head;
			while(tmp_linker->next != NULL) {
				tmp_linker = tmp_linker->next;
			}
			tmp_linker->next = new_linker;
		}
		tmp_linker = NULL;
	}
RESULT:
	return head;
}
User_Linker *add_user_link_addr(User_Linker *head, int srcid,struct sockaddr_in *addr,const socklen_t len)
{
	User_Linker *tmp_linker = NULL;
	if(head != NULL) {
		tmp_linker = head;
		while(tmp_linker != NULL) {
			if(tmp_linker->srcid == srcid) {
				//printf("sid:%d,pt:%d\n",tmp_linker->srcid,addr->sin_port);
				memcpy(&tmp_linker->addr, addr, len);
				//printf("ss sid:%d,pt:%d\n",tmp_linker->srcid,tmp_linker->addr.sin_port);
				return head;
			}
			tmp_linker = tmp_linker->next;
		}
	}
	return head;
}
User_Linker *del_user_link(User_Linker *head, int fd)
{
/*	User_Linker *tmp_linker = NULL;
	if(head != NULL) {
		tmp_linker = head;
		while(tmp_linker != NULL) {
			if(tmp_linker == fd) {

			}
		}
	}*/
	return NULL;
}
int search_user_link_fd(User_Linker *head, int srcid, struct sockaddr_in *addr)
{
	int fd = 0;
	User_Linker *tmp_linker = NULL;
	if(head != NULL) {
		tmp_linker = head;
		while(tmp_linker != NULL) {
			if(tmp_linker->srcid == srcid) {
				fd = tmp_linker->fd;
				memcpy(addr, &tmp_linker->addr, sizeof(struct sockaddr_in));
			}
			tmp_linker = tmp_linker->next;
		}
	}
	tmp_linker = NULL;
	return fd;
}

void print_user_linker_table(User_Linker *head)
{
	User_Linker *p;
	p = head;
	while(p != NULL) {
		fprintf(stdout,"srcid:%d,fd:%d,sin_addr:%s,sin_port:%d\n",
				p->srcid,p->fd,inet_ntoa(p->addr.sin_addr),ntohs(p->addr.sin_port));
		p = p->next;
	}
}

void free_user_link_linker(User_Linker *head)
{
	User_Linker *p;
	while(head != NULL) {
		p = head;
		head = head->next;
		free(p);
	}
}
