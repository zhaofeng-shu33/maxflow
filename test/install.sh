set -e -x
echo "deb http://dl.bintray.com/zhaofeng-shu33/libgtest/ bionic main" > /etc/apt/sources.list.d/gtest.list
wget https://bintray.com/user/downloadSubjectPublicKey?username=bintray -O bintray-public.key.asc
apt-key add bintray-public.key.asc
apt update
apt install --yes g++ libomp-dev libgtest1.7 cmake
