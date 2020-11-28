MJPG_STREAMER_VERSION = 2a24fc9d952bf79ce17957b2aaf0fb66413f539e
MJPG_STREAMER_SITE = https://github.com/openmiko/mjpg-streamer.git
MJPG_STREAMER_SITE_METHOD = git
MJPG_STREAMER_DEPENDENCIES =
# MJPG_STREAMER_CONF_OPTS = -DCMAKE_BUILD_TYPE=Debug
MJPG_STREAMER_SUBDIR = mjpg-streamer-experimental

$(eval $(cmake-package))
