#!/bin/bash

NIGHTVISION_FILE="/tmp/night_vision_enabled"

case $1 in
  on)
    echo "$(date) - nightmode on"
    curl -d value=0 http://127.0.0.1:8081/api/ir_cut 2>/dev/null
    curl -d value=1 http://127.0.0.1:8081/api/ir_led 2>/dev/null
    echo "1" > $NIGHTVISION_FILE
    ;;
  off)
    echo "$(date) - nightmode off"
    curl -d value=1 http://127.0.0.1:8081/api/ir_cut 2>/dev/null
    curl -d value=0 http://127.0.0.1:8081/api/ir_led 2>/dev/null
    echo "0" > $NIGHTVISION_FILE
    ;;
  *)
    ;;
esac

