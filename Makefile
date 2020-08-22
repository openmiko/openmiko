

REVISION_HASH := $(shell git rev-parse --quiet --short HEAD)
BUILDROOT := "/openmiko/build/buildroot-2016.02"

release:
	cd ${BUILDROOT}	
	$(info Creating new firmware release: openmiko_${REVISION_HASH}.bin)
	make

build:
	# Command designed to be run outside the container
	docker build -t openmiko/openmiko:latest .

syncrootfs:
	rsync -vr /src/overlayfs_dafang/etc/ /sharedfiles/rootfs/etc/
