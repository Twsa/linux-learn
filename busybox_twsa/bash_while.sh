while true
do
   	echo `date` >> log.txt
	echo `cat /proc/326/status | grep -E "VmSize|VmRSS"` >>log.txt
	date
	cat /proc/326/status | grep -E "VmSize|VmRSS"
	sleep 10
done
