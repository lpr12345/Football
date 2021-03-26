/*************************************************************************
	> File Name: music.c
	> Author: 
	> Mail: 
	> Created Time: 2020年11月27日 星期五 11时22分14秒
 ************************************************************************/
#include "../../Common/datatype.h"
#include "../../Common/head.h"
#include "../Include/game_map.h"
#include "../Include/draw_game.h"

extern WINDOW *Write_win;
extern struct dirent **namelist;
extern int music_cnt;
extern int music_id;
extern int current_music_id;
extern int music_ind;
extern int over_flag;
extern int exit_flag;
extern int music_dir;
extern int current_page;
extern int max_page;
extern int last_music_id;
extern pthread_mutex_t music_lock;
extern pthread_mutex_t stop_music_lock;

void *bgm(void *arg) {
    //int last_id = 0;
    while (1) {
        if (over_flag) break;

        char temp_opt_buff[512] = {0};
       
        draw_now_music(1);

        pthread_mutex_lock(&music_lock);
        sprintf(temp_opt_buff, "play ./Bgm/%s 2>/dev/null", namelist[music_id]->d_name);
        pthread_mutex_unlock(&music_lock);
        
        system(temp_opt_buff);
        
        pthread_mutex_lock(&stop_music_lock);

        pthread_mutex_lock(&music_lock);
        last_music_id = music_id;
        if (current_music_id == -1) {
            //last_id = music_id % 44;
            music_id = music_id + 1;
        
            if (music_id == music_cnt) music_id = 0;
        } else {
            music_id = current_music_id;
            current_music_id = -1;
        }
        pthread_mutex_unlock(&music_lock);
        
        pthread_mutex_unlock(&stop_music_lock);
    }
}

void *play_music(void *arg) {
    int *flag = (int *)arg;
    switch (*flag) {
        case 1 :
            system("play ./Voice/1_你好.mp3 2>/dev/null");
            break;
        case 2 : 
            system("play ./Voice/2_拜拜.mp3 2>/dev/null");
            break;
        case 3 : 
            system("play ./Voice/3_牛逼.mp3 2>/dev/null");
            break;
        case 4 : 
            system("play ./Voice/4_听得见吗.mp3 2>/dev/null");
            break;
        case 5 : 
            system("play ./Voice/5_诶嘿听得见吗.mp3 2>/dev/null");
            break;
        case 6 : 
            system("play ./Voice/6_不多比比.mp3 2>/dev/null");
            break;
        case 7 : 
            system("play ./Voice/7_跳.mp3 2>/dev/null");
            break;
        case 8 :
            system("play ./Voice/8_伞兵一号.mp3 2>/dev/null");
            break;
    }
}

