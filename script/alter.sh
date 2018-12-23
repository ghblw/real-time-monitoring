#!/bin/bash

Date=`date "+%Y-%m-%d %H:%M"`
User=`uname -a | awk '{print $2}'`
`df -m | grep -v "tmpfs" | tail -n +2 > txt`
Disksum=`cat txt | awk '{print $2}' | echo $[ $( tr "\n" "+"  ) 0   ]`
Diskused=`cat txt | awk '{print $3}' | echo $[ $( tr "\n" "+"  ) 0   ]`
Diskpercent=`awk 'BEGIN{printf "%d\n",('$Diskused'/'$Disksum') * 100}'`
Mem=`free -m |grep "Mem" | awk '{print $2}'`
Memused=`free -m | grep "Mem" | awk '{print $3}'`
Mempercent=`awk 'BEGIN{printf"%d\n",('$Memused'/'$Mem')*100}'`
CPU=`cat /sys/class/thermal/thermal_zone0/temp | cut -c -2`
`ps -aux --sort -pcpu | awk '$3 > 90 || $4>90 {print ;}' > txt`
sleep 5
`ps -aux --sort -pcpu | awk '$3 > 90 || $4>90 {print ;}' >> txt`
`cat txt | sort -nk 2 | awk '{print $2, $11}' | uniq -d | awk '{print $2}' > alt_p.log`

if [[ $Diskpercent -gt 90 ]];then
    echo $Date $User DISK Warning DISK usage $Diskpercent%  
fi
if [[ $CPU -gt 70 ]]; then
    echo $Date $User CPU Warning CPU temperature $CPU°C  
fi
if [[ $Mempercent -gt 80 ]]; then
    echo $Date $User Mem Warning Mem usage $Mempercent% 
fi
for i in `cat alt_p.log`; do
    echo $Date $User Proc Warning danger Proc "$i" 
done



