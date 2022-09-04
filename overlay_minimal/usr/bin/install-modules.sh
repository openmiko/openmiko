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
			cp "$MODULE" /etc/openmiko.d/modules/
			xz -d < "$MODULE" | tar x
			pushd "$MODULE_NAME"

			if [ -x "pre-install.sh" ]; then
				./pre-install.sh
				rm -f pre-install.sh
			fi

			if [ -d "overlay" ]; then
				find overlay -type f -exec sh -c 'SOURCE={};TARGET=${SOURCE#overlay};install -D $SOURCE $TARGET' \; -print
				rm -rf overlay
			fi

			if [ -x "post-install.sh" ]; then
				./post-install.sh
				rm -f post-install.sh
			fi

			popd

			rm -f "/etc/openmiko.d/modules/$MODULE_NAME.tar.xz"
		fi
	done

	popd
fi

