TSILInitialize::usage = "Loads the TSIL LibraryLink.  After
 TSILInitialize[] has been called, the function
 TSILEvaluateLoopFunctions[] is available.

Usage:

  TSILInitialize[libName];

Arguments:

 - libName - The path to the TSIL LibraryLink, 'LibraryLink.so'

";

TSILEvaluateLoopFunctions::usage = "Parameters: x, y, z, u, v, s, qq";
TSILA::usage = "A(x,Q^2)";

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
       TSILEvaluateLoopFunctionsLL =
          LibraryFunctionLoad[libName, "TSILEvaluateLoopFunctions", LinkObject, LinkObject];
       TSILALL =
          LibraryFunctionLoad[libName, "TSILA", LinkObject, LinkObject];
    );

TSILEvaluateLoopFunctions[x_?NumericQ, y_?NumericQ, z_?NumericQ, u_?NumericQ, v_?NumericQ, s_?NumericQ, qq_?NumericQ] :=
    TSILEvaluateLoopFunctionsLL[{x, y, z, u, v, s, qq}];

TSILA[x_?NumericQ, qq_?NumericQ] := TSILALL[{x, qq}];

End[];
