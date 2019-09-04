function gbtest38
%GBTEST38 test sqrt, eps, ceil, floor, round, fix, real, conj, ...
% isfinite, isinf, isnan, spfun, eig

% SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2019, All Rights Reserved.
% http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

rng ('default') ;

for trial = 1:40

    A = 1e3 * rand (3) ;
    B = single (A) ;

    G = gb (A) ;
    H = gb (B) ;

    err = norm (sqrt (A) - sqrt (G), 1) ; assert (err < 8 * eps ('double')) ;
    err = norm (sqrt (B) - sqrt (H), 1) ; assert (err < 8 * eps ('single')) ;

    assert (gbtest_eq (eps (A), eps (G))) ;
    assert (gbtest_eq (eps (B), eps (H))) ;

    assert (gbtest_eq (ceil (A), ceil (G))) ;
    assert (gbtest_eq (ceil (B), ceil (H))) ;

    assert (gbtest_eq (floor (A), floor (G))) ;
    assert (gbtest_eq (floor (B), floor (H))) ;

    assert (gbtest_eq (round (A), round (G))) ;
    assert (gbtest_eq (round (B), round (H))) ;

    assert (gbtest_eq (fix (A), fix (G))) ;
    assert (gbtest_eq (fix (B), fix (H))) ;

    assert (gbtest_eq (real (A), real (G))) ;
    assert (gbtest_eq (real (B), real (H))) ;

    assert (gbtest_eq (conj (A), conj (G))) ;
    assert (gbtest_eq (conj (B), conj (H))) ;

    C = A ;
    C (1,1) = inf ;
    C (2,2) = nan ;
    G = gb (C) ;

    assert (gbtest_eq (isfinite (C), isfinite (G))) ;
    assert (gbtest_eq (isnan    (C), isnan    (G))) ;

    A = sprand (10, 10, 0.5) ;
    G = gb (A) ;
    assert (gbtest_eq (spfun (@exp, A), double (spfun (@exp, G)))) ;

    A = rand (10) ;
    G = gb (A) ;
    assert (isequal (eig (A), double (eig (G)))) ;

end

fprintf ('gbtest38: all tests passed\n') ;