#!/bin/sh

LIBRARY_PATH=${LIBRARY_PATH}:../../libs/lbk
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:../../libs/lbk
exec ./LBPanel $*
