# Build libmicrohttpd-http2 : MHD with HTTP/2 support
#
# Maru Berezin 
# https://github.com/maru/libmicrohttpd-http2
#

FROM debian:stable-slim

ENV DEBIAN_FRONTEND noninteractive

# Versions
ENV NGHTTP2_VERSION 1.37.0
ENV CURL_VERSION    7.64.1

RUN apt-get update && apt-get install -y \
	apt-transport-https \
	ca-certificates \
	gnupg \
	wget \
	--no-install-recommends

# Install dependencies
RUN apt-get update && apt-get install -y \
	autopoint \
	automake \
	clang \
	g++ \
	gcc-mingw-w64 \
	libtool \
	libgnutls28-dev \
	make \
	pkgconf \
	socat \
	zzuf \
	--no-install-recommends

# Install nghttp2
RUN wget -qO - https://github.com/nghttp2/nghttp2/releases/download/v${NGHTTP2_VERSION}/nghttp2-${NGHTTP2_VERSION}.tar.xz \
	|  tar Jxf - \ 
	&& ( \
		cd nghttp2-${NGHTTP2_VERSION} \
		&& ./configure --enable-lib-only \
		&& make \
		&& make install \
	   ) \
	&& rm -fr nghttp2-${NGHTTP2_VERSION}

# Install curl
RUN wget -qO - https://curl.haxx.se/download/curl-${CURL_VERSION}.tar.gz \
	|  tar zxf - \
	&& ( \
		cd curl-${CURL_VERSION} \
		&& ./configure --with-nghttp2 --without-ssl --with-gnutls \
		&& make \
		&& make install \
	   ) \
	&& rm -fr curl-${CURL_VERSION}

RUN echo "export PS1='${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ '" >> /root/.bashrc

# Clean apt
RUN apt-get clean \
	&& rm -rf /var/lib/apt/lists/*

# Copy build.sh
COPY build.sh /build.sh
RUN chmod +x /build.sh

# Autorun
# ENTRYPOINT [ "./build.sh" ]
