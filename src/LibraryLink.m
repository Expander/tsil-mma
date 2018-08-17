TSILInitialize::usage = "Loads the TSIL LibraryLink.  After
 TSILInitialize[] has been called, the function
 TSILEvaluateLoopFunctions[] is available.

Usage:

  TSILInitialize[libName];

Arguments:

 - libName - The path to the TSIL LibraryLink, 'LibraryLink.so'

";

TSILEvaluateLoopFunctions::usage = "";

(* output parameters *)
{};


TSIL::nonum = "Error: `1` is not a numeric input value!";
TSIL::error = "`1`";
TSIL::info  = "`1`";
TSILErrorMessage[s_] := Message[TSIL::error, s];
TSILInfoMessage[s_]  := Message[TSIL::info, s];

Begin["`Private`"];

TSILInitialize[libName_String] := (
       TSILEvaluateLoopFunctionsLL =
          LibraryFunctionLoad[libName, "TSILEvaluateLoopFunctions", LinkObject, LinkObject];
    );

TSILEvaluateLoopFunctions[x_, y_, z_, u_, v_, s_, qq_] :=
    TSILEvaluateLoopFunctionsLL[{x, y, z, u, v, s, qq}];

End[];
