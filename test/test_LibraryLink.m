(* load LibrayLink *)
Get[FileNameJoin[{DirectoryName[$InputFileName], "..", "src", "LibraryLink.m"}]];
TSILInitialize[libPath];

passedTests = 0;
failedTests = 0;

TestEqual[a_, b_] :=
    If[a === b,
       Print["Test passed: ", a, " === ", b];
       passedTests++
       ,
       Print["Test failed: ", a, " =!= ", b];
       failedTests++
      ];

lf = TSILEvaluateLoopFunctions[1, 2, 3, 4, 5, 10, 1];

TestEqual[lf, {}];

Print["Number of passed tests: ", passedTests];
Print["Number of failed tests: ", failedTests];

Quit[failedTests];
