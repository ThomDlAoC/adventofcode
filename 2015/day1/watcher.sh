#!/bin/bash

while inotifywait -e modify main.cpp; do
clear;
    g++ -g -O3 -Wall -Werror -fsanitize=address -fno-omit-frame-pointer -std=c++20 main.cpp -o app || continue
    ARGFILE=$(cat filewatch.txt | xargs)
    for i in $ARGFILE; do
        EXPECTED_PATH=$(echo $i | sed 's/\.[^.]*$//')_expected.txt  
        if [ ! -f $EXPECTED_PATH ]; then
            echo "Expected file $EXPECTED_PATH not found"
            continue
        fi    
        echo -ne "\n\n Running $i\n\n"
        timeout 10 time ./app $i $EXPECTED_PATH
        echo "----------------------------------------"
    done
done

 