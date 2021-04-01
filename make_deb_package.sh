#!/bin/bash

cd /Anna
tar -zcvf ../anna_0.1.0.orig.tar.gz . 

# Test if it compiles
qmake -o Makefile anna.pro 

pdebuild --debbuildopts -sa

dpkg-buildpackage -rfakeroot
