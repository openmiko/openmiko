MJPG_STREAMER_VERSION = 2ee9fb893d12fddb54636eac807b3e96ef2bbdbe
MJPG_STREAMER_SITE = https://github.com/openmiko/mjpg-streamer.git
MJPG_STREAMER_SITE_METHOD = git
MJPG_STREAMER_DEPENDENCIES =
# MJPG_STREAMER_CONF_OPTS = -DCMAKE_BUILD_TYPE=Debug
MJPG_STREAMER_SUBDIR = mjpg-streamer-experimental

$(eval $(cmake-package))
