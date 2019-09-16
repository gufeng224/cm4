#!/system/bin/sh
chmod 777 /dev/snd/*
tinymix -D 2 0 1
tinymix -D 2 5 1
tinymix -D 2 1 1
tinymix -D 2 6 2
tinymix -D 2 7 0
tinymix -D 2 8 2
tinymix -D 2 9 2
tinymix -D 2 12 4
tinymix -D 2 10 1
tinymix -D 2 13 2
tinymix -D 2 17 4

chmod 777 /var
chmod 777 /tmp
#export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/data/base/lib/:/data/base/lib/mslib:/tvos/libGlibc:/lib:/mslib:/mslib/utopia:/tvservice/conflictlib:/config
  
export F1_CONFIG=/tvos/bin/run_config 

#add coredump folder
#ulimit -c unlimited
#echo "1" > /proc/sys/kernel/core_uses_pid
#echo "/dev/null" > /proc/sys/kernel/core_pattern
chmod 777 log_out
/tvos/bin/log_out &		

# create EPG userdata in RAM
#mkdir /userdata/epg 0777 root system
#mount -t tmpfs -o size=32m,mode=0777 tmpfs /userdata/epg
#touch /userdata/epg/EpgData.db
#chmod 0666 /userdata/epg/EpgData.db

echo "run tcl MonitorServer start"
/system/bin/logwrapper /tvos/bin/sitatvservice &

/system/bin/logwrapper /system/bin/MonitorServer &
echo "run tcl mw process end"
