Installing cHealpix
===================

cHealpix uses CMake for compiling its sources, so you have to install
it if you want to compile cHealpix. Once you've extracted the tarball,
enter the cHealpix directory and run the following commands::

    cd cHealpix-0.0.1
    mkdir build
    cd build
    cmake .. && make

Once the library has been built, you can check that the code works by
running a battery of tests on it::

    make test
