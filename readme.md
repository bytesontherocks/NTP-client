# NTP client

Forked from https://github.com/plusangel/NTP-client


## installing glog (https://learn.microsoft.com/en-us/vcpkg/get_started/get-started-vscode?pivots=shell-bash)

# install vcpkg and glog
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh

# from root of project you are working on 
vcpkg new --application
vcpkg add port glog
vcpkg add port gtest
vcpkg install

# add in ~/.bashrc
export VCPKG_ROOT=/home/bytes/repos/vcpkg
export PATH=$PATH:$VCPKG_ROOT

