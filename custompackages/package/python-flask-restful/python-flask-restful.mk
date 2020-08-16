################################################################################
#
# python-flask-restful
#
################################################################################

PYTHON_FLASK_RESTFUL_VERSION = 0.3.8
PYTHON_FLASK_RESTFUL_SOURCE = Flask-RESTful-$(PYTHON_FLASK_RESTFUL_VERSION).tar.gz
PYTHON_FLASK_RESTFUL_SITE = https://files.pythonhosted.org/packages/67/65/84f3218666fc115497a13ff727f16d02374d07d1924cd4fd72e275294e8b
PYTHON_FLASK_RESTFUL_SETUP_TYPE = setuptools
PYTHON_FLASK_RESTFUL_LICENSE = BSD-3-Clause
PYTHON_FLASK_RESTFUL_LICENSE_FILES = LICENSE

$(eval $(python-package))
