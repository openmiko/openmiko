#!/bin/bash

HOSTNAME=$1

while inotifywait -r *; do
    rsync -avzz . root@"$HOSTNAME":/app
done
