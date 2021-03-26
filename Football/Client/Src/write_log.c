/*************************************************************************
	> File Name: write_log.c
	> Author: 
	> Mail: 
	> Created Time: 2020年11月27日 星期五 09时36分03秒
 ************************************************************************/
#include "../../Common/head.h"
#include "../../Common/datatype.h"

extern pthread_mutex_t file_lock;

int write_log(char *path, const char *format, ...) {
    va_list arg;
    va_start(arg, format);
    time_t timep;
    time(&timep);
    struct tm *p;
    p = gmtime(&timep);
    FILE *fp = fopen(path, "a+");
    fprintf(fp, "[%04d-%02d-%02d ", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday);
    fprintf(fp, "%02d:%02d:%02d] ", (p->tm_hour + 8), p->tm_min, p->tm_sec);
    int k = vfprintf(fp, format, arg);
    fprintf(fp, "\n");
    fclose(fp);
    va_end(arg);
    return k;
}

