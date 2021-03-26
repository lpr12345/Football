/*************************************************************************
	> File Name: write_log.c
	> Author: 
	> Mail: 
	> Created Time: 2020年11月11日 星期三 19时32分29秒
 ************************************************************************/

#include "../../Common/head.h"
#include "../../Common/datatype.h"

extern pthread_mutex_t file_lock;
extern FILE *rf;

void *write_game(void *arg) {
    time_t time_now = time(NULL);
    struct tm* t = localtime(&time_now);
    char *buff = (char *)arg;
    char temp_buff[256] = {0};

    pthread_mutex_lock(&file_lock);
    
    sprintf(temp_buff, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n", (1900 + t->tm_year), (1 + t->tm_mon), t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, buff);
    fwrite(temp_buff, 1, strlen(temp_buff), rf);
    
    pthread_mutex_unlock(&file_lock);
}

int write_log(char *path, const char *format, ...) {
    va_list arg;
    va_start(arg, format);
    time_t timep;
    time(&timep);
    struct tm *p;
    p = gmtime(&timep);
    FILE *fp = fopen(path, "a+");
    fprintf(fp, "[%02d-%02d-%02d ", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday);
    fprintf(fp, "%02d:%02d:%d] ", (p->tm_hour + 8), p->tm_min, p->tm_sec);
    int k = vfprintf(fp, format, arg);
    fprintf(fp, "\n");
    fclose(fp);
    va_end(arg);
    return k;
}

