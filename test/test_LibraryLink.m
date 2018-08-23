Needs["NumericalCalculus`"];

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

PrintHeadline[a___] := (
    Print[];
    Print["=================================================="];
    Print[a];
    Print["=================================================="];
    Print[];
);

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

x  = 1;
y  = 2;
z  = 3;
u  = 4;
v  = 5;
s  = 10;
qq = 1;

(* obtained by ./tsil 1 2 3 4 5 10 1 *)
results = {
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
};

PrintHeadline["Testing TSILEvaluate"];

TestClose[sym /. TSILEvaluate[x, y, z, u, v, s, qq],
          sym /. results];

PrintHeadline["Testing TSILA"];

TestClose[Ax /. results,
          TSILA[x, qq]];

PrintHeadline["Testing TSILB"];

TestClose[Bxz /. results,
          TSILB[x, z, s, qq]];

TestClose[Byu /. results,
          TSILB[y, u, s, qq]];

PrintHeadline["Testing TSILBp"];

TestClose[ND[TSILB[xx, y, s, qq], xx, x],
          TSILBp[x, y, s, qq], 1*^-9];

PrintHeadline["Testing TSILdBds"];

TestClose[ND[TSILB[x, y, ss, qq], ss, s],
          TSILdBds[x, y, s, qq], 1*^-9];

PrintHeadline["Testing TSILI"];

TestClose[Ixyv /. results,
          TSILI[x, y, v, qq]];

TestClose[Izuv /. results,
          TSILI[z, u, v, qq]];

PrintHeadline["Testing TSILIp"];

TestClose[ND[TSILI[xx, y, s, qq], xx, x],
          TSILIp[x, y, s, qq], 1*^-9];

PrintHeadline["Testing TSILIp2"];

TestClose[ND[TSILI[xx, y, s, qq], {xx, 2}, x],
          TSILIp2[x, y, s, qq], 1*^-6];

PrintHeadline["Testing TSILIpp"];

DI1[x_?NumericQ, y_?NumericQ, s_?NumericQ, qq_?NumericQ] :=
    ND[TSILI[xx, y, s, qq], xx, x];

TestClose[ND[DI1[x, yy, s, qq], yy, y],
          TSILIpp[x, y, s, qq], 1*^-9];

PrintHeadline["Testing TSILIp3"];

TestClose[ND[TSILI[xx, y, s, qq], {xx, 3}, x],
          TSILIp3[x, y, s, qq], 1*^-4];

PrintHeadline["Testing TSILM"];

TestClose[Mxyzuv /. results,
          TSILM[x, y, z, u, v, s]];

PrintHeadline["Testing TSILS"];

TestClose[Svyz /. results,
          TSILS[v, y, z, s, qq]];

TestClose[Suxv /. results,
          TSILS[u, x, v, s, qq]];

(* Eq.(2.21) *)
TestClose[TSILI[x, y, z, qq],
          TSILS[x, y, z, 0, qq]];

PrintHeadline["Testing TSILT"];

TestClose[Tvyz /. results,
          TSILT[v, y, z, s, qq]];

TestClose[Tuxv /. results,
          TSILT[u, x, v, s, qq]];

TestClose[Tyzv /. results,
          TSILT[y, z, v, s, qq]];

TestClose[Txuv /. results,
          TSILT[x, u, v, s, qq]];

TestClose[Tzyv /. results,
          TSILT[z, y, v, s, qq]];

TestClose[Tvxu /. results,
          TSILT[v, x, u, s, qq]];

(* Eq.(2.22) *)
TestClose[TSILT[x, y, z, s, qq],
          -ND[TSILS[xx, y, z, s, qq], xx, x],
          1*^-9];

PrintHeadline["Testing TSILTbar"];

TestClose[TBARvyz /. results,
          TSILTbar[v, y, z, s, qq]];

TestClose[TBARuxv /. results,
          TSILTbar[u, x, v, s, qq]];

TestClose[TBARyzv /. results,
          TSILTbar[y, z, v, s, qq]];

TestClose[TBARxuv /. results,
          TSILTbar[x, u, v, s, qq]];

TestClose[TBARzyv /. results,
          TSILTbar[z, y, v, s, qq]];

TestClose[TBARvxu /. results,
          TSILTbar[v, x, u, s, qq]];

(* Eq.(2.41) *)
TestClose[TSILTbar[x, y, z, s, qq],
          TSILT[x, y, z, s, qq] + B[x, y, s, qq] Log[x/qq]];

PrintHeadline["Testing TSILU"];

eps = 1*^-14;

TestClose[Uzxyv /. results,
          TSILU[z, x, y, v, s, qq], eps];

TestClose[Uuyxv /. results,
          TSILU[u, y, x, v, s, qq], eps];

TestClose[Uxzuv /. results,
          TSILU[x, z, u, v, s, qq], eps];

TestClose[Uyuzv /. results,
          TSILU[y, u, z, v, s, qq], eps];

PrintHeadline["Testing TSILV"];

eps = 1*^-14;

TestClose[Vzxyv /. results,
          TSILV[z, x, y, v, s, qq], eps];

TestClose[Vuyxv /. results,
          TSILV[u, y, x, v, s, qq], eps];

TestClose[Vxzuv /. results,
          TSILV[x, z, u, v, s, qq], eps];

TestClose[Vyuzv /. results,
          TSILV[y, u, z, v, s, qq], eps];

(* Eq.(2.26) *)
TestClose[TSILV[x, y, z, u, s, qq],
          -ND[TSILU[x, yy, z, u, s, qq], yy, y],
          1*^-10];

Print["Number of passed tests: ", passedTests];
Print["Number of failed tests: ", failedTests];

Quit[failedTests];
