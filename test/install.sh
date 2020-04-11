set -e -x
echo "deb http://ppa.launchpad.net/zhaofeng-shu33/gtest/ubuntu xenial main" > /etc/apt/sources.list.d/gtest.list
apt-key adv --keyserver keyserver.ubuntu.com --recv-keys B4552FED361B2FCCEAD7193C69FDF0C946D67581
apt update
apt install --yes g++ libgtest-dev cmake
