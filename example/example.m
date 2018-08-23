(* load function definitions *)
Get[FileNameJoin[{"..", "src", "LibraryLink.m"}]];

(* initialize the LibrayLink *)
TSILInitialize[FileNameJoin[{"src", "LibraryLink.so"}]];

x  = 1;
y  = 2;
z  = 3;
u  = 4;
v  = 5;
s  = 10;
qq = 1;

Print[TSILEvaluateLoopFunctions[x, y, z, u, v, qq, s]];

Print["A(x,qq)          = ", TSILA[x, qq]];
Print["Ap(x,qq)         = ", TSILAp[x, qq]];
Print["Aeps(x,qq)       = ", TSILAeps[x, qq]];
Print["B(x,y,s,qq)      = ", TSILB[x, y, s, qq]];
Print["I(x,y,z,qq)      = ", TSILI[x, y, z, qq]];
Print["M(x,y,z,u,v,s)   = ", TSILM[x, y, z, u, v, s]];
Print["S(x,y,z,s,qq)    = ", TSILS[x, y, z, s, qq]];
Print["T(x,y,z,s,qq)    = ", TSILT[x, y, z, s, qq]];
Print["Tbar(x,y,z,s,qq) = ", TSILTbar[x, y, z, s, qq]];
Print["U(x,y,z,u,s,qq)  = ", TSILU[x, y, z, u, s, qq]];
