#!/bin/sh

if [ $# -ne 2 ];  then
    echo "Usage:\n\t$0 ip log_file"
    exit 1
fi

while [ 0 ]
do
    ping -c 1 $1
    if [ $? -ne 0 ]; then
        echo "$1 failed at `date`" >> $2
    fi
    sleep 1
done
