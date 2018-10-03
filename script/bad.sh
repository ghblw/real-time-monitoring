#!/bin/bash
time=`date +%Y-%m-%d__%H:%M:%S`
#标记数组
declare -a vis
for i in `seq 1 10000`; do
    vis[$i]=0
done
#第一次检测
list1=`ps aux | awk '{if(($3>50)||($4>50)) print $2}'`
cnt=0
for i in $list1; do
    vis[$i]=1
    cnt=$[$cnt+1]
done
if [[ cnt -gt 0 ]]; then
    sleep 5
    #第二次检测
    list2=(`ps aux | awk '{if(($3>50)||($4>50)) printf("%s %d %s %.2f %.2f ",$11,$2,$1,$3,$4)}'`)
    cnt=0
    for i in ${list2[@]}; do
        cnt=$[$cnt+1]
    done
    p=1
    while [[ $p -lt $cnt ]]; do
        if [[ ${vis[${list2[$p]}]} -eq 1 ]]; then    #判定为恶意进程
            echo "$time ${list2[$p-1]} ${list2[$p]} ${list2[$p+1]} ${list2[$p+2]}% ${list2[$p+3]}%"
        fi
        p=$[$p+5]
    done
fi
