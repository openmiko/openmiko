#!/bin/bash

NIGHTVISION_FILE="/tmp/night_vision_enabled"

if [[ -f /etc/openmiko.conf ]]; then
	. /etc/openmiko.conf
fi

. /usr/bin/libgpio.sh

gpio_select_gpiochip 0

gpio_direction_output 25
gpio_direction_output 26
gpio_direction_output 49

ir_led() {
	case "$1" in
	on)
		gpio_set_value 49 1
	;;
	off)
		gpio_set_value 49 0
	;;
	esac
}

ir_cut() {
	case "$1" in
	on)
		gpio_set_value 25 0
		gpio_set_value 26 1
		sleep 1
		gpio_set_value 26 0
	;;
	off)
		gpio_set_value 26 0
		gpio_set_value 25 1
		sleep 1
		gpio_set_value 25 0
	;;
	esac
}

case $1 in
	on)
		echo "$(date) - nightmode on"
		if [ "$DISABLE_LEDS" == "0" ]; then
			ir_led on
			ir_cut off
		fi
		echo "1" > $NIGHTVISION_FILE
		;;
	off)
		echo "$(date) - nightmode off"
		ir_led off
		ir_cut on
		echo "0" > $NIGHTVISION_FILE
		;;
	*)
		;;
esac
