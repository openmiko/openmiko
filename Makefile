REVISION_HASH := $(shell git rev-parse --quiet --short HEAD)
BUILDROOT := "/openmiko/build/buildroot-2016.02"

build:
	# Command designed to be run outside the container
	docker build -t openmiko/openmiko:latest .
