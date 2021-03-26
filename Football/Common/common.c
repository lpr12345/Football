/*************************************************************************
	> File Name: common.c
	> Author: 
	> Mail: 
	> Created Time: 2020年11月09日 星期一 12时31分08秒
 ************************************************************************/

#include "./datatype.h"
#include "./head.h"

char *get_conf_value(char *path, char *key) {
   	FILE *fp = NULL;
    char *line = NULL, *substr = NULL, *value = NULL;
    value = (char *)malloc(20);
    memset(value, 0, 20);
    size_t n = 0, len = 0;
    ssize_t read;

    if (key == NULL) {
        return NULL;
    }
    fp = fopen(path, "r");
    if (fp == NULL){
        perror("fopen");
        return NULL;
    }
    while((read = getline(&line, &n, fp)) != -1){
        substr = strstr(line, key);
        //strstr（在谁里面， 找谁)
        if (substr == NULL){
            continue;
        } else {
            len = strlen(key);
            if (line[len] == '='){
                strncpy(value, line + len + 1, (int)read - len - 2);
                //strncpy(放到哪， 从哪开始拷贝，拷贝的长度)
                break;
            } else {
                continue;
            }
        }
    
    }
    if(substr == NULL) {
        fclose(fp);
        free(line);
        return NULL;
    }

    fclose(fp);
    free(line);
    return value;
}

