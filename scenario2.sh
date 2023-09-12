#!/bin/bash

# Subscriber (security plugin enabled)
cd ./HelloWorld_dpde_secure_SCENARIO2/
gnome-terminal --title="Secure subscriber" -- bash -c "taskset -c 1 ./objs/x64Linux4gcc9.4.0/HelloWorld_subscriber; exec bash" 
cd  ..
sleep 2

# Subscriber (security plugin disabled)
cd ./HelloWorld_dpde_SCENARIO1/
gnome-terminal --title="Unauthorized subscriber" -- bash -c "taskset -c 2 ./objs/x64Linux4gcc9.4.0/HelloWorld_subscriber; exec bash"
cd  ..
sleep 2

# Publisher
cd ./HelloWorld_dpde_secure_SCENARIO2/
gnome-terminal --title="Publisher" -- bash -c "taskset -c 0 ./objs/x64Linux4gcc9.4.0/HelloWorld_publisher; exec bash"
cd ..

