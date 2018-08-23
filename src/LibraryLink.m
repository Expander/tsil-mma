TSILInitialize::usage = "Loads the TSIL LibraryLink.  After
 TSILInitialize[] has been called, the integral functions are
 available.

Usage:

  TSILInitialize[libName];

Arguments:

 - libName - The path to the TSIL LibraryLink, 'LibraryLink.so'
";

TSILEvaluate::usage = "Evaluate all integral functions. 
Parameters: x, y, z, u, v, s, qq";
TSILA::usage = "A(x,Q^2)";
TSILAp::usage = "Ap(x,Q^2)";
TSILAeps::usage = "Aeps(x,Q^2)";
TSILB::usage = "B(x,y,s,Q^2)";
TSILBp::usage = "Bp(x,y,s,Q^2)";
TSILdBds::usage = "dBds(x,y,s,Q^2)";
TSILBeps::usage = "Beps(x,y,s,Q^2)";
TSILI::usage = "I(x,y,z,Q^2)";
TSILIp::usage = "Ip(x,y,z,Q^2)";
TSILIp2::usage = "Ip2(x,y,z,Q^2)";
TSILIpp::usage = "Ipp(x,y,z,Q^2)";
TSILIp3::usage = "Ip3(x,y,z,Q^2)";
TSILM::usage = "M(x,y,z,u,v,s)";
TSILS::usage = "S(x,y,z,s,qq)";
TSILT::usage = "T(x,y,z,s,qq)";
TSILTbar::usage = "Tbar(x,y,z,s,qq)";
TSILU::usage = "U(x,y,z,u,s,qq)";
TSILV::usage = "U(x,y,z,u,s,qq)";

(* output parameters *)
{ Mxyzuv,
  Uzxyv, Uuyxv, Uxzuv, Uyuzv,
  Tvyz, Tuxv, Tyzv, Txuv, Tzyv, Tvxu,
  Svyz, Suxv,
  Bxz, Byu,
  Vzxyv, Vuyxv, Vxzuv, Vyuzv,
  TBARvyz, TBARuxv, TBARyzv, TBARxuv, TBARzyv, TBARvxu,
  Ixyv, Izuv,
  Ax, Ay, Az, Au, Av };

TSIL::nonum = "Error: `1` is not a numeric input value!";
TSIL::error = "`1`";
TSIL::info  = "`1`";
TSILErrorMessage[s_] := Message[TSIL::error, s];
TSILInfoMessage[s_]  := Message[TSIL::info, s];

Begin["`Private`"];

TSILInitialize[libName_String] := (
       TSILEvaluateLL =
          LibraryFunctionLoad[libName, "TSILEvaluate", LinkObject, LinkObject];
       TSILALL =
          LibraryFunctionLoad[libName, "TSILA", LinkObject, LinkObject];
       TSILApLL =
          LibraryFunctionLoad[libName, "TSILAp", LinkObject, LinkObject];
       TSILAepsLL =
          LibraryFunctionLoad[libName, "TSILAeps", LinkObject, LinkObject];
       TSILBLL =
          LibraryFunctionLoad[libName, "TSILB", LinkObject, LinkObject];
       TSILBpLL =
          LibraryFunctionLoad[libName, "TSILBp", LinkObject, LinkObject];
       TSILdBdsLL =
          LibraryFunctionLoad[libName, "TSILdBds", LinkObject, LinkObject];
       TSILBepsLL =
          LibraryFunctionLoad[libName, "TSILBeps", LinkObject, LinkObject];
       TSILILL =
          LibraryFunctionLoad[libName, "TSILI", LinkObject, LinkObject];
       TSILIpLL =
          LibraryFunctionLoad[libName, "TSILIp", LinkObject, LinkObject];
       TSILIp2LL =
          LibraryFunctionLoad[libName, "TSILIp2", LinkObject, LinkObject];
       TSILIppLL =
          LibraryFunctionLoad[libName, "TSILIpp", LinkObject, LinkObject];
       TSILIp3LL =
          LibraryFunctionLoad[libName, "TSILIp3", LinkObject, LinkObject];
       TSILMLL =
          LibraryFunctionLoad[libName, "TSILM", LinkObject, LinkObject];
       TSILSLL =
          LibraryFunctionLoad[libName, "TSILS", LinkObject, LinkObject];
       TSILTLL =
          LibraryFunctionLoad[libName, "TSILT", LinkObject, LinkObject];
       TSILTbarLL =
          LibraryFunctionLoad[libName, "TSILTbar", LinkObject, LinkObject];
       TSILULL =
          LibraryFunctionLoad[libName, "TSILU", LinkObject, LinkObject];
       TSILVLL =
          LibraryFunctionLoad[libName, "TSILV", LinkObject, LinkObject];
    );

