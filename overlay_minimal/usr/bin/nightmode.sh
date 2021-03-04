#!/bin/sh

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
		echo "nightmode on"
		#curl -d value=1 http://127.0.0.1:8081/api/ir_led
		#curl -d value=0 http://127.0.0.1:8081/api/ir_cut

		/usr/bin/videoconfig -s -k night_mode -v 1
		ir_led on
		ir_cut off
	;;
	off)
		echo "nightmode off"
		#curl -d value=0 http://127.0.0.1:8081/api/ir_led
		#curl -d value=1 http://127.0.0.1:8081/api/ir_cut

		/usr/bin/videoconfig -s -k night_mode -v 0
		ir_led off
		ir_cut on
	;;
	*)
	;;
esac
