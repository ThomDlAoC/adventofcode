#!/bin/bash


ARGFILE="$@"

if [ -z "$ARGFILE" ]; then
    echo "Usage: $0 <argfile>"
    exit 1
fi

while inotifywait -e modify main.cpp; do
clear;
    g++ -g -O3 -Wall -Werror -fsanitize=address -fno-omit-frame-pointer -std=c++17 main.cpp -o app || continue
    for i in $ARGFILE; do
        ./app $i
        echo "----------------------------------------"
    done
done

 