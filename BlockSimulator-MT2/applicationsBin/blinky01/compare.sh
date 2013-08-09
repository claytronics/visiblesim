#! /bin/sh

#diff <(head -$3 $1) <(head -$3 $2)
#diff <(head -3000 exec2.txt) <(head -3000 exec1.txt)

vert="\\033[1;32m"
rouge="\\033[0;31m"
neutre="\\033[0;m"

./blinky01 > exec1.out
killall meld

systemTime=0
userTime=0
realTime=0

nbr=10
for i in $(seq 2 1 $nbr)
do
   	time="$( { time ./blinky01 > exec$i.out; } 2>&1 )"
   	killall meld
   	echo $time
   	userTime=$(echo "$userTime+$(echo $time | cut -d " " -f 1 | cut -d "u" -f 1)" | bc -l)
   	systemTime=$(echo "$systemTime+$(echo $time | cut -d " " -f 2 | cut -d "s" -f 1)" | bc -l)
   	realTime=$(echo "$realTime+$(echo $time | cut -d " " -f 3 | cut -d "e" -f 1 | cut -d ":" -f 2)" | bc -l)
	echo "cumulated user time: $userTime"
	echo "cumulated system time: $systemTime"
	echo "cumulated real time: $realTime"
	r=$(diff -q exec1.out exec$i.out | wc -w)
	if [ $r -eq 0 ]
	then
		echo "${vert}OK${neutre}"
	else
		echo "${rouge}KO${neutre}"
	fi
done
	echo "average user time: $( echo $userTime/$nbr | bc -l)"
	echo "average system time: $( echo $systemTime/$nbr | bc -l)"
	echo "average real time: $( echo $realTime/$nbr | bc -l)"
	

	
