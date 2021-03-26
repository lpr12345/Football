#!/bin/bash

if [[ $# -eq 0 ]]; then
    echo "Usage: run client.c or server.c"
    exit
fi

name=$1
canshu=$@

temp=$1
canshu=${canshu:8}

if [[ $name = "server.c" ]]; then
    gcc ./server.c ../Thread_pool/thread_pool.c ./Src/pretreatment.c ./Src/write_log.c ../Common/common.c  ./Src/user_pool.c ./Src/heart_beat.c ./Src/udp_server.c ./Src/sub_reactor.c ./Src/game.c ./Src/send_map.c -lpthread -lcurses -lm -o SERVER 
    ./SERVER $canshu
else 
    echo 'Without this file name'
    exit
fi

