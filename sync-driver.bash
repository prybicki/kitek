#!/bin/bash

make -C build-armv6 kitek -j
scp data/config.yaml build-armv6/kitek pi:~/
