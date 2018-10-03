#!/bin/bash
time=`date +"%Y-%m-%d__%H:%M:%S"`
avr_load=`uptime | awk '{print $10$11$12}' | tr "," " "`
#计算总的cpu占用率
#第一次采样
total1=0
idle1=`cat /proc/stat |head -n 1 | awk '{print $5}'`
list=`cat /proc/stat | head -n 1`
for i in $list; do
    if [[ $i = [cpu] ]]; then
        continue
    fi
    total1=$[$total1+$i]
done
#第二次采样
sleep 0.5
total2=0
idle2=`cat /proc/stat |head -n 1 | awk '{print $5}'`
list=`cat /proc/stat | head -n 1`
for i in $list; do
    if [[ $i = [cpu] ]]; then
        continue
    fi
    total2=$[$total2+$i]
done
total=$[$total2-$total1]
idle=$[$idle2-$idle1]
cpu_per=`printf "%.2f" $[100*$[$total-$idle]/$total]`
temp1=`cat /sys/class/thermal/thermal_zone0/temp`
temp=`printf "%.2f" $[$temp1/1000]`
echo -n "$time $avr_load $cpu_per $temp°C "
if [[ `echo "$cpu_per >= 0" | bc` -eq 1 && `echo "$cpu_per < 50" |bc` -eq 1 ]]; then
    echo normal
elif [[ `echo "$cpu_per >= 50" | bc` -eq 1 && `echo "$cpu_per <70" | bc` -eq 1 ]]; then
    echo note
elif [[ `echo "$cpu_per >=70" | bc` -eq 1 ]]; then
    echo warning
fi
