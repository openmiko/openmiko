#!/bin/bash

case $1 in
  on)
    echo "nightmode on"
    curl -d value=0 http://127.0.0.1:8081/api/ir_cut
    curl -d value=1 http://127.0.0.1:8081/api/ir_led
    ;;
  off)
    echo "nightmode off"
    curl -d value=1 http://127.0.0.1:8081/api/ir_cut
    curl -d value=0 http://127.0.0.1:8081/api/ir_led
    ;;
  *)
    ;;
esac

