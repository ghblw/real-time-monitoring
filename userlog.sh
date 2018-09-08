#!/bin/bash
#时间
date=`date +"%Y-%m-%d__%H:%M"`
users=`last | grep [a-zA-Z] | grep -v wtmp | cut -d " " -f 1 | sort | uniq`    #近期登陆所有用户
echo -n "$date "
#用户总数（非系统用户）uid>500
total=0
for i in $users; do
    if [[ `id -u $i 2>/dev/null` -gt 500 ]]; then
        total=$[$total+1]
    fi
done
echo -n "$total "
#近期活跃用户（3个）sort
rank=`last | grep [a-zA-Z] | grep -v wtmp | cut -d " " -f 1 |sort |uniq -c | sort -t " " -k 1 -r | awk '{printf("%s\n",$2)}'`
k=0
echo -n "["
for i in $rank; do
    echo -n "$i"
    k=$[$k+1]
    if [[ $k -eq 3 ]]; then 
        break;
    fi
    echo -n ","
done
echo -n "] "
#具有root权限用户
echo -n "["
for i in $users; do
    id $i 2>/dev/null | grep sudo 1>/dev/null  
    if [[ $? -eq 0 ]]; then 
        echo -n $i
    fi
done
echo -n "] "
#当前在线用户 IP TTY
echo -n "["
now_user=`w -h | awk '{printf("%s_%s_%s\n",$1,$3,$2)}'`
echo -n $now_user
echo "]"
