#!/bin/bash
pid=`cat /etc/master_yxw.pid`

if test -n $pid ;then
    ps -ef | grep -w ${pid} | grep master_yxw > /dev/null
    if [[ $? == 0 ]]; then
        kill -9 ${pid}
        echo "Pihealthd has stopped."
    else
        echo "Pihealthd has not been opened"

    fi
fi
