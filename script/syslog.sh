#!/bin/bash
time=`date +"%Y-%m-%d__%H:%M:%S"`
hostname=`whoami`
os=`uname -o`
kernel=`uname -r`
uptime=`uptime -p | tr " " "_"`
avr_load=`uptime | awk '{print $10$11$12}'|tr "," " "`
#磁盘
cp_size=(`df -m | tail -n +2 | grep -v tmp |awk '{total+=$2;used+=$3} END{printf "%d %d",total,used}'`)
cp_per=$[${cp_size[1]}*100/${cp_size[0]}]
#内存
mem_size=(`free -m |head -n 2 | tail -n 1 | awk '{print $2" "$3}'`)
mem_per=$[${mem_size[1]}*100/${mem_size[0]}]
#echo -n ${mem_size[0]} $mem_per"%"
#CPU温度
#temp=$[`cat /sys/class/thermal/thermal_zone0/temp`/1000]
temp1=`cat /sys/class/thermal/thermal_zone0/temp`
temp=`printf "%.2f" $[$temp1/1000]`
#磁盘、内存、CPU报警级别
echo -n "$time $hostname $os $kernel $uptime $avr_load ${cp_size[0]} $cp_per% ${mem_size[0]} $mem_per% "   
if [[ $cp_per -gt 0 && $cp_per -lt 80 ]]; then
    echo -n normal" "
elif [[ $cp_per -lt 90 ]]; then
    echo -n note" "
elif [[ $cp_per -lt 100 ]]; then
    echo -n warning" "
fi
if [[ $temp1 -gt 0 && $temp1 -lt 50000 ]]; then
    echo -n normal" "
elif [[ $temp1 -lt 70000 ]]; then
    echo -n note" "
elif [[ $temp1 -lt 100000 ]]; then
    echo -n warning" "
fi
if [[ $mem_per -gt 0 && $mem_per -lt 70 ]]; then
    echo normal" "
elif [[ $mem_per -lt 80 ]]; then
    echo  note" "
elif [[ $mem_per -lt 100 ]]; then
    echo  warning" "
fi
