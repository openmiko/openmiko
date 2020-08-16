

REVISION_HASH := $(shell git rev-parse --quiet --short HEAD)

release:	
	$(info Creating new firmware release: openmiko_${REVISION_HASH}.bin)

build:
	docker build -t openmiko/openmiko:latest .

syncrootfs:
	rsync -vr /src/overlayfs_dafang/etc/ /sharedfiles/rootfs/etc/
