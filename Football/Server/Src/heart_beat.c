/*************************************************************************
	> File Name: heart_beat.c
	> Author: 
	> Mail: 
	> Created Time: 2020年11月04日 星期三 19时57分45秒
 ************************************************************************/

#include "../../Common/head.h"
#include "../../Common/datatype.h"
#include "../../Common/common.h"
#include "../Include/udp_server.h"
#include "../Include/write_log.h"
#include "../../Thread_pool/thread_pool.h"
#include "../Include/send_map.h"

extern User *bteam, *rteam, *wteam;
extern int bepollfd, repollfd, wepollfd;
extern FootBallMsg root_user;

extern Thread_pool pool;

extern int flag_id[26];

void heart_beat_team(User *team, int epollfd_tmp) {
    FootBallMsg msg;
    msg.music = 0;
    msg.type = FT_TEST;
    for (int i = 0; i < MAX; i++) {
        if (team[i].online) {
            if (!team[i].flag) {
                FootBallMsg temp_msg;
                temp_msg.flag = 0;

                if (team[i].team == 2) {
                    temp_msg.flag = 2;
                    strcpy(temp_msg.name, team[i].name);
                }

                sprintf(temp_msg.msg, "%s 已经失联了.", team[i].name);
                send_to_user(&temp_msg, ALL_USER);

                add_task(&pool, write_game, (void *)&temp_msg.msg);
                
                flag_id[team[i].id[0] - 'A'] = 0;

                team[i].online = 0;
                del_event(epollfd_tmp, team[i].fd);
                continue;
            }
            send(team[i].fd, (void *)&msg, sizeof(msg), 0);
            team[i].flag--;
        }
    }
}

void *heart_beat(void *arg) {
    while (1) {
        sleep(5);
        heart_beat_team(bteam, bepollfd);
        heart_beat_team(rteam, repollfd);
        heart_beat_team(wteam, wepollfd);
    }
}
