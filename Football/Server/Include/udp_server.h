/*************************************************************************
	> File Name: udp_server.h
	> Author: 
	> Mail: 
	> Created Time: 2020年11月09日 星期一 11时25分59秒
 ************************************************************************/

#ifndef _UDP_SERVER_H
#define _UDP_SERVER_H

#include "../../Common/head.h"
#include "../../Common/datatype.h"

int socket_create_udp(int port);
int socket_udp();
void add_event(int epollfd, int fd, int events);
void add_event_ptr(int epollfd, int fd, int events, User *user);
void del_event(int epollfd, int fd);
int udp_connect(struct sockaddr_in *serveraddr);
int udp_accept(int epollfd, int fd, User *user);


#endif
