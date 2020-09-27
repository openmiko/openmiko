#!/bin/bash
set -e

echo "Executing pre filesystem image creation script"

# The environment variables BR2_CONFIG, HOST_DIR, STAGING_DIR,
# TARGET_DIR, BUILD_DIR, BINARIES_DIR and BASE_DIR are defined


DEFAULT_IMAGE_DIR="/openmiko/build/buildroot-2016.02/output/images"
BASE_DIR=${BASE_DIR:-/openmiko/build/buildroot-2016.02/output}
IMAGES="${BASE_DIR}/images"
HOST_DIR=${HOST_DIR:-/openmiko/build/buildroot-2016.02/output/host}
TARGET_DIR=${TARGET_DIR:-/openmiko/build/buildroot-2016.02/output/target}