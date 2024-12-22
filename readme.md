# NTP client

This is a C++ class that implements a NTP client. It has been tested with a chrony server.
I have also included a sample of my chrony.conf file where I have setup a local server and client for my experiments.

### build
First create an empty directory inside your working folder:
```
mkdir build && cd build
```
and then 
```
cmake ..
```
and
```
cmake --build .
```

Optional, if you want to install: 

```
make install DESTDIR={YOUR_INSTALLATION_PATH}
```
By default, the installation path is `/usr/local/`

### licence
Part of the code has been based to C NTPClient by David Lettier

### useful links for understanding the client internals
- [UDP Server-Client implementation in C](https://www.geeksforgeeks.org/udp-server-client-implementation-c/)
- [Let's make a NTP Client in C](https://lettier.github.io/posts/2016-04-26-lets-make-a-ntp-client-in-c.html)
- [Technical information - NTP Data Packet](https://www.meinbergglobal.com/english/info/ntp-packet.htm)

### useful links for setting up a chrony server in ubuntu

- [Ubuntu Bionic: Using chrony to configure NTP](https://ubuntu.com/blog/ubuntu-bionic-using-chrony-to-configure-ntp) (which works for Xenial as well)
- [rosbee - chrony.wiki](https://code.google.com/archive/p/rosbee/wikis/chrony.wiki)

### License
[MIT](./license.md)

### Contributors
[Angelos Plastropoulos](https://github.com/plusangel) \
[Jairo Reyes](https://github.com/jairoareyes)


## installing glog (https://learn.microsoft.com/en-us/vcpkg/get_started/get-started-vscode?pivots=shell-bash)

# install vcpkg and glog
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh

# from root of project you are working on 
vcpkg new --application
vcpkg add port glog

# add in ~/.bashrc
export VCPKG_ROOT=/home/bytes/repos/vcpkg/
export PATH=$PATH:$VCPKG_ROOT

