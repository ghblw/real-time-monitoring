#!/bin/bash
time=`date +"%Y-%m-%d %H:%M:%S"`
#分区
list=`df -m | tail -n +2 | grep -v tmp | awk '{print 1" "$6" "$2" "$4" "$5}'`
total=0
avail=0
cnt=0
#磁盘
for i in $list; do
    if [[ cnt%5 -eq 2 ]]; then
        total=$[$total+$i]
    elif [[ cnt%5 -eq 3 ]]; then
        avail=$[$avail+$i]
    fi
    cnt=$[$cnt+1]
done
cp_per=$[$[$total-$avail]*100/$total]
echo "$time 0 disk $total $avail $cp_per%"
cnt=0
for i in $list; do
    if [[ cnt%5 -eq 0 ]]; then
        echo -n "$time "
    fi
    echo -n "$i "
    if [[ cnt%5 -eq 4 ]]; then
        printf "\n"
    fi
    cnt=$[$cnt+1]
done
