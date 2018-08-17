TSIL-Mma
========

TSIL-Mma provides a Mathematica interface for the two-loop self-energy
integral library [TSIL](https://www.niu.edu/spmartin/TSIL/) [[CPC 174
(2006) 133-151](https://arxiv.org/abs/hep-ph/0501132)].  TSIL is
written by Stephen P. Martin and David G. Robertson.

Building TSIL-Mma
-----------------

To build TSIL-Mma, first create a build sub-directory and enter that
directory:

    mkdir build
    cd build

Then, call `cmake` to search for the Mathematica installation and for
the TSIL library and header files:

    cmake -DTSIL_DIR=$TSIL_DIR ..

where `$TSIL_DIR` points to the TSIL source directory.  Note, that
TSIL must have been build with `-fPIC` in order to create the
LibraryLink Mathematica interface.  Finally, build TSIL-Mma by running

    make

Example
-------

An example script can be found in `example/example.m`.
It can be run from the build directory for example as:

    math -run "<< \"../example/example.m\""

Author
------

Alexander Voigt

Copying
-------

Polylogarithm is licenced under the GNU Lesser General Public License
(GNU LGPL) version 3.
