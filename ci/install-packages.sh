#! /bin/bash

# For g++-9 #TODO remove once we update to bionic
sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y

# For clang-8
sudo echo "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-8 main" >> /etc/apt/sources.list
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|sudo apt-key add -

# We need a more recent meson too
sudo add-apt-repository ppa:jonathonf/meson -y

sudo apt-get update

sudo apt-get install meson build-essential git g++-9 libgtest-dev libgflags-dev pkg-config make clang-tidy-8 -y
