#!/bin/bash
if [[ ! -e /etc/master_yxw.pid ]]; then
    touch /etc/master_yxw.pid
fi

pre_pid=`cat /etc/master_yxw.pid`
if test -n $pre_pid ; then
    ps -ef | grep -w ${pre_pid} | grep master_yxw > /dev/null
    if [[ $? == 0 ]]; then
        echo "Pihealthd has already started."
        exit 0
    else 
        echo "Pihealthd is starting."
        cd /home/yuxiaowei/real-time-monitoring/master/
        ./master_yxw
        echo "Pihealthd started."
    fi
else
    echo "Pihealthd is starting."
    cd /home/yuxiaowei/real-time-monitoring/master/
    ./master_yxw
    echo "Pihealthd started."
fi
pid=`ps -ef | awk '{if ($8 == "./master_yxw") print $2}'`
echo $pid > /etc/master_yxw.pid
