#!/bin/sh

LIBDIR="../../libs/lbk"

if [ -x "./lb_panel" ]; then
	# for autoconf environment
	PROG="./lb_panel"
elif [ -x "./LBPanel" ]; then
	PROG="./LBPanel"
else
	echo "Error: no executable file!"
	exit 1
fi

LIBRARY_PATH="${LIBRARY_PATH}:${LIBDIR}" LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${LIBDIR}" $PROG $*

