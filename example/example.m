(* load function definitions *)
Get[FileNameJoin[{"..", "src", "LibraryLink.m"}]];

(* initialize the LibrayLink *)
TSILInitialize[FileNameJoin[{"src", "LibraryLink.so"}]];

x  = 1;
y  = 2;
z  = 3;
u  = 4;
v  = 5;
s  = 10 + I;
qq = 1;

Print[TSILEvaluate[x, y, z, u, v, s, qq]];

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
