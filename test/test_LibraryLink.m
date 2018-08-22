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

TestClose[a_List, b_List, eps_:10^-15] :=
    TestClose[Sequence @@ #, eps]& /@ MapThread[List, {a, b}];

TestClose[a_, b_, eps_:10^-15] :=
    If[Abs[(a - b)] < eps || (Abs[a] > eps && Abs[(a - b)/a] < eps),
       Print["Test passed: ", a, " ~ ", b, " within ", eps];
       passedTests++
       ,
       Print["Test failed: ", a, " !~ ", b, " within ", eps];
       failedTests++
       ,
       Print["Test failed: Could not evaluate expression to bool: ", Abs[a - b] < eps];
       failedTests++
      ];

sym = {
    Mxyzuv,
    Uzxyv, Uuyxv, Uxzuv, Uyuzv,
    Tvyz, Tuxv, Tyzv, Txuv, Tzyv, Tvxu,
    Svyz, Suxv,
    Bxz, Byu,
    Vzxyv, Vuyxv, Vxzuv, Vyuzv,
    TBARvyz, TBARuxv, TBARyzv, TBARxuv, TBARzyv, TBARvxu,
    Ixyv, Izuv,
    Ax, Ay, Az, Au, Av
};

Print[];
Print["Testing TSILEvaluateLoopFunctions ..."];
Print[];

TestClose[sym /. TSILEvaluateLoopFunctions[1, 2, 3, 4, 5, 10, 1],
          sym /. {
              (* obtained by ./tsil 1 2 3 4 5 10 1 *)
              Mxyzuv   ->  0.7183353535335331 +  0.3901621999727627 I,

              Uzxyv    -> -3.9926362044407706 + -1.7995145055126969 I,
              Uuyxv    -> -2.2323589397530124 + -0.0000000000000002 I,
              Uxzuv    -> -4.8569530649079544 + -2.1275603386772164 I,
              Uyuzv    -> -3.0864179723725735 + -0.0000000000000003 I,

              Tvyz     ->  0.4467752422961078 +  0.0000000000000001 I,
              Tuxv     -> -0.0303601769384458 +  0.0000000000000002 I,
              Tyzv     -> -1.6945169297078884 +  0.0000000000000008 I,
              Txuv     -> -3.0122117237897288 +  0.0000000000000003 I,
              Tzyv     -> -0.7861278798773168 + -0.0000000000000005 I,
              Tvxu     ->  0.5159165810473397 + -0.0000000000000003 I,

              Svyz     -> -7.6704797871895378 +  0.0000000000000014 I,
              Suxv     -> -9.5666067888028632 +  0.0000000000000003 I,

              Bxz      ->  0.7793038407369921 +  1.5390597961942369 I,
              Byu      -> -0.0515132849728505 + -0.0000000000000000 I,

              Vzxyv    ->  0.1961860092807476 + -0.9808434544663881 I,
              Vuyxv    -> -0.7859205937573606 +  0.0000000000000012 I,
              Vxzuv    ->  0.0181520588278395 + -0.7752579663009971 I,
              Vyuzv    -> -0.7543085167460677 +  0.0000000000000006 I,

              TBARvyz  ->  2.1585311655792454 +  0.5056198322111863 I,
              TBARuxv  ->  0.4379362305548289 +  0.0000000000000002 I,
              TBARyzv  -> -2.2324104469869441 +  0.0000000000000008 I,
              TBARxuv  -> -3.0122117237897288 +  0.0000000000000003 I,
              TBARzyv  -> -1.2678544111018766 + -0.0000000000000005 I,
              TBARvxu  ->  1.9498686438515711 +  1.5168594966335585 I,

              Ixyv     -> -2.4330986130738634 +  0.0000000000000000 I,
              Izuv     ->  5.1155528299660125 +  0.0000000000000000 I,

              Ax       -> -1.0000000000000000 +  0.0000000000000000 I,
              Ay       -> -0.6137056388801094 +  0.0000000000000000 I,
              Az       ->  0.2958368660043291 +  0.0000000000000000 I,
              Au       ->  1.5451774444795625 +  0.0000000000000000 I,
              Av       ->  3.0471895621705021 +  0.0000000000000000 I

          }];

Print[];
Print["Testing TSILA ..."];
Print[];

TestClose[Ax /. TSILEvaluateLoopFunctions[1, 2, 3, 4, 5, 10, 1],
          TSILA[1, 1]];

Print["Number of passed tests: ", passedTests];
Print["Number of failed tests: ", failedTests];

Quit[failedTests];