TSILEvaluate[x_?NumericQ, y_?NumericQ, z_?NumericQ, u_?NumericQ, v_?NumericQ, s_?NumericQ, qq_?NumericQ] :=
    TSILEvaluateLL[{x, y, z, u, v, s, qq}];

TSILA[x_?NumericQ, qq_?NumericQ] := TSILALL[N @ {x, qq}];

TSILAp[x_?NumericQ, qq_?NumericQ] := TSILApLL[N @ {x, qq}];

TSILAeps[x_?NumericQ, qq_?NumericQ] := TSILAepsLL[N @ {x, qq}];

TSILB[x_?NumericQ, y_?NumericQ, s_?NumericQ, qq_?NumericQ] := TSILBLL[N @ {x, y, s, qq}];

TSILBp[x_?NumericQ, y_?NumericQ, s_?NumericQ, qq_?NumericQ] := TSILBpLL[N @ {x, y, s, qq}];

TSILdBds[x_?NumericQ, y_?NumericQ, s_?NumericQ, qq_?NumericQ] := TSILdBdsLL[N @ {x, y, s, qq}];

TSILBeps[x_?NumericQ, y_?NumericQ, s_?NumericQ, qq_?NumericQ] := TSILBepsLL[N @ {x, y, s, qq}];

TSILI[x_?NumericQ, y_?NumericQ, z_?NumericQ, qq_?NumericQ] := TSILILL[N @ {x, y, z, qq}];

TSILIp[x_?NumericQ, y_?NumericQ, z_?NumericQ, qq_?NumericQ] := TSILIpLL[N @ {x, y, z, qq}];

TSILIp2[x_?NumericQ, y_?NumericQ, z_?NumericQ, qq_?NumericQ] := TSILIp2LL[N @ {x, y, z, qq}];

TSILIpp[x_?NumericQ, y_?NumericQ, z_?NumericQ, qq_?NumericQ] := TSILIppLL[N @ {x, y, z, qq}];

TSILIp3[x_?NumericQ, y_?NumericQ, z_?NumericQ, qq_?NumericQ] := TSILIp3LL[N @ {x, y, z, qq}];

TSILM[x_?NumericQ, y_?NumericQ, z_?NumericQ, u_?NumericQ, v_?NumericQ, s_?NumericQ] := TSILMLL[N @ {x, y, z, u, v, s}];

TSILS[x_?NumericQ, y_?NumericQ, z_?NumericQ, s_?NumericQ, qq_?NumericQ] := TSILSLL[N @ {x, y, z, s, qq}];

TSILT[x_?NumericQ, y_?NumericQ, z_?NumericQ, s_?NumericQ, qq_?NumericQ] := TSILTLL[N @ {x, y, z, s, qq}];

TSILTbar[x_?NumericQ, y_?NumericQ, z_?NumericQ, s_?NumericQ, qq_?NumericQ] := TSILTbarLL[N @ {x, y, z, s, qq}];

TSILU[x_?NumericQ, y_?NumericQ, z_?NumericQ, u_?NumericQ, s_?NumericQ, qq_?NumericQ] := TSILULL[N @ {x, y, z, u, s, qq}];

TSILV[x_?NumericQ, y_?NumericQ, z_?NumericQ, u_?NumericQ, s_?NumericQ, qq_?NumericQ] := TSILVLL[N @ {x, y, z, u, s, qq}];

End[];
