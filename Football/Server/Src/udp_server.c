/*************************************************************************
	> File Name: udp_server.c
	> Author: 
	> Mail: 
	> Created Time: 2020年11月09日 星期一 11时27分56秒
 ************************************************************************/

#include "../../Common/head.h"
#include "../../Common/datatype.h"
#include "../Include/write_log.h"

extern User *rteam, *bteam, *wteam;
extern int flag_id[26][26];
extern Map court;
extern int flag_arr[50][100];
extern int data_port;
extern char *error_log_path;

int socket_create_udp(int port) {
    int sockfd;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (sockfd < 0) {
        write_log(error_log_path, "[socket()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
        return -1;
    }

    unsigned long ul = 1;
    ioctl(sockfd, FIONBIO, &ul); //设置非阻塞
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    int opt = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        write_log(error_log_path, "[setsockopt()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
        return -1;
    } 
    // 设置端口复用
    
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        write_log(error_log_path, "[bind()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
        return -1;
    }
    return sockfd;
}

int socket_udp() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        write_log(error_log_path, "[socket()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
        return -1;
    }
    return sockfd;
}

void add_event(int epollfd, int fd, int events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        write_log(error_log_path, "[epoll_ctl()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
        exit(1);
    }
    return ;
}

void add_event_ptr(int epollfd, int fd, int events, User *user) {
    struct epoll_event ev;
    ev.events = events;

    ev.data.ptr = (void *)user;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        write_log(error_log_path, "[epoll_ctl()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
        exit(1);
    }
    return ;
}

void del_event(int epollfd, int fd) {
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL) < 0) {
        write_log(error_log_path, "[epoll_ctl()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
        exit(1);
    }
    return ;
}

int udp_connect(struct sockaddr_in *serveraddr) {
    int sockfd;
    if ((sockfd = socket_create_udp(data_port)) < 0) {
        write_log(error_log_path, "[socket_create_udp()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
        return -1;
    }
    
    if (connect(sockfd, (struct sockaddr *)serveraddr, sizeof(struct sockaddr)) < 0) {
        write_log(error_log_path, "[connect()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
        return -1;
    }
    if (sendto(sockfd, "connect", sizeof("connect"), 0, NULL, 0) < 0) {
        write_log(error_log_path, "[sendto()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
        return -1;
    }
    return sockfd;
}

int check_online(struct LogRequest *request) {
    for (int i = 0; i < MAX; i++) {
        if (bteam[i].online && !strcmp(bteam[i].name, request->name)) {
            return 1;
        }
        if (rteam[i].online && !strcmp(rteam[i].name, request->name)) {
            return 1;
        }
        if (wteam[i].online && !strcmp(wteam[i].name, request->name)) {
            return 1;
        }
    }
    return 0;
}

int udp_accept(int epollfd, int fd, User *user) {
    struct sockaddr_in c_addr;
    int new_fd, ret;
    socklen_t len = sizeof(c_addr);
    LogRequest request;
    LogResponse response;

    ret = recvfrom(fd, (void*)&request, sizeof(request), 0, (struct sockaddr *)&c_addr, &len);
    
    if (ret != sizeof(request)) {
        response.type = 1;
        strcpy(response.msg, "Login failed or network erros!");
        sendto(fd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&c_addr, len);
        return -1;
    }
    
    if (check_online(&request)) {
        response.type = 1;
        strcpy(response.msg, "You are alreadly playing this game somewhere!");
        sendto(fd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&c_addr, len);
        return -1;
    }
    response.type = 0;
    response.num = 0;
    for (int i = 0; i < MAX; i++) {
        if (wteam[i].online) strcpy(response.name[response.num++], wteam[i].name);
    }

    if (sendto(fd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&c_addr, len) < 0) {
        write_log(error_log_path, "[sendto()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
        return -1;
    }
    
    strcpy(user->name, request.name);
    user->team = request.team;
    user->score = 0;
    user->own = 0;
    if (user->team == 1) {
        user->loc.x = court.start.x + court.width - 4;
        user->loc.y = 1;
        flag_arr[user->loc.y][user->loc.x] = 1;
    } else if (user->team == 0){
        user->loc.x = court.start.x + 1;
        user->loc.y = 1;
        flag_arr[user->loc.y][user->loc.x] = 1;
    }

    if (user->team != 2) {
        for (int i = 0; i < 26; i++) {
            int temp_flag = 0;
            for (int j = 0; j < 26; j++) {
                if (!flag_id[i][j]) {
                    sprintf(user->id, "%c%c", 'A' + i, 'A' + j);
                    flag_id[i][j] = 1;
                    temp_flag = 1;
                    break;
                }
            }
            if (temp_flag) break;
        }
    }
    new_fd = udp_connect(&c_addr);
    user->fd = new_fd;
    return new_fd;
}

