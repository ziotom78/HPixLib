Using the library
=================

To use HPixLib in your C/C++ program, include the header file
``hpixlib/hpix.h`` at the beginning of your code:

.. code-block:: c

  #include <hpixlib/hpix.h>

To compile the library, you can use pkg-config::

  cc my_program.c `pkg-config --cflags --libs libhpix`

Some parts of HPixLib can take advantage of the Cairo library. To
determine if HPixLib was compiled with the Cairo support enable or
not, use `pkg-config` with `--variable=havecairo`::

  pkg-config --variable=havecairo libhpix

It returns ``no`` if HPixLib does not provide Cairo support, ``yes``
otherwise. In the latter case, you must include the header file
``hpixlib/hpix-cairo.h`` as well:

.. code-block:: c

  #include <hpixlib/hpix.h>
  #include <hpixlib/hpix-cairo.h>
