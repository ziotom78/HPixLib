Command-line utilities
===================

In the utilities directory there are a few non-trivial programs using
HPixLib. They are automatically build and installed with the library,
and provide a few useful utilities that can be used e.g. in bash
scripts.

map2fig
-------

This utility is compiled only if an useable version of the Cairo
library (http://www.cairographics.org) is found at compilation time.
It converts a map in FITS format into a figure. It can support the
following formats:

   * PNG (bitmapped format)
   * PostScript and Encapsulated PostScript (EPS, a vector format)
   * PDF (Adobe Portable Document Format, a vector format)
   * SVG (Scalar Vector Graphics, a vector format)

(Some formats might not be available, depending on settings used in
compiling the Cairo library.) Vector formats contain a bitmapped
representation of the map, but the text and color bar are vector
elements that can be modified by vector drawing programs like e.g.
Inkscape (http://inkscape.org).

Run `map2fig --help` for a list of options.
