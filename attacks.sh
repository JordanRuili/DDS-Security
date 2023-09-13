#!/bin/bash

# Publisher
cd ./HelloWorld_dpde_secure_SCENARIO2/
gnome-terminal --title="Publisher" -- bash -c "taskset -c 0 ./objs/x64Linux4gcc9.4.0/HelloWorld_publisher; exec bash"
cd ..

publisher_terminal_pid=$(xdotool search --onlyvisible --name "Publisher")
echo "Publisher PID : $publisher_terminal_pid"


sleep 2


iterations=10

for ((i=1; i<=iterations; i++))
do
    # Random binary number
    random_number=$((RANDOM % 2))

    # Secure or authorized subscriber depending on binary
    if [ "$random_number" -eq 0 ]; then
        subscriber_type="Secure subscriber"
        scenario_folder="./HelloWorld_dpde_secure_SCENARIO2/"
    else
        subscriber_type="Unauthorized subscriber"
        scenario_folder="./HelloWorld_dpde_SCENARIO1/"
    fi

    # Show the iteration and the security of subscriber
    echo "Iteration $i: Running $subscriber_type"

    # Corresponding repository
    cd "$scenario_folder"

    # Launching the terminal with the subscriber
    gnome-terminal --tab --title="$subscriber_type" -- bash -c "taskset -c 1,2,3 ./objs/x64Linux4gcc9.4.0/HelloWorld_subscriber; exec bash" --tab-with-profile="publisherProfile"

    # Going back
    cd ..

    # Waiting time
    sleep 1
done
