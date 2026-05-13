#!/bin/bash
set -e

# Install glog (libgoogle-glog-dev pulls in the runtime library)
sudo apt install -y libgoogle-glog-dev

# Build and install evpp from source (skip clone if already exists)
EVPP_DIR="$HOME/evpp"
if [ ! -d "$EVPP_DIR" ]; then
    git clone https://github.com/Qihoo360/evpp "$EVPP_DIR"
fi

cd "$EVPP_DIR"
git submodule update --init --recursive
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j"$(nproc)"
sudo make install
sudo ldconfig
