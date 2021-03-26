/*************************************************************************
	> File Name: datatype.h
	> Author: 
	> Mail: 
	> Created Time: 2020年11月01日 星期日 15时18分13秒
 ************************************************************************/

#ifndef _DATATYPE_H
#define _DATATYPE_H
#include "head.h"

typedef struct File_buff {
    FILE *file;
    char buff[256];
} File_buff;

typedef struct LogRequest { //客户端登陆用的登录包
    char name[20];
    int team;// 0 RED 1 BLUE
    char msg[256];
} LogRequest;

typedef struct LogResponse { //服务端回复客户端登录的包
    int type; // 0 : success 1 : failed
    int num;
    char msg[256];
    char name[MAX][20];
} LogResponse;

typedef struct Point {
    int x;
    int y;
} Point;

typedef struct User { //用户
    int team;
    int fd;
    int online;
    int flag; 
    int score;
    int own;
    char id[2];
    struct Point loc;
    char name[20];
} User;

typedef struct User_t {
    int team;
    int online;
    char id[2];
    struct Point loc;
} User_t;

typedef struct Watch_user {
    int online;
    char name[20];
} Watch_user;

typedef struct Map {
    int width;
    int heigth;
    struct Point start;
} Map;

typedef struct Ctrl {
    int dirx;
    int diry;
} Ctrl;

typedef struct Dir {
    int x;
    int y;
} Dir;

typedef struct Bpoint {
    double x;
    double y;
} Bpoint;

typedef struct BallStatus { //球的状态
    double v; //球的速度
    double a; //球的加速度
    int by_team; //哪个队伍踢的
    int flag_team; //哪个队伍发球
    int out_flag;
    struct Dir dir; //方向
    User *by_user;
} BallStatus;

typedef struct Score { //比分
    int red;
    int blue;
} Score;

#define FT_TEST 0x01  //服务端心跳
#define FT_WALL 0x02  //服务端广播
#define FT_MSG 0x04  //用户发送的信息和服务端转发的信息
#define FT_ACK 0x08  //客户端对心跳的确认
#define FT_FIN 0x10  //客户端、服务端下线前对彼此的通知
#define FT_CTL 0x20  //客户端发送的控制信息
#define FT_GAME 0x40  //服务端向客户端广播的实时地图
#define FT_SCORE 0x80  //服务端对客户端发送的游戏比分变化
#define FT_GAMEOVER 0x100  //游戏结束
#define FT_TIME 0x200 //发送时间

typedef struct FootBallMsg {
    int type; //什么类型的数据
    int strength; //踢球的力
    int team; //来自哪个队伍的成员 0 : red  1 : blue  2 : 观众
    int flag;
    //type为FT_MSG时 1:发给全体 2:发给队伍 3:发给个人
    //type为FT_CTL时 1:控制方向 2:踢球 3:停球
    //type为FT_TIME时 代表时间
    //type为FT_FIN时 1:服务端主动退出 2:时间到游戏结束
    //type为FT_WALL时 1:代表有人登陆 0:正常
    int music;
    char to_name[20]; //发送给谁
    char name[20]; //来自谁
    struct Score score; //比分
    struct Point ball; //球的坐标
    struct Ctrl ctl; //控制方向
    struct User_t rteam[MAX];
    struct User_t bteam[MAX];
    char msg[256]; //信息
} FootBallMsg;

typedef struct Echo_user_msg {
    User *user;
    FootBallMsg foot_ball_msg;
} Echo_user_msg;

typedef struct Exit_user_msg {
    struct User rteam[MAX];
    struct User bteam[MAX];
} Exit_user_msg;

#endif
