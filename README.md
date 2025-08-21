# tcl-cmark

This project implementes Tcl bindings to the cmark-gfm 
Github Flavoured CommonMark/Markdown library.

Version 1.1 adds support for Tcl 9.

To build for Windows using Visual Studio compilers, see the
instructions in win/makefile.vc. Currently the extension has been
built and tested with Visual Studio 2017. Older Visual C++ compilers
may or may not work as the cmark-gfm library uses C99-isms that might
not be supported.

For other platforms that support Tcl's TEA build framework, follow
the usual process of configure + make + make install. You will need
to have the cmark-gfm include files and libraries installed.

See the [Github actions](https://github.com/apnadkarni/tcl-cmark/tree/master/.github/workflows)
for examples for building for Windows, Linux and MacOS.
