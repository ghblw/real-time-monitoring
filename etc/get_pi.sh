for i in `seq 1 10`; do 
    scp -r real-time-monitoring/ yuxiaowei@pi$i:.
    ssh yuxiaowei@pi$i "gcc -o real-time-monitoring/client/a.out real-time-monitoring/client/client1.c -pthread"	#执行脚本 
done
