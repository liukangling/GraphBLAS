//------------------------------------------------------------------------------
// GB_unaryop:  hard-coded functions for each built-in unary operator
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2019, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

// If this file is in the Generated/ folder, do not edit it (auto-generated)

#include "GB.h"
#ifndef GBCOMPACT
#include "GB_unaryop__include.h"

// C=unop(A) is defined by the following types and operators:

// op(A)  function:  GB_unop__abs_uint32_fp32
// op(A') function:  GB_tran__abs_uint32_fp32

// C type:   uint32_t
// A type:   float
// cast:     uint32_t cij ; GB_CAST_UNSIGNED(cij,aij,32)
// unaryop:  cij = aij

#define GB_ATYPE \
    float

#define GB_CTYPE \
    uint32_t

// aij = Ax [pA]
#define GB_GETA(aij,Ax,pA)  \
    float aij = Ax [pA]

#define GB_CX(p) Cx [p]

// unary operator
#define GB_OP(z, x)   \
    z = x ;

// casting
#define GB_CASTING(z, x)   \
    uint32_t z ; GB_CAST_UNSIGNED(z,x,32) ;

// cij = op (cast (aij))
#define GB_CAST_OP(pC,pA)           \
{                                   \
    /* aij = Ax [pA] */             \
    GB_GETA (aij, Ax, pA) ;         \
    /* Cx [pC] = op (cast (aij)) */ \
    GB_CASTING (x, aij) ;           \
    GB_OP (GB_CX (pC), x) ;         \
}

//------------------------------------------------------------------------------
// Cx = op (cast (Ax)): apply a unary operator
//------------------------------------------------------------------------------

void GB_unop__abs_uint32_fp32
(
    uint32_t *restrict Cx,
    float *restrict Ax,
    int64_t anz,
    int nthreads
)
{ 
    #pragma omp parallel for num_threads(nthreads)
    for (int64_t p = 0 ; p < anz ; p++)
    {
        GB_CAST_OP (p, p) ;
    }
}

//------------------------------------------------------------------------------
// C = op (cast (A')): transpose, typecast, and apply a unary operator
//------------------------------------------------------------------------------

void GB_tran__abs_uint32_fp32
(
    GrB_Matrix C,
    const GrB_Matrix A,
    int64_t **Rowcounts,
    GBI_single_iterator Iter,
    const int64_t *restrict A_slice,
    int naslice
)
{ 
    #define GB_PHASE_2_OF_2
    #include "GB_unaryop_transpose.c"
}

#endif

