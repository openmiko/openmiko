################################################################################
#
# r4l2rtspserver
#
################################################################################

V4L2RTSPSERVER_VERSION = v0.1.3
V4L2RTSPSERVER_SOURCE = $(V4L2RTSPSERVER_VERSION).tar.gz
V4L2RTSPSERVER_SITE = https://github.com/mpromonet/v4l2rtspserver/archive
V4L2RTSPSERVER_DEPENDENCIES =
V4L2RTSPSERVER_CONF_OPTS = -DCMAKE_INSTALL_PREFIX=/apps

$(eval $(cmake-package))
