#!/bin/bash

# This pulls a docker image with qt5

sudo docker run -it -v $PWD/..:/tmp lejeunel/ubuntu18_debbuild\
     /bin/bash -c "cd /tmp/${PWD##*/}; ls; ./make_deb_package.sh"
