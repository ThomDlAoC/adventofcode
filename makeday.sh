#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: makeday.sh <year> <day>"
    exit 1
fi

YEAR=$1
DAY=$2
DAY_FORMAT=$(printf "%02d" $DAY)

mkdir -p $YEAR
cp -rf day_template $YEAR/day$DAY_FORMAT