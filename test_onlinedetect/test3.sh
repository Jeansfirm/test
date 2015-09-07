#!/bin/sh

while [ 0 ]
do
    ping -c 1 192.168.1.172 &
    ping -c 1 192.168.1.172

    if [ $? -ne 0 ]; then
         echo "error\n\n" >> log
    fi
    sleep 2
done
