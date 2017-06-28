# Makefile for examples
.PHONY: libevent-server nghttp2-server

DEFS = -DHAVE_UNISTD_H -DHAVE_FCNTL_H
INCLUDES = -I../../usr/local/include/ -L../../usr/local/lib
LIBS = -lnghttp2 -levent_openssl -levent -lssl -lcrypto -ldl  -pthread
COMPILE = gcc $(DEFS) -g -o $@ $(INCLUDES) $@.c $(LIBS)

# nghttp2 server example
libevent-server:
	rm -f $@
	$(COMPILE)

# nghttp2 server example - no TLS
nghttp2-server:
	rm -f $@
	$(COMPILE)
