#!/usr/bin/env bash

echo "Openmiko WPA Supplicant Generator"

OUTPUT_DIRECTORY="${1}"

if [ -z "${OUTPUT_DIRECTORY}" ]; then
  echo "Error: Must supply output directory"
  echo "Usage: ${0} output_directory"
  exit 1
fi

if [ ! -d "${OUTPUT_DIRECTORY}" ]; then
  echo "Output directory '${OUTPUT_DIRECTORY}' not found. Creating."
  mkdir -p "${OUTPUT_DIRECTORY}"
fi

DETAILS_CORRECT=n

while [[ ${DETAILS_CORRECT} != [yY] ]]; do

  WPA_SSID=""
  WPA_PASSWORD=""
  IS_HIDDEN_SSID=""

  echo ""

  while [[ -z "${WPA_SSID}" ]]; do
    read -rp "Enter the SSID:" WPA_SSID
  done

  while [[ -z "${WPA_PASSWORD}" ]]; do
    read -rp "Enter the password:" WPA_PASSWORD
  done

  while [[ ${IS_HIDDEN_SSID} != [yYnN] ]]; do
    read -n1 -rp "Is this a hidden SSID? (y/n): " IS_HIDDEN_SSID
    echo ""
  done

  echo -e "\nSSID: ${WPA_SSID}"
  echo "Password: ${WPA_PASSWORD}"
  echo "Hidden SSID: ${IS_HIDDEN_SSID}"

  read -n1 -rp "Is this correct? (y/n): " DETAILS_CORRECT
done

echo ""

SCAN_SSID="0"

if [ "${IS_HIDDEN_SSID}" == "y" ]; then
  SCAN_SSID="1"
fi

cat << EOF > "${OUTPUT_DIRECTORY}/wpa_supplicant.conf"
ctrl_interface=/var/run/wpa_supplicant
ctrl_interface_group=0
ap_scan=1
bgscan="simple:30:-45:300"

network={
        ssid="${WPA_SSID}"
        scan_ssid=${SCAN_SSID}
        key_mgmt=WPA-PSK
        pairwise=CCMP TKIP
        group=CCMP TKIP WEP104 WEP40
        psk="${WPA_PASSWORD}"
        priority=2
}
EOF

echo "${OUTPUT_DIRECTORY}/wpa_supplicant.conf generated"
