/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: 2020年10月29日 星期四 16时00分00秒
 ************************************************************************/

#include "../Common/common.h"
#include "../Common/head.h"
#include "./Include/heart_beat.h"
#include "./Include/sub_reactor.h"
#include "./Include/udp_server.h"
#include "./Include/user_pool.h"
#include "./Include/game.h"
#include "./Include/send_map.h"
#include "./Include/write_log.h"
#include "../Thread_pool/thread_pool.h"
#include "./Include/pretreatment.h"

char *conf = "./server.conf";
char *game_record_path = "./Log/record.log";
char *error_log_path = "./Log/error.log";
Map court;

User *rteam;
User *bteam;
User *wteam;

int flag_id[26][26] = {0};

int port = 0;
int data_port = 0;

int repollfd, bepollfd, wepollfd;
int epollfd;
    
FILE *rf;

pthread_t Re_ball, Send_map;

pthread_rwlock_t ball_lock;
pthread_mutex_t status_lock;
pthread_mutex_t file_lock;

FootBallMsg root_user;
Bpoint ball;
BallStatus ball_status;
Score score;

Thread_pool pool;

int flag_arr[50][100] = {0};
int time_val = 0;

int over_flag = 0;

void server_exit(int signalnum) {
    FootBallMsg msg;
    msg.type = FT_FIN;
    msg.flag = 1;
    msg.music = 2;
    sprintf(msg.msg, "服务端主动退出了.抱歉！");
    send_to_user(&msg, ALL_USER);
    
    write_game("Server Ctrl C.");

    fclose(rf);
    printf("服务端关闭\n");
    exit(0);
}

int main(int argc, char **argv) {
    int opt, sockfd;
    
    init_data(); //初始化数据
    
    rf = fopen(game_record_path, "a");
    
    thread_pool_init(&pool);
    
    write_game("---------------------------------------------------------------------------");
    
    write_game("Game start.");

    while ((opt = getopt(argc, argv, "p:")) != -1) {
        do_opt(opt, optarg);
    }
    argc -= (optind - 1);
    argv += (optind - 1);
    
    if (argc > 1) {
        printf("Usage : %s [-p port].", argv[0]);
        exit(1);
    }
    
    get_conf(); //从配置文件获取数据

    if ((sockfd = socket_create_udp(port)) < 0) {
        exit(1);
    }

    epollfd = epoll_create(MAX * 3);
    repollfd = epoll_create(MAX);
    bepollfd = epoll_create(MAX);
    wepollfd = epoll_create(MAX);
    
    if (epollfd < 0 || repollfd < 0 || bepollfd < 0) {
        write_log(error_log_path, "[epoll_create()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
        exit(1);
    }

    struct task_queue red_queue, blue_queue, watch_queue;
    
    task_queue_init(&red_queue, MAX, repollfd);
    task_queue_init(&blue_queue, MAX, bepollfd);
    task_queue_init(&watch_queue, MAX, wepollfd);
    
    pthread_t draw_t, red_t, blue_t, watch_t, heart_t;
    
    //创建从反应堆线程
    pthread_create(&red_t, NULL, sub_reactor, (void *)&red_queue); 
    pthread_create(&blue_t, NULL, sub_reactor, (void *)&blue_queue); 
    pthread_create(&watch_t, NULL, sub_reactor, (void *)&watch_queue); 
    
    pthread_create(&heart_t, NULL, heart_beat, NULL); //创建心跳线程

    signal(SIGINT, server_exit);

    struct epoll_event ev, events[MAX * 2];
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev);
    struct sockaddr_in c_addr;
    socklen_t len = sizeof(c_addr);
    
    add_task(&pool, write_game, "Naiting for login.");

    pthread_create(&Re_ball, NULL, re_ball, NULL); //定时计算球坐标
    pthread_create(&Send_map, NULL, send_map, NULL); //定时发送球场信息
    
    //设置一个定时器，定时执行信号，调用相应函数
    signal(14, send_time); //定时发送游戏时间
    struct itimerval itimer;
    itimer.it_interval.tv_sec = 1;
    itimer.it_interval.tv_usec = 0;
    itimer.it_value.tv_sec = 1;
    itimer.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &itimer, NULL);

    while (1) {
        int nfds = epoll_wait(epollfd, events, MAX * 2, -1);
        if (over_flag) continue;
        for (int i = 0; i < nfds; i++) {
            User user;
            FootBallMsg temp_msg;
            temp_msg.flag = 0;
            if (events[i].data.fd == sockfd) {
                int temp_fd = udp_accept(epollfd, sockfd, &user);
                //模拟TCP建立连接
                
                if (temp_fd > 0) {
                    if (user.team == 1) {
                        sprintf(temp_msg.msg, "%s 登录游戏并加入蓝队. ID : %s", user.name, user.id);
                    } else if (user.team == 0) {
                        sprintf(temp_msg.msg, "%s 登录游戏并加入红队. ID : %s", user.name, user.id);
                    } else if (user.team == 2) {
                        sprintf(temp_msg.msg, "%s 登录游戏并加入观众席. ID : %s", user.name, user.id);
                        temp_msg.flag = 1; //观众席的登录消息
                        strcpy(temp_msg.name, user.name);
                    }
                    char temp_buff[128];
                    strcpy(temp_buff, temp_msg.msg);
                    add_task(&pool, write_game, (void *)&temp_buff);
                    
                    temp_msg.type = FT_WALL;
                    temp_msg.music = 1;
                    send_to_user(&temp_msg, ALL_USER); 
                    //客户登录信息发送给所有人
                    
                    sprintf(temp_msg.msg, "欢迎你进入游戏, 你的ID是 : %s.", user.id);
                    temp_msg.music = -1;
                    send(user.fd, (char *)&temp_msg, sizeof(temp_msg), 0);
                    //发给登录客户欢迎信息
                    
                    add_to_sub_reactor(&user);
                    //将登录客户加入从反应堆
                }
            }
        }
    } 
    return 0;
}
