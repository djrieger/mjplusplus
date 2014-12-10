#!/bin/sh

BINDIR=$(dirname $(readlink -e $0))
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BINDIR/libfirm/build/debug $BINDIR/mj++ "$@"
