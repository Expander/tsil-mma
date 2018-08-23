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

Print["A(x,qq)     = ", TSILA[x, qq]];
Print["Ap(x,qq)    = ", TSILAp[x, qq]];
Print["Aeps(x,qq)  = ", TSILAeps[x, qq]];
Print["B(x,y,s,qq) = ", TSILB[x, y, s, qq]];
Print["I(x,y,z,qq) = ", TSILI[x, y, z, qq]];
