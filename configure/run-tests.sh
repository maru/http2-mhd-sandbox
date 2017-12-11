#!/bin/bash
PREFIX_DIR=$HOME/dev/http2/usr/local

reset
autoreconf -fi || exit 1

VERBOSE=0
if [ "$1" == "-v" ]; then VERBOSE=1; fi

function run_test
{
  echo -e "\e[1m${1}\e[0m"
  ./configure -n --prefix=${PREFIX_DIR} ${1} | grep "$2"
  echo
}

function run_test_check
{
  echo -en "\e[1m${1}\e[0m ... "
  output=$(./configure -n --prefix=${PREFIX_DIR} ${1} 2>&1 | grep -c "$3")
  if [ "$output" -eq 1 ]; then
    echo -e "\e[1m\e[32mOK!\e[0m"
  else
    echo -e "\e[1m\e[31mFAILED\e[0m"
  fi
  if [ "$VERBOSE" -eq 1 ]; then
    ./configure -n --prefix=${PREFIX_DIR} ${1} 2>&1 | grep "$2"
  fi
  echo
}

GREP_PATTERN="HTTP/2\|http2\|HTTP2"

CHECK_PATTERN="^configure: error: cannot find usable libnghttp2 at specified prefix /path$"
run_test_check "--enable-http2 --with-nghttp2=/path"     "$GREP_PATTERN" "$CHECK_PATTERN"

CHECK_PATTERN="HTTP2 support:     yes (using libnghttp2)"
run_test_check "--enable-http2 --with-nghttp2=${PREFIX_DIR}" "$GREP_PATTERN" "$CHECK_PATTERN"

# These two tests can fail if libnghttp2 is not installed in the system (default directories).
CHECK_PATTERN="^configure: error: cannot find usable libnghttp2 at specified prefix"
run_test_check "--enable-http2"                              "$GREP_PATTERN" "$CHECK_PATTERN"
run_test_check "--enable-http2 --with-nghttp2"               "$GREP_PATTERN" "$CHECK_PATTERN"

CHECK_PATTERN="^configure: error: HTTP2 support cannot be enabled without libnghttp2.$"
run_test_check "--enable-http2 --with-nghttp2=no"     "$GREP_PATTERN" "$CHECK_PATTERN"

# HTTP/2 support:     no (disabled)
CHECK_PATTERN="^[[:space:]]*HTTP2 support:[[:space:]]*no (disabled)[[:space:]]*$"

run_test_check "-v"                                   "$GREP_PATTERN" "$CHECK_PATTERN"
run_test_check "--disable-http2"                      "$GREP_PATTERN" "$CHECK_PATTERN"
run_test_check "--with-nghttp2=${PREFIX_DIR}"         "$GREP_PATTERN" "$CHECK_PATTERN"
run_test_check "--disable-http2 --with-nghttp2"       "$GREP_PATTERN" "$CHECK_PATTERN"
run_test_check "--disable-http2 --with-nghttp2=/path" "$GREP_PATTERN" "$CHECK_PATTERN"
run_test_check "--disable-http2 --with-nghttp2=${PREFIX_DIR}" "$GREP_PATTERN" "$CHECK_PATTERN"
