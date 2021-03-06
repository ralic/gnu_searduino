#!/bin/bash

DIST=raspbian

FUNC=$(dirname $0)/build-functions
if [ -f $FUNC ]
then
    . $FUNC
else
    echo "Failed finding functions file ....."
    exit 1
fi

dobuild --cflags --enable-debian-sources $*

