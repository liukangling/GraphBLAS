//------------------------------------------------------------------------------
// GB_msort_3.h: definitions for GB_msort_3.c
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2019, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

// A parallel mergesort of an array of 3-by-n integers.  Each key consists
// of three integers.

#include "GB_sort.h"

//------------------------------------------------------------------------------
// prototypes only needed for GB_msort_3
//------------------------------------------------------------------------------

void GB_merge_parallel_3                // parallel merge
(
    int64_t *restrict S_0,              // output of length nbigger + nsmaller
    int64_t *restrict S_1,
    int64_t *restrict S_2,
    const int64_t *restrict Bigger_0,   // Bigger [0..nbigger-1]
    const int64_t *restrict Bigger_1,
    const int64_t *restrict Bigger_2,
    const int64_t nbigger,
    const int64_t *restrict Smaller_0,  // Smaller [0..nsmaller-1]
    const int64_t *restrict Smaller_1,
    const int64_t *restrict Smaller_2,
    const int64_t nsmaller
) ;

void GB_merge_select_3      // parallel or sequential merge of 3-by-n arrays
(
    int64_t *restrict S_0,              // output of length nleft+nright
    int64_t *restrict S_1,
    int64_t *restrict S_2,
    const int64_t *restrict Left_0,     // Left [0..nleft-1]
    const int64_t *restrict Left_1,
    const int64_t *restrict Left_2,
    const int64_t nleft,
    const int64_t *restrict Right_0,    // Right [0..nright-1]
    const int64_t *restrict Right_1,
    const int64_t *restrict Right_2,
    const int64_t nright
) ;

void GB_mergesort_3 // sort array A of size 3-by-n, using 3 keys (A [0:1][])
(
    int64_t *restrict A_0,      // size n array
    int64_t *restrict A_1,      // size n array
    int64_t *restrict A_2,      // size n array
    int64_t *restrict W_0,      // size n array, workspace
    int64_t *restrict W_1,      // size n array, workspace
    int64_t *restrict W_2,      // size n array, workspace
    const int64_t n
) ;

