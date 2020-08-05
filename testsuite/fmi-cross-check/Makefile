# Makefile to run the FMI Cross-Check

# On Windows change to win64 or win32
# On Mac change to darwin64
OS_SHORT = linux64

# Set to your fmi-cross-check as an abolute path.
# If the directory does not exist it will be cloned into that location.
FMI_CROSS_CHECK_DIR = fmi-cross-check

# If you don't have FMPy installed skip step "validate-files" or use a Docker image with FMPy:
# For Ubuntu Bionic use "anheuermann/fmpy:bionic-amd64" or for Ubuntu Focal use "anheuermann/fmpy:focal-amd64"
# For other OS you have to create your own Docker image.
# FMPY_DOCKER =
FMPY_DOCKER = docker run -it --rm -v ${FMI_CROSS_CHECK_DIR}:/fmi-cross-check -w /fmi-cross-check anheuermann/fmpy:bionic-amd64

OMS_VERSION=$$(OMSimulator --version | sed -e 's/[\~]/-/g' | grep -oE '[^ ]+$$')
BRANCH_NAME=add-OMSimulator-$(OMS_VERSION)

all:
	make import-FMUs
	make validate-files
	make show-result

validate-files:
	$(info Validating results with FMPy)
	$(info This takes a while, get yourself a cup of tea.)
	${FMPY_DOCKER} python3 -m fmpy.cross_check.validate_vendor_repo --clean-up

import-FMUs: fmi-cross-check-update
	python3 addResults.py ${FMI_CROSS_CHECK_DIR}

fmi-cross-check-update:
	if [ -d "fmi-cross-check" ]; then \
	  cd ${FMI_CROSS_CHECK_DIR} && git checkout master && git pull ; \
	else \
	  git clone https://github.com/modelica/fmi-cross-check.git; \
	fi

show-result:
	echo "Passed tests"
	cd ${FMI_CROSS_CHECK_DIR} && find results/2.0/cs/${OS_SHORT}/OMSimulator -name passed && find results/2.0/me/${OS_SHORT}/OMSimulator -name passed

	echo "Rejected tests"
	cd ${FMI_CROSS_CHECK_DIR} && find results/2.0/cs/${OS_SHORT}/OMSimulator -name rejected && find results/2.0/me/${OS_SHORT}/OMSimulator -name rejected

	echo "Failed tests"
	cd ${FMI_CROSS_CHECK_DIR} && find results/2.0/cs/${OS_SHORT}/OMSimulator -name failed && find results/2.0/me/${OS_SHORT}/OMSimulator -name failed

clean:
	cd ${FMI_CROSS_CHECK_DIR} && git clean -dfx
