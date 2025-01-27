TSIL-Mma
========

TSIL-Mma provides a Mathematica interface for the two-loop self-energy
integral library [TSIL](https://www.niu.edu/spmartin/TSIL/) [[CPC 174
(2006) 133-151](https://arxiv.org/abs/hep-ph/0501132)].  TSIL is
written by Stephen P. Martin and David G. Robertson.

Building TSIL
----------------

First, TSIL must be [downloaded](https://www.niu.edu/spmartin/TSIL/)
and extracted.  Afterwards, edit the TSIL `Makefile` and add `-fPIC`
to `TSIL_OPT`:

```mak
TSIL_OPT = -O3 -funroll-loops -fPIC
```

In addition, we recommend to enable long double precision in the TSIL
`Makefile` (enabled by default):

```mak
TSIL_SIZE = -DTSIL_SIZE_LONG
```

Afterwards, run

```sh
make
```

Building TSIL-Mma
-----------------

To build TSIL-Mma, first create a build sub-directory and enter that
directory:

```sh
mkdir build
cd build
```

Then, call `cmake` to search for the Mathematica installation and for
the TSIL library and header files:

```sh
cmake -DTSIL_DIR=/path/to/tsil -DCMAKE_CXX_FLAGS=-DTSIL_SIZE_LONG ..
```

The variable `CMAKE_CXX_FLAGS` must be set to the value of `TSIL_SIZE`
that has been set in the TSIL `Makefile` (see above):

* If TSIL has been build with long double precision (`TSIL_SIZE = -DTSIL_SIZE_LONG`), pass to cmake:

  ```mak
  -DCMAKE_CXX_FLAGS=-DTSIL_SIZE_LONG
  ```

  This is the default TSIL configuration.

* If TSIL has been build with double precision (`TSIL_SIZE = -DTSIL_SIZE_DOUBLE`), pass to cmake:

  ```mak
  -DCMAKE_CXX_FLAGS=-DTSIL_SIZE_DOUBLE
  ```

Finally TSIL-Mma can be build by running

```sh
make
```

Usage
-----

To call the TSIL functions from Mathematica one has to first include
the file `src/LibraryLink.m` (assuming one is currently in the `build`
directory):

```wl
Get[FileNameJoin[{"..", "src", "LibraryLink.m"}]];
```

Afterwards, the TSIL-Mma LibraryLink library must be loaded using the
function `TSILInitialize[libName]`.  The function argument `libName`
is the name of the created library, including the path.  On Linux the
library is called `LibraryLink.so`, on MacOS it is called
`LibraryLink.dylib`.  Thus, on Linux you may write:

```wl
libName = FileNameJoin[{"src", "LibraryLink.so"}];
TSILInitialize[libName];
```

When these two steps are done, the TSIL functions can be called, for
example:

```wl
x  = 1;
y  = 2;
z  = 3;
u  = 4;
v  = 5;
s  = 10;
qq = 1;

(* evaluate all TSIL integral functions *)
Print[TSILEvaluate[x, y, z, u, v, s, qq]];

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
```

A full example script can be found in `example/example.m`.
It can be run from the `build` directory as:

```wl
math -run '<< "../example/example.m"'
```

Author
------

TSIL-Mma is written by Alexander Voigt, RWTH Aachen University.

Copying
-------

TSIL-Mma is licenced under the GNU General Public License (GNU GPL)
version 3.
