#!/bin/bash

# Subscriber 
cd ./HelloWorld_dpde_secure_SCENARIO1/
gnome-terminal --title="Secure subscriber" -- bash -c "taskset -c 1 ./objs/x64Linux4gcc9.4.0/HelloWorld_subscriber; exec bash"
cd  ..
sleep 2

# Publisher
cd ./HelloWorld_dpde_secure_SCENARIO1/
gnome-terminal --title="Publisher" -- bash -c "taskset -c 0 ./objs/x64Linux4gcc9.4.0/HelloWorld_publisher; exec bash"
cd ..

