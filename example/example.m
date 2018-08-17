(* load function definitions *)
Get[FileNameJoin[{"..", "src", "LibraryLink.m"}]];

(* initialize the LibrayLink *)
TSILInitialize[FileNameJoin[{"src", "LibraryLink.so"}]];

Print @ TSILEvaluateLoopFunctions[1, 2, 3, 4, 5, 10, 1]
