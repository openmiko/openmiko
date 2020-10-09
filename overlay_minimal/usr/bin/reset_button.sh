#!/bin/bash

. /usr/bin/libgpio.sh

# This is CHIP B so it is 46-32
gpio_select_gpiochip 32

SETUP_GPIO=14

gpio_export $SETUP_GPIO
gpio_direction_input $SETUP_GPIO

SECONDS_HELD_DOWN=0


function reset_system()
{
	echo "Reset system called"
	echo "none" > /sys/class/leds/led_blue/trigger
	echo "none" > /sys/class/leds/led_yellow/trigger
	sleep 1

	rm -rf /config/overlay
	mkdir -p /config/overlay

	# 3 heartbeats to indicate successful reset
	echo "heartbeat" > /sys/class/leds/led_blue/trigger
	sleep 3

	echo "none" > /sys/class/leds/led_blue/trigger
	echo "timer" > /sys/class/leds/led_yellow/trigger
}


while true
do
	sleep 1
	GPIO_VAL=$(gpio_get_value $SETUP_GPIO)
	#echo $GPIO_VAL

	if [ $GPIO_VAL -eq 0 ];
	then
		SECONDS_HELD=$(($SECONDS_HELD+1))
	else
		SECONDS_HELD=0
	fi

	echo "Setup button has been held down for $SECONDS_HELD seconds"

	if [ $SECONDS_HELD -gt 5 ];
	then
		reset_system
		SECONDS_HELD=0
	fi
done



