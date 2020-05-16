function C = GB_spec_kron (C, Mask, accum, mult, A, B, descriptor)
%GB_SPEC_KRON a MATLAB mimic of GxB_kron
%
% Usage:
% C = GB_spec_kron (C, Mask, accum, mult, A, B, descriptor)
%
% Computes C<Mask> = accum(C,T), in GraphBLAS notation, where T = kron(A,B),
% kron(A',B), kron(A,B') or kron(A',B')

% SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights Reserved.
% http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

%-------------------------------------------------------------------------------
% get inputs
%-------------------------------------------------------------------------------

if (nargout > 1 || nargin ~= 7)
    error ('usage: C = GB_spec_kron (C, Mask, accum, mult, A, B, descriptor)') ;
end

C = GB_spec_matrix (C) ;
A = GB_spec_matrix (A) ;
B = GB_spec_matrix (B) ;
[mult_op xyclass ztype xtype ytype] = GB_spec_operator (mult, C.class) ;
[C_replace Mask_comp Atrans Btrans Mask_struct] = ...
    GB_spec_descriptor (descriptor) ;
Mask = GB_spec_getmask (Mask, Mask_struct) ;

%-------------------------------------------------------------------------------
% do the work via a clean MATLAB interpretation of the entire GraphBLAS spec
%-------------------------------------------------------------------------------

% apply the descriptor to A
if (Atrans)
    A.matrix = A.matrix' ;
    A.pattern = A.pattern' ;
end

% apply the descriptor to B
if (Btrans)
    B.matrix = B.matrix' ;
    B.pattern = B.pattern' ;
end

% T = A.*B, with typecasting
[anrows, ancols] = size (A.matrix) ;
[bnrows, bncols] = size (B.matrix) ;
cnrows = anrows * bnrows ;
cncols = ancols * bncols ;

% first cast the inputs into the x,y types of the operator
A1 = GB_mex_cast (A.matrix, xtype) ;
B1 = GB_mex_cast (B.matrix, ytype) ;

% do the values
T.matrix  = GB_spec_zeros ([cnrows cncols], ztype) ;
T.pattern = false (cnrows, cncols) ;
S = GB_spec_zeros ([bnrows bncols], xtype) ;
for j = 1:ancols
    for i = 1:anrows
        if A.pattern (i,j)
            S (:,:) = A1 (i,j) ;
            ci = (i-1) * bnrows + 1 ;
            cj = (j-1) * bncols + 1 ;
            p = B.pattern ;
            K = GB_spec_op (mult, S(p), B1(p)) ;
            Tblock = GB_spec_zeros ([bnrows bncols], ztype) ;
            Tblock (p) = K ;
            T.matrix  (ci:ci+bnrows-1, cj:cj+bncols-1) = Tblock ;
            T.pattern (ci:ci+bnrows-1, cj:cj+bncols-1) = B.pattern ;
        end
    end
end

assert (isequal (ztype, GB_spec_type (T.matrix))) ;
T.class = ztype ;

% C<Mask> = accum (C,T): apply the accum, then Mask, and return the result
C = GB_spec_accum_mask (C, Mask, accum, T, C_replace, Mask_comp, 0) ;


