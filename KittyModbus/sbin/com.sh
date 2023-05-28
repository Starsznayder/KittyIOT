#!/bin/bash
counter=10
while [ $counter -gt 0 ]
do
	socat pty,link=/kitty/dev/COM6 tcp:192.168.107.12:8329
	sleep 5
done
