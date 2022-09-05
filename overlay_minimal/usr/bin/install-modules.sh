#!/bin/bash
set -x

if [ -d "/sdcard/modules" ]
then
	logger -s -t general_init "Installing modules from sdcard"

	if [ ! -d "/etc/openmiko.d/modules" ]; then
		mkdir -p /etc/openmiko.d/modules
	fi

	pushd /etc/openmiko.d/modules/

	for MODULE in /sdcard/modules/*.tar.xz; do
		if [ -f "$MODULE" ]; then
			MODULE_NAME=`basename $MODULE .tar.xz`
			logger -s -t general_init "Installing module $MODULE_NAME"

			mkdir -p /etc/openmiko.d/modules/$MODULE_NAME
			cp "$MODULE" /etc/openmiko.d/modules/$MODULE_NAME
			pushd "$MODULE_NAME"
			xz -d < "$MODULE" | tar x

			if [ -x "preinstall.sh" ]; then
				./preinstall.sh
				rm -f preinstall.sh
			fi

			if [ -d "overlay" ]; then
				find overlay -type f -exec sh -c 'SOURCE={};TARGET=${SOURCE#overlay};install -D $SOURCE $TARGET' \; -print
				rm -rf overlay
			fi

			if [ -x "postinstall.sh" ]; then
				./postinstall.sh
				rm -f postinstall.sh
			fi

			popd

			rm -f "/etc/openmiko.d/modules/$MODULE/$MODULE_NAME.tar.xz"
		fi
	done

	popd
fi

