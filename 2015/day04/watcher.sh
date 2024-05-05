#!/bin/bash

OPTIMIZATION_LEVEL=$1
if [ -z "$OPTIMIZATION_LEVEL" ]; then
    OPTIMIZATION_LEVEL=3
fi

while inotifywait -e modify main.cpp; do
clear;
    g++ -g -O$OPTIMIZATION_LEVEL -Wall -Werror -fsanitize=address -fno-omit-frame-pointer -std=c++20 -lcryptopp main.cpp -o app || continue
    ARGFILE=$(cat filewatch.txt | xargs)
    for i in $ARGFILE; do
        EXPECTED_PATH=$(echo $i | sed 's/\.[^.]*$//')_expected.txt  
        if [ ! -f $EXPECTED_PATH ]; then
            echo "Expected file $EXPECTED_PATH not found"
            continue
        fi    
        echo -ne "\n\n Running $i\n\n"
        timeout 30 time ./app $i $EXPECTED_PATH
        echo "----------------------------------------"
    done
done

 