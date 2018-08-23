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
LibraryLink Mathematica interface.  Finally TSIL-Mma can be build by
running

    make

Usage
-----

To call the TSIL functions from Mathematica one has to first include
the file `src/LibraryLink.m` (assuming one is currently in the `build`
directory):

    Get[FileNameJoin[{"..", "src", "LibraryLink.m"}]];

Afterwards, TSIL-Mma LibraryLink library must be loaded using the
`TSILInitialize[libName]` function, which takes as argument
(`libName`) the path to the created `LibraryLink.so` library:

    libName = FileNameJoin[{"src", "LibraryLink.so"}];
    TSILInitialize[libName];

When these two steps are done, the TSIL functions can be called, for
example:

    x  = 1;
    y  = 2;
    z  = 3;
    u  = 4;
    v  = 5;
    s  = 10;
    qq = 1;

    (* evaluate all TSIL integral functions *)
    Print[TSILEvaluate[x, y, z, u, v, qq, s]];

    (* individual TSIL integral functions *)
    Print["A(x,qq)          = ", TSILA[x, qq]];
    Print["Ap(x,qq)         = ", TSILAp[x, qq]];
    Print["Aeps(x,qq)       = ", TSILAeps[x, qq]];
    Print["B(x,y,s,qq)      = ", TSILB[x, y, s, qq]];
    Print["Bp(x,y,s,qq)     = ", TSILBp[x, y, s, qq]];
    Print["dBds(x,y,s,qq)   = ", TSILdBds[x, y, s, qq]];
    Print["Beps(x,y,s,qq)   = ", TSILBeps[x, y, s, qq]];
    Print["I(x,y,z,qq)      = ", TSILI[x, y, z, qq]];
    Print["Ip(x,y,z,qq)     = ", TSILIp[x, y, z, qq]];
    Print["Ip2(x,y,z,qq)    = ", TSILIp2[x, y, z, qq]];
    Print["Ipp(x,y,z,qq)    = ", TSILIpp[x, y, z, qq]];
    Print["Ip3(x,y,z,qq)    = ", TSILIp3[x, y, z, qq]];
    Print["M(x,y,z,u,v,s)   = ", TSILM[x, y, z, u, v, s]];
    Print["S(x,y,z,s,qq)    = ", TSILS[x, y, z, s, qq]];
    Print["T(x,y,z,s,qq)    = ", TSILT[x, y, z, s, qq]];
    Print["Tbar(x,y,z,s,qq) = ", TSILTbar[x, y, z, s, qq]];
    Print["U(x,y,z,u,s,qq)  = ", TSILU[x, y, z, u, s, qq]];
    Print["V(x,y,z,u,s,qq)  = ", TSILV[x, y, z, u, s, qq]];

A full example script can be found in `example/example.m`.
It can be run from the `build` directory as:

    math -run "<< \"../example/example.m\""

Author
------

TSIL-Mma is written by Alexander Voigt, RWTH Aachen University.

Copying
-------

TSIL-Mma is licenced under the GNU General Public License (GNU GPL)
version 3.
