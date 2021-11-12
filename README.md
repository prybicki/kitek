# C++ software for a custom-made RPi-based teleoperated robot

# Building:

cmake -DCMAKE_TOOLCHAIN_FILE=<armv6hf toolchain file, eg. from ng> <path to source>

# Notes:

Static linking requires requires to cross-compile pigpio, which depends on pthread and you don't want to cross-compile pthread :)
