function C = cot (G)
%COT cotangent.
% C = cot (G) computes the cotangent of each entry of a GraphBLAS matrix G.
% Since cot (0) is nonzero, C is a full matrix.
%
% See also GrB/coth, GrB/acot, GrB/acoth.

% SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights
% Reserved. http://suitesparse.com.  See GraphBLAS/Doc/License.txt.

C = GrB.apply ('minv', full (tan (G))) ;
