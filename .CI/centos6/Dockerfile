FROM centos:6

RUN yum -y install epel-release centos-release-scl yum-utils && \
  yum-config-manager --enable rhel-server-rhscl-7-rpms && \
  yum -y install devtoolset-7-gcc devtoolset-7-gcc-c++ && \
  yum -y install libtool automake gcc-c++ boost-devel git cmake readline-devel lua-devel
