/*************************************************************************
	> File Name: udp_client.c
	> Author: 
	> Mail: 
	> Created Time: 2020年11月09日 星期一 15时32分00秒
 ************************************************************************/

#include "../../Common/datatype.h"
#include "../../Common/head.h"
#include "../Include/write_log.h"

extern char *error_log_path;

int socket_create_udp(int port) {
    int sockfd;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    int opt = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        perror("setsockopt");
        return -1;
    } // 设置端口复用
    

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

