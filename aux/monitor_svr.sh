#!/bin/bash

MAX_THREAD_NUM=40
MIN_THREAD_NUM=3
MAX_CPU_USAGE=75

PROG_PATH=/opt/cumulus/
PROG_NAME=CumulusServer

echo "checking $PROG_NAME ..."
result=`ps -ef | grep "$PROG_NAME"| grep -v "grep" | grep -v "monitor" | wc -l`
if [ $result -gt 0 ]; then
	result=`top -b -n1 | grep $PROG_NAME | grep -v grep | grep -v monitor`
	read pid USER PR  NI  VIRT  RES  SHR S cpu MEM TIME cmd <<<$(echo $result)
	hu=`echo $cpu '>=' $MAX_CPU_USAGE | bc -l`
	thread_num=`ps -mp $pid | grep -v "TTY" | grep -v "$pid" | wc -l | bc -l`
	if (($hu == 1 || $thread_num <= $MIN_THREAD_NUM || $thread_num >= $MAX_THREAD_NUM)) ; then
		kill -9 $pid 
		echo "$PROG_NAME is killed!!"
		echo "pid=$pid, cpu=$cpu, hu=$hu, thread=$thread_num, cmd=$cmd"
	fi
fi

result=`ps -ef | grep "$PROG_NAME"| grep -v "grep" | grep -v "monitor" | wc -l`
if [ $result -eq 0 ]; then
	sleep 5
	DT=`date +"%Y/%m/%d %H:%M:%S"`
	echo "$PROG_NAME restarted at: "$DT
	export POCO_ENABLE_DEBUGGER=yes;
    cd ${PROG_PATH};
	nohup ${PROG_PATH}/$PROG_NAME &
fi
