#!/bin/bash

CWD=`pwd`
TARGET_ARCH=$1

DIRS=`find $CWD/$TARGET_ARCH/ -name Makefile -exec "realpath" "{}" \;`
DIRS=`dirname $DIRS`

for DIR in $DIRS; do
    if [ "$DIR" == "$CWD" ]; then
        continue
    fi

    echo $DIR
done