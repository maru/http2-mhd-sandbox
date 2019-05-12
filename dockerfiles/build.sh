#!/bin/bash
set -e
set -v

MHD_OPTIONS="--enable-https --disable-doc --enable-silent-rules --enable-asserts --disable-examples"
export LD_LIBRARY_PATH=/usr/local/lib

cp -r /tmp/mhd2 /usr/src/mhd2
cd /usr/src/mhd2

(make -s clean || true)
./bootstrap

function check_http1
{
	echo "Testing HTTP/1: ./configure -q ${MHD_OPTIONS}"
	./configure -q ${MHD_OPTIONS}
	echo '#undef HAVE_INET6' >> MHD_config.h
	make -s && make -s check
}

function check_http2
{
	echo "Testing HTTP/2 ./configure -q ${MHD_OPTIONS} --enable-http2"
	./configure -q ${MHD_OPTIONS} --enable-http2 
	echo '#undef HAVE_INET6' >> MHD_config.h 
	make -s && make -s check 
}

check_http2
check_http1
