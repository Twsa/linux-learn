while : 
	do 
		sleep 10 
		running=$(ps -ef |grep -w "gree" |grep -v "grep") 
		echo $running
		if [ "$running" ] ; then 
			echo "gree is running..." 
		else 
			echo "gree was not started" 
			gree &
		fi 
done 
