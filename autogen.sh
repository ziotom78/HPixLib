#!/bin/sh

touch README & libtoolize && aclocal && autoheader && autoconf && automake --add-missing
