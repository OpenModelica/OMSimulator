FROM centos:7

RUN yum -y install libtool automake gcc-c++ libstdc++-static boost-devel make cmake readline-devel lua-devel

# Install newer git version
RUN yum -y install https://packages.endpoint.com/rhel/7/os/x86_64/endpoint-repo-1.7-1.x86_64.rpm \
    && yum -y install git
