#!/bin/bash
date=`date +"%Y-%m-%d__%H:%M"`
total_used=(`free -m | head -n 2 | tail -n 1 | awk '{printf("%s %s %s\n",$2,$3,$4) }'`)
now_per=`echo "scale=1;${total_used[1]}*100/${total_used[0]}" | bc`
avr_per=`echo "scale=1;0.3*$1+0.7*$now_per" | bc`
echo "$date ${total_used[0]}M ${total_used[2]}M $now_per% $avr_per%"
