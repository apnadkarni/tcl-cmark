# tcl-cmark

This project implementes [doc/tclmark.md](Tcl bindings) to the cmark-gfm 
Github Flavoured CommonMark/Markdown library.

To build for Windows using Visual Studio compilers, see the
instructions in win/makefile.vc. Currently the extension has been
built and tested with Visual Studio 2017. Older Visual C++ compilers
may or may not work as the cmark-gfm library uses C99-isms that might
not be supported.

For other platforms that support Tcl's TEA build framework, follow
the usual process of configure + make + make install. You will need
to have the cmark-gfm include files and libraries installed.

Doing TEA builds with MingW on Windows currently does not work 
because the cmark-gfm build fails with a configuration error when
trying to build the library. If you can get that to work, it should
be possible to build this extension as well using the above steps
for TEA.
