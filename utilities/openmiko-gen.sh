#!/usr/bin/env bash

echo "Openmiko Config Generator"

OUTPUT_DIRECTORY="${1}"

if [ -z "${OUTPUT_DIRECTORY}" ]; then
  echo "Error: Must supply output directory"
  echo "Usage: ${0} output_directory"
  exit
fi

if [ ! -d "${OUTPUT_DIRECTORY}" ]; then
  echo "Output directory '${OUTPUT_DIRECTORY}' not found. Creating."
  mkdir -p "${OUTPUT_DIRECTORY}"
fi

ENABLE_MJPEG_OVER_HTTP=
WIFI_MODULE=
ENABLE_LEDS=

DETAILS_CORRECT=n

while [[ ${DETAILS_CORRECT} != [yY] ]]; do

  echo ""

  echo "Select Camera Type:"
  echo "0) Wyze V2"
  echo "1) Wyze V2 Pan or Dafang"

  while [[ ${CAM_TYPE} != [01] ]]; do
    read -n1 -rp "Enter 0 or 1: " CAM_TYPE
    echo ""
  done

  if [ "${CAM_TYPE}" == "0" ]; then
    WIFI_MODULE=8189fs
  else
    WIFI_MODULE=8189es
  fi

  echo "Enable MJPEG over HTTP. Not needed if using RTSP:"
  echo "0) Disable"
  echo "1) Enable"

  while [[ ${ENABLE_MJPEG_OVER_HTTP} != [01] ]]; do
    read -n1 -rp "Enter 0 or 1: " ENABLE_MJPEG_OVER_HTTP
    echo ""
  done

  echo "Enable LEDS"
  echo "0) Disable"
  echo "1) Enable"

  while [[ ${ENABLE_LEDS} != [01] ]]; do
    read -n1 -rp "Enter 0 or 1: " ENABLE_LEDS
    echo ""
  done

  echo -e "\nCamera Type: "
  [ "${CAM_TYPE}" == "0" ] && echo -e "Wyze V2" || echo -e "Wyze V2 Pan or Dafang"
  echo "Enable MJPEG over HTTP: ${ENABLE_MJPEG_OVER_HTTP}"
  echo "Enable LEDS: ${ENABLE_LEDS}"

  read -n1 -rp "Is this correct? (y/n): " DETAILS_CORRECT
done

echo ""

cat << EOF > "${OUTPUT_DIRECTORY}/openmiko.conf"
ENABLE_DEBUG=0
# Starts ffmpeg on startup recording from the v4l2rtspserver
ENABLE_RECORDING=0
# Start the micropython based api
ENABLE_API=1
# Use a swapfile on the sdcard
ENABLE_SWAP=1
# Use this setting if swapfile is a dedicated partition
SWAPFILE=/dev/mmcblk0p1
# Set partition to use for mounting at /sdcard
SD_PARTITION=/dev/mmcblk0p2
SD_FILESYSTEM=vfat
# Enable MJPEG over HTTP
ENABLE_MJPEG_OVER_HTTP=${ENABLE_MJPEG_OVER_HTTP}
MJPEG_FPS=15
# Script that detects nighttime/daytime and turns on IR LEDs
ENABLE_AUTONIGHT=1
AUTONIGHT_PARAMS="-j 3 -w 3 -1 1200000 -2 930000,14,10 -3 3000,17,8"
# Set alternate videocapture settings file
# VIDEO_CAPTURE_SETTINGS=/etc/videocapture_settings_1_encoder.json
# Set enable audio (is currently buggy, use at your own risk!)
ENABLE_AUDIO=0
# Make sure these settings match the /etc/videocapture_settings.json file
VIDEO_DEV_1=/dev/video3
VIDEO_DEV_2=/dev/video4
VIDEO_DEV_3=/dev/video5
# Use 8189fs for WyzeCam V2. For the WyzeCam Pan and Dafang use 8189es
WIFI_MODULE=${WIFI_MODULE}
# Disables LEDs on the camera
DISABLE_LEDS=$((ENABLE_LEDS ^ 1))
EOF

echo "${OUTPUT_DIRECTORY}/openmiko.conf generated"
