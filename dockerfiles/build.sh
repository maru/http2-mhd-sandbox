#!/bin/bash
set -e
set -v

export LD_LIBRARY_PATH=/usr/local/lib

cp -r /tmp/mhd2 /usr/src/mhd2
cd /usr/src/mhd2

function check_mhd
{
	MHD_OPTIONS="-q --enable-https --disable-doc --enable-silent-rules --enable-asserts --disable-examples"
	MHD_OPTIONS="${MHD_OPTIONS} $1"
	
	echo -e "\033[38;5;219m============================================================================"
	echo -e "./configure ${MHD_OPTIONS}"
	echo -e "============================================================================\033[00m"

	(make -s distclean || true)
	./bootstrap --force

	./configure ${MHD_OPTIONS} 
	make -s
	echo '#undef HAVE_INET6' >> MHD_config.h 
	make -s check 
}

check_mhd "--disable-curl --enable-http2"
check_mhd "--enable-http2"
check_mhd
