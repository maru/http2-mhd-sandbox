#!/bin/bash

PREFIX_DIR=/home/maru/dev/http2/usr/local

autoconf

function run_test
{
  echo -e "\e[1m\e[31m${1}\e[0m"
  ./configure --prefix=${PREFIX_DIR} ${1} | grep ${2}
  echo
}

run_test "-v" "HTTP/2"
run_test "--disable-http2" "HTTP/2"
run_test "--disable-http2 --with-nghttp2" "HTTP/2"
run_test "--disable-http2 --with-nghttp2=/path" "HTTP/2"
run_test "--disable-http2 --with-nghttp2=${PREFIX_DIR}" "HTTP/2"

run_test "--enable-http2" "HTTP/2"
run_test "--enable-http2 --with-nghttp2" "HTTP/2"
run_test "--enable-http2 --with-nghttp2=/path" "HTTP/2"
run_test "--enable-http2 --with-nghttp2=${PREFIX_DIR}" "HTTP/2"
