/*************************************************************************
	> File Name: pretreatment.c
	> Author: 
	> Mail: 
	> Created Time: 2020年11月29日 星期日 21时43分53秒
 ************************************************************************/

#include "../../Common/head.h"
#include "../../Common/datatype.h"
#include "../../Common/common.h"

extern char *conf;

extern Map court;

extern User *rteam;
extern User *bteam;
extern User *wteam;

extern FILE *rf;

extern int port;
extern int data_port;

extern int repollfd, bepollfd;
extern int epollfd;

extern pthread_rwlock_t ball_lock;
extern pthread_mutex_t status_lock;
extern pthread_mutex_t file_lock;
extern FootBallMsg root_user;
extern Bpoint ball;
extern BallStatus ball_status;
extern Score score;

extern int time_val;

void do_opt(int opt, const char *optarg) {
	switch (opt) {
        case 'p': 
            port = atoi(optarg);
            break;
        default:
            printf("Usage :  [-p port].\n");
            exit(1);
    }

    return ;
}

void get_conf() {
	if (!port) port = atoi(get_conf_value(conf, "SERVERPORT"));
    time_val = atoi(get_conf_value(conf, "GAMETIME"));
    time_val *= 60; 
    data_port = atoi(get_conf_value(conf, "DATAPORT"));
    court.width = atoi(get_conf_value(conf, "COLS"));
    court.heigth = atoi(get_conf_value(conf, "LINES"));
    ball.x = court.width / 2;
    ball.y = court.heigth / 2;
    return ;
}

void init_data() {
    pthread_mutex_init(&file_lock, NULL);
    pthread_rwlock_init(&ball_lock, NULL);
    pthread_mutex_init(&status_lock, NULL);

    court.start.x = 1;
    court.start.y = 1;
     
    ball_status.v = 0;
    ball_status.a = 0;
    ball_status.flag_team = 2;
    ball_status.out_flag = 0;

    rteam = (User *)calloc(MAX, sizeof(User));
    bteam = (User *)calloc(MAX, sizeof(User));
    wteam = (User *)calloc(MAX, sizeof(User));

    sprintf(root_user.name, "root");
    root_user.music = 0;

    return ;
}
