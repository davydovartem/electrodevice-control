# Multimeter Control

A project for remote control of a multichannel multimeter via UNIX sockets.

## Requirements
- **OS**: Ubuntu 22.04 LTS (recommended)
- **Compiler**: GCC 11+ (C++17 support)
- **Build System**: CMake 3.16+
- **Qt**: 5.12+ (for GUI client only)
- **Boost**: 1.74+ (for server only)

## Installation
```bash
# Clone repository
git clone https://github.com/davydovartem/multimeter-control.git
cd multimeter-control

# Install dependencies
sudo apt-get install build-essential cmake qtbase5-dev libboost-system-dev

# To build you can run
./build.sh

# Or do the following
mkdir build
cd build
cmake ..
cmake --build ./
```
When building is done. Server executable is ./build/server/server and Client executable is ./build/client/client.

## Running
Start server first:

```
./build/server/server /tmp/multimeter.sock <Maximum number of channels>
```
Then launch client:

```
./build/client/client
```
The client is a simple window. To connect to the server use the default socket path and press "Connect" button. Try to change channels, and sending commands. The status of a chosen channel is displaying in window's bottom.