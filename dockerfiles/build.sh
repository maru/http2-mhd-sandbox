#!/bin/sh

MHD_OPTIONS="--enable-https --disable-doc --enable-silent-rules --enable-asserts --disable-examples"
export LD_LIBRARY_PATH=/usr/local/lib
cp -r /tmp/mhd2 . \
	&& cd mhd2 \
	&& (make clean || true) \
	&& ./bootstrap \
	&& true || (\
		./configure ${MHD_OPTIONS} \
		&& echo '#undef HAVE_INET6' >> MHD_config.h \
		&& make && make check \
	   )\
	&& (\
		./configure ${MHD_OPTIONS} --enable-http2 \
		&& echo '#undef HAVE_INET6' >> MHD_config.h \
		&& make && make check \
	   )
