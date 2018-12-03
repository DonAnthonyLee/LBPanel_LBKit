#!/bin/sh

# clean
if test -L Makefile && test "X$1" = X--clean; then
	make clean
	FILES=`find ./ -name "Makefile"`
	for f in $FILES; do
		test ! -L $f || rm -f $f
	done
	exit 0
elif test -f Makefile && test "X$1" = X--clean; then
	make distclean
	exit 0
fi

# link
if test ! -e Makefile && test "X$1" = X--link; then
	FILES=`find ./ -name "Makefile.alone"`
	for f in $FILES; do
		ln -sf ./Makefile.alone `dirname $f`/Makefile
	done
	exit 0
fi

# config
if test ! -f configure && test "X$1" = X--config; then
	FILES=`find ./ -name "Makefile"`
	for f in $FILES; do
		test ! -L $f || rm -f $f
	done
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
echo "    --link          link Makefile.alone to Makefile"

