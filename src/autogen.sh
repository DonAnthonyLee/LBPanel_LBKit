#!/bin/sh

# clean
if test -f Makefile && test "X$1" = X--clean; then
	make distclean
	exit 0
fi

# config
if test ! -f configure && test "X$1" = X--config; then
	if test "X$OSTYPE" = Xmsys; then
		${ACLOCAL-aclocal} -I ${ACDIR-/MinGW/share/aclocal}
	elif test "X$BE_HOST_CPU" != X; then
		${ACLOCAL-aclocal} -I ${ACDIR-/boot/home/config/share/aclocal}
	else
		${ACLOCAL-aclocal} -I ${ACDIR-/usr/local/share/aclocal}
	fi
	${AUTOCONF-autoconf}
	${LIBTOOLIZE-libtoolize} --force --copy
	${AUTOMAKE-automake} --copy --add-missing
	echo ""
	echo "*******************************************"
	echo "Run \"./configure; make\" for what you want." 
	echo "*******************************************"
	exit 0
fi

# failed
if test "X$1" = X--clean; then
	echo "***********************************************************"
	echo "\"Makefile\" don't exist, you should run \"./configure\" first."
	echo "***********************************************************"
	exit 0
elif test "X$1" = X--config; then
	echo "***************************"
	echo "\"configure\" already exists."
	echo "***************************"
	exit 0
fi

# show usage
echo "Usage: ./autogen.sh [option]"
echo ""
echo "Option:"
echo "    --config        generate the configure script"
echo "    --clean         clean the script files"


