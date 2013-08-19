#! /bin/sh

#diff <(head -$3 $1) <(head -$3 $2)
#diff <(head -3000 exec2.txt) <(head -3000 exec1.txt)

green="\\033[1;32m"
red="\\033[0;31m"
classic="\\033[0;m"

#small : 5 blocks
#medium : 15 blocks
#large : 30 blocks
#huge : 60 blocks

totalSuccesses=0
totalFailures=0
total=0

for program in rainbow #ends
do
	for config in small medium large huge
	do
		echo '
		<?xml version="1.0" standalone="no" ?>
		<vm serverport="5000" vmPath="/home/ubuntu/Bureau/meld/meld" programPath="/home/ubuntu/Bureau/meld/examples/'$program.m'" debugging="False" />

		<world gridsize="10,10,10">
			<camera target="200,200,200" directionSpherical="0,70,400" angle="45"/>
			<spotlight target="200,20,200" directionSpherical="45,60,500" angle="40"/>

			<blockList color="0,255,0" size="1,1,1" blocksize="40,40,41" > ' > config.xml
			# cotes, avant/arriere, haut
		case $config in
			small) echo '<block position="5,5,5"/>
		<block position="5,5,6"/>
		<block position="5,5,7"/>
		<block position="4,5,6"/>
		<block position="6,5,6"/>' >> config.xml ;;
			medium) echo '<block position="3,3,2"/> 
		<block position="3,3,3"/>
		<block position="3,3,4"/>
		<block position="3,3,5"/>		
		<block position="3,3,6"/>		
		<block position="4,3,4"/>
		<block position="4,3,5"/>
		<block position="4,3,6"/>		
		<block position="5,3,6"/>
		<block position="5,3,5"/>
		<block position="5,3,7"/>
		<block position="5,3,8"/>
		<block position="6,3,5"/>
		<block position="6,3,6"/>
		<block position="6,3,7"/>
		<block position="6,3,8"/>' >> config.xml ;;
			large) echo '<block position="3,3,2"/> 
		<block position="3,3,3"/>
		<block position="3,3,4"/>
		<block position="3,3,5"/>		
		<block position="3,3,6"/>		
		<block position="4,3,4"/>
		<block position="4,3,5"/>
		<block position="4,3,6"/>		
		<block position="4,4,6"/>
		<block position="4,4,5"/>
		<block position="4,4,7"/>
		<block position="4,4,8"/>
		<block position="4,5,5"/>
		<block position="4,5,6"/>
		<block position="4,5,7"/>
		<block position="4,5,8"/>
		<block position="3,4,2"/> 
		<block position="3,4,3"/>
		<block position="3,4,4"/>
		<block position="3,4,5"/>		
		<block position="3,4,6"/>		
		<block position="3,5,4"/>
		<block position="3,5,5"/>
		<block position="3,5,6"/>		
		<block position="3,6,6"/>
		<block position="3,6,5"/>
		<block position="3,6,7"/>
		<block position="3,6,8"/>
		<block position="3,7,5"/>
		<block position="3,7,6"/>
		<block position="3,7,7"/>
		<block position="3,7,8"/>' >> config.xml ;;
		huge) echo '<block position="3,3,2"/> 
		<block position="3,3,3"/>
		<block position="3,3,4"/>
		<block position="3,3,5"/>		
		<block position="3,3,6"/>		
		<block position="4,3,4"/>
		<block position="4,3,5"/>
		<block position="4,3,6"/>		
		<block position="4,4,6"/>
		<block position="4,4,5"/>
		<block position="4,4,7"/>
		<block position="4,4,8"/>
		<block position="4,5,5"/>
		<block position="4,5,6"/>
		<block position="4,5,7"/>
		<block position="4,5,8"/>
		<block position="3,4,2"/> 
		<block position="3,4,3"/>
		<block position="3,4,4"/>
		<block position="3,4,5"/>		
		<block position="3,4,6"/>		
		<block position="3,5,4"/>
		<block position="3,5,5"/>
		<block position="3,5,6"/>		
		<block position="3,6,6"/>
		<block position="3,6,5"/>
		<block position="3,6,7"/>
		<block position="3,6,8"/>
		<block position="3,7,5"/>
		<block position="3,7,6"/>
		<block position="3,7,7"/>
		<block position="3,7,8"/>
		
		<block position="5,3,2"/> 
		<block position="5,3,3"/>
		<block position="5,3,4"/>
		<block position="5,3,5"/>		
		<block position="5,3,6"/>		
		<block position="6,3,4"/>
		<block position="6,3,5"/>
		<block position="6,3,6"/>		
		<block position="6,4,6"/>
		<block position="6,4,5"/>
		<block position="6,4,7"/>
		<block position="6,4,8"/>
		<block position="6,5,5"/>
		<block position="6,5,6"/>
		<block position="6,5,7"/>
		<block position="6,5,8"/>
		<block position="5,4,2"/> 
		<block position="5,4,3"/>
		<block position="5,4,4"/>
		<block position="5,4,5"/>		
		<block position="5,4,6"/>		
		<block position="5,5,4"/>
		<block position="5,5,5"/>
		<block position="5,5,6"/>		
		<block position="5,6,6"/>
		<block position="5,6,5"/>
		<block position="5,6,7"/>
		<block position="5,6,8"/>
		<block position="5,7,5"/>
		<block position="5,7,6"/>
		<block position="5,7,7"/>
		<block position="5,7,8"/>' >> config.xml ;;
			*) ;;
		esac
		echo '
			</blockList>
			</world>' >> config.xml
		mkdir -p $config
		./blinky01 > $config/exec1.out
		killall meld

		systemTime=0
		userTime=0
		realTime=0

		nbr=30
		total=$(($total+$nbr))
		failures=0
		successes=0
		for i in $(seq 1 1 $nbr)
		do
			j=$(($i+1))
			time="$( { time ./blinky01 > $config/exec$j.out; } 2>&1 )"
			killall meld
			echo $time
			userTime=$(echo "$userTime+$(echo $time | cut -d " " -f 1 | cut -d "u" -f 1)" | bc -l)
			systemTime=$(echo "$systemTime+$(echo $time | cut -d " " -f 2 | cut -d "s" -f 1)" | bc -l)
			realTime=$(echo "$realTime+$(echo $time | cut -d " " -f 3 | cut -d "e" -f 1 | cut -d ":" -f 2)" | bc -l)
			echo "cumulated user time: $userTime"
			echo "cumulated system time: $systemTime"
			echo "cumulated real time: $realTime"
			r=$(diff -q $config/exec1.out $config/exec$j.out | wc -w)
			if [ $r -eq 0 ]
			then
				echo "${green}OK${classic}"
				successes=$(($successes+1))
			else
				echo "${red}KO${classic}"
				failures=$(($failures+1))
			fi
		done
		echo "average user time: $( echo $userTime/$nbr | bc -l)"
		echo "average system time: $( echo $systemTime/$nbr | bc -l)"
		echo "average real time: $( echo $realTime/$nbr | bc -l)"
		
		echo ""
		echo "Test : $program, $config" 
		echo "${green}successes: $successes/$nbr${classic}"
		echo "${red}failures: $failures/$nbr${classic}"
		if [ $successes -eq $nbr ] && [ $failures -eq 0 ]
		then
			echo "${green}TEST PASSED${classic}"
		else
			echo "${red}TEST FAILED${classic}"
		fi
		totalSuccesses=$(($totalSuccesses+$successes))	
		totalFailures=$(($totalFailures+$failures))
	done
done
echo ""
echo ""
echo "GLOBAL TEST : "
echo "${green}successes: $totalSuccesses/$total${classic}"
echo "${red}failures: $totalFailures/$total${classic}"
if [ $totalSuccesses -eq $total ] && [ $totalFailures -eq 0 ]
	then
		echo "${green}ALL TESTS PASSED${classic}"
	else
		echo "${red}AT LEAST ONE TEST FAILED${classic}"
	fi

