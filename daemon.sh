#!/bin/bash

if [  $1 == 'help' ] || [ $1 == '-h' ];then
	sudo systemctl start skype.service
	echo "./daemon start           | starts skype server."
	echo "./daemon stop            | stops skype server."
	echo "./daemon status          | get skype daemon status."
	echo "./daemon config          | prompt with daemon config helper."
	echo "./daemon log             | prints skype server logs."
fi

if [  $1 == 'start' ];then
	sudo systemctl start skype.service
fi

if [  $1 == 'stop' ];then
	sudo systemctl stop skype.service
fi

if [  $1 == 'status' ];then
	sudo systemctl status skype.service
fi

if [  $1 == 'log' ];then
	sudo journalctl -u skype.service --since "1 hour ago"
fi

if [  $1 == 'watch' ];then
	sudo journalctl -u skype.service -f --since "1 hour ago"
fi

if [  $1 == 'config' ];then

	echo "-- COPY BELOW"
	echo ""
	printf "	[Unit]
	Description=Skype daemon
	After=network.target
	StartLimitIntervalSec=0

	[Service]
	Type=simple
	Restart=always
	RestartSec=1
	User=pb
	ExecStart=/home/pb/Skype/build/bin/server

	[Install]
	WantedBy=multi-user.target "

	echo " "
	echo " "
	echo "-- COPY ABOVE"

	echo "Copy the above to paste. We will open VIM next so you can paste in the correct config file."
	echo "Press any key to continue...."
	read

	sudo vim /etc/systemd/system/skype.service

	echo "it's done! Run ./daemon start."

fi
