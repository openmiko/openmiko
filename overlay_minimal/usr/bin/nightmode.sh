#!/bin/bash

NIGHTVISION_FILE="/tmp/night_vision_enabled"

. /scripts/common_functions.sh

case $1 in
  on)
    echo "$(date) - nightmode on"
    ir_cut off
    ir_led off
    echo "1" > $NIGHTVISION_FILE
    ;;
  off)
    echo "$(date) - nightmode off"
    ir_led on
    ir_cut on
    echo "0" > $NIGHTVISION_FILE
    ;;
  *)
    ;;
esac

