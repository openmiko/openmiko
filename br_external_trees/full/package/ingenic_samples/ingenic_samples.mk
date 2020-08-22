################################################################################
#
# ingenic_samples
#
################################################################################

INGENIC_SAMPLES_VERSION = v0.1.0
INGENIC_SAMPLES_SOURCE = ingenic-samples.tar.gz
INGENIC_SAMPLES_SITE = https://github.com/mpromonet/INGENIC_SAMPLES_/archive
INGENIC_SAMPLES_DEPENDENCIES =
INGENIC_SAMPLES_CONF_OPTS =

$(eval $(generic-package))
