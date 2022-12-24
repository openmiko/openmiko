#!/bin/sh

NIGHTVISION_FILE="/tmp/night_vision_enabled"

. /usr/bin/libgpio.sh
gpio_select_gpiochip 0

case $1 in
  on)
    echo "$(date) - nightmode on"
    #curl -d value=0 http://127.0.0.1:8081/api/ir_cut 2>/dev/null
    #curl -d value=1 http://127.0.0.1:8081/api/ir_led 2>/dev/null
    gpio_set_value 49 1
    gpio_set_value 25 1
    echo "1" > $NIGHTVISION_FILE
    sleep 1
    gpio_set_value 25 0
    ;;. /usr/bin/libgpio.sh
  off)
    echo "$(date) - nightmode off"
    #curl -d value=1 http://127.0.0.1:8081/api/ir_cut 2>/dev/null
    #curl -d value=0 http://127.0.0.1:8081/api/ir_led 2>/dev/null
    gpio_set_value 49 0
    gpio_set_value 26 1
    echo "0" > $NIGHTVISION_FILE
    sleep 1
    gpio_set_value 25 0
    ;;
  *)
    ;;
esac

