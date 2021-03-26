/*************************************************************************
	> File Name: pretreatment.c
	> Author: 
	> Mail: 
	> Created Time: 2020年11月29日 星期日 16时37分41秒
 ************************************************************************/

#include "../../Common/head.h"
#include "../../Common/datatype.h"
#include "../../Common/common.h"
#include "../Include/music.h"

extern char *conf;

extern Map court;

extern int server_port;
extern char server_ip[20];
extern int sleep_speed;

extern int music_id;

extern User_t *rteam, *bteam;
extern FootBallMsg root_user, chat_msg, ctl_msg;

extern LogRequest request;

int bgm_flag;
int sound_effect_flag;
int mus = 8;

void do_opt(int opt, const char *optarg) {
   	switch (opt) {
        case 'h':
            strcpy(server_ip, optarg);
            break;
        case 'p':
            server_port = atoi(optarg);
            break;
        case 'n':
            strcpy(request.name, optarg);
            break;
        case 't':
            request.team = atoi(optarg);
            break;
        case 'm':
            strcpy(request.msg, optarg);
            break;
        default:
            printf("opt error.\n");
            exit(1);
    }
    return ;
}

void get_conf() {
	memset(&server_ip, 0, sizeof(server_ip));
    //从配置文件读取数据
    if (!strlen(server_ip)) strcpy(server_ip, get_conf_value(conf, "SERVERIP"));
    if (!server_port) server_port = atoi(get_conf_value(conf, "SERVERPORT"));
    if (!strlen(request.name)) strcpy(request.name, get_conf_value(conf, "NAME"));
    if (!strlen(request.msg)) strcpy(request.msg, get_conf_value(conf, "MSG"));
    if (request.team == 2) request.team = atoi(get_conf_value(conf, "TEAM"));
    bgm_flag = atoi(get_conf_value(conf, "BGM"));
    sound_effect_flag = atoi(get_conf_value(conf, "SOUND"));
    sleep_speed = atoi(get_conf_value(conf, "SPEED"));
    sleep_speed = 200000 - (1000 * sleep_speed);
    
    court.width = atoi(get_conf_value(conf, "COLS"));
    court.heigth = atoi(get_conf_value(conf, "LINES"));

	return ;
}

void init_data() {
   	bzero(&request, sizeof(request));
    request.team = 2;
    bzero(&chat_msg, sizeof(FootBallMsg));
    bzero(&ctl_msg, sizeof(FootBallMsg));
    ctl_msg.flag = 1;
    sprintf(root_user.name, "root"); //初始化管理员对象

    rteam = (User_t *)calloc(MAX, sizeof(User_t));
    bteam = (User_t *)calloc(MAX, sizeof(User_t));
    
    music_id = 0;

    chat_msg.type = FT_MSG;
    ctl_msg.type = FT_CTL;

    court.start.x = 1;
    court.start.y = 1;

 	return ;
}

void start_sound() {
    if (bgm_flag) {
        pthread_t Bgm;
        pthread_create(&Bgm, NULL, bgm, NULL);    
    }

    if (sound_effect_flag) {
        pthread_t Voice;
        pthread_create(&Voice, NULL, play_music, (void *)&mus);    
    }
    return ;
}
