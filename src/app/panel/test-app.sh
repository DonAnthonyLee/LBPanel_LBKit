#!/bin/sh

if [ "x$LIBDIR" = "x" ]; then
LIBDIR="../../libs/lbk"
fi

if [ "x$PROG" = "x" ]; then
	if [ -x "./lb_panel" ]; then
		# for autoconf environment
		PROG="./lb_panel"
	elif [ -x "./LBPanel" ]; then
		PROG="./LBPanel"
	else
		echo "Error: no executable file!"
		exit 1
	fi
fi

LIBRARY_PATH="${LIBRARY_PATH}:${LIBDIR}" LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${LIBDIR}" ${DEBUGGER} $PROG $*

