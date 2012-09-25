Installing HPixLib
==================

HPixLib uses the GNU Autotools for compiling its sources
(http://en.wikipedia.org/wiki/GNU_build_system), so there are
relatively little dependencies to satisfy in order to bootstrap the
compilation.

If you obtained a .tar.gz file, you should simply untar it somewhere
in your home, enter the directory :file:`hpixlib-???` (where `???` is
the version number) and run the following commands:

    ./configure
    make && sudo make install

If you have cloned the Git repository
(https://github.com/ziotom78/hpixlib), then you first have to run
autogen.sh:

    ./autogen.sh
    ./configure
    make && sudo make install

This step requires your system to have the autotools (i.e. autoconf,
automake, libtool and m4) already installed.
