/*************************************************************************
	> File Name: sign_in.c
	> Author: 
	> Mail: 
	> Created Time: 2020年11月29日 星期日 20时43分36秒
 ************************************************************************/

#include "../Include/udp_client.h"
#include "../../Common/head.h"
#include "../../Common/datatype.h"
#include "../Include/write_log.h"

extern char *error_log_path;

extern int server_port;
extern int sockfd;
extern char server_ip[20];

extern LogRequest request;
extern LogResponse response;
extern Watch_user watch_user[MAX];

void sign_in() {
	if ((sockfd = socket_udp()) < 0) {
        exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_port);  
    addr.sin_addr.s_addr = inet_addr(server_ip);
    socklen_t len = sizeof(addr);

    //发送登录信息
    if (sendto(sockfd, (void *)&request, sizeof(request), 0, (struct sockaddr *)&addr, len) < 0) {
        write_log(error_log_path, "[socket_create_udp()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));     
        exit(1);
    }
    
    fd_set set;
    FD_ZERO(&set);
    FD_SET(sockfd, &set);
    struct timeval tm_val;
    tm_val.tv_sec = 2;
    tm_val.tv_usec = 0;

    int ret;
    //select控制时间，超时证明服务端不在，退出
    ret = select(sockfd + 1, &set, NULL, NULL, &tm_val);
    
    if (ret == 0) {
        printf("服务端不在线..QAQ\n");
        exit(0);
    } else if (ret < 0) {
        write_log(error_log_path, "[select()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));     
        exit(1);
    }

    //接受是否登录成功相关信息
    ret = recvfrom(sockfd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&addr, &len);

    if (ret != sizeof(response) || response.type) {
        write_log(error_log_path, "[recv_from()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));     
        exit(1);
    }
    
    for (int i = 0; i < MAX; i++) {
        if (i >= response.num) {
            watch_user[i].online = 0;
            continue;
        }
        watch_user[i].online = 1;
        strcpy(watch_user[i].name, response.name[i]);
    }

    //建立传输数据相关连接
    char temp_msg[10] = {0};
    recvfrom(sockfd, (void *)&temp_msg, sizeof(temp_msg), 0, (struct sockaddr *)&addr, &len);
    
    connect(sockfd, (struct sockaddr *)&addr, len);
    
    return ;
}
