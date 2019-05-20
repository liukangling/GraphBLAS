//------------------------------------------------------------------------------
// GB_reduce_each_index: T(i)=reduce(A(i,:)), reduce a matrix to a vector
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2019, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

// Reduce a matrix to a vector.  All entries in A(i,:) are reduced to T(i).
// First, all threads reduce their slice to their own workspace, operating on
// roughly the same number of entries each.  The vectors in A are ignored; the
// reduction only depends on the indices.  Next, the threads cooperate to
// reduce all workspaces to the workspace of thread 0.  Finally, this last
// workspace is collected into T.

// PARALLEL: done

{

    //--------------------------------------------------------------------------
    // get A
    //--------------------------------------------------------------------------

    const GB_ATYPE *restrict Ax = A->x ;
    const int64_t  *restrict Ai = A->i ;
    const int64_t n = A->vlen ;
    size_t zsize = ttype->size ;

    //--------------------------------------------------------------------------
    // reduce each slice in its own workspace
    //--------------------------------------------------------------------------

    GB_CTYPE **Works [nth] ;
    bool     **Marks [nth] ;
    int64_t  Tnz [nth] ;
    bool ok = true ;

    // each thread reduces its own slice in parallel
    #pragma omp parallel for num_threads(nth) schedule(static) reduction(&&:ok)
    for (int tid = 0 ; tid < nth ; tid++)
    {

        //----------------------------------------------------------------------
        // allocate workspace for this thread
        //----------------------------------------------------------------------

        GB_CTYPE *restrict Work ;
        bool     *restrict Mark ;
        GB_MALLOC_MEMORY (Work, n, zsize) ;
        GB_CALLOC_MEMORY (Mark, n, sizeof (bool)) ;
        Works [tid] = Work ;
        Marks [tid] = Mark ;
        bool my_ok = (Mark != NULL && Work != NULL) ;
        ok = ok && my_ok ;
        int64_t my_tnz = 0 ;

        //----------------------------------------------------------------------
        // reduce the entries
        //----------------------------------------------------------------------

        if (my_ok)
        {
            for (int64_t p = pstart_slice [tid] ; p < pstart_slice [tid+1] ;p++)
            {
                int64_t i = Ai [p] ;
                // ztype aij = (ztype) Ax [p], with typecast
                GB_CAST_ARRAY_TO_SCALAR (aij, Ax, p) ;
                if (!Mark [i])
                {
                    // first time index i has been seen
                    // Work [i] = aij ; no typecast
                    GB_COPY_SCALAR_TO_ARRAY (Work, i, aij) ;
                    Mark [i] = true ;
                    my_tnz++ ;
                }
                else
                {
                    // Work [i] += aij ; no typecast
                    GB_ADD_SCALAR_TO_ARRAY (Work, i, aij) ;
                }
            }
            Tnz [tid] = my_tnz ;
        }
    }

    //--------------------------------------------------------------------------
    // handle out-of-memory condition
    //--------------------------------------------------------------------------

    if (!ok)
    {
        // out of memory
        for (int tid = 0 ; tid < nth ; tid++)
        {
            GB_FREE_MEMORY (Works [tid], n, zsize) ;
            GB_FREE_MEMORY (Marks [tid], n, sizeof (bool)) ;
        }
        return (GB_OUT_OF_MEMORY) ;
    }

    //--------------------------------------------------------------------------
    // reduce all workspace to Work [0] and count # entries in T
    //--------------------------------------------------------------------------

    GB_CTYPE *restrict Work0 = Works [0] ;
    bool     *restrict Mark0 = Marks [0] ;
    int64_t tnz = Tnz [0] ;

    if (nth > 1)
    {
        #pragma omp parallel for num_threads(nthreads) schedule(static) \
            reduction(+:tnz)
        for (int64_t i = 0 ; i < n ; i++)
        {
            for (int tid = 1 ; tid < nth ; tid++)
            {
                const bool *restrict Mark = Marks [tid] ;
                if (Mark [i])
                {
                    // thread tid has a contribution to index i
                    const GB_CTYPE *restrict Work = Works [tid] ;
                    if (!Mark0 [i])
                    {
                        // first time index i has been seen
                        // Work0 [i] = Work [i] ; no typecast
                        GB_COPY_ARRAY_TO_ARRAY (Work0, i, Work, i) ;
                        Mark0 [i] = true ;
                        tnz++ ;
                    }
                    else
                    {
                        // Work0 [i] += Work [i] ; no typecast
                        GB_ADD_ARRAY_TO_ARRAY (Work0, i, Work, i) ;
                    }
                }
            }
        }

        // free all but workspace for thread 0
        for (int tid = 1 ; tid < nth ; tid++)
        {
            GB_FREE_MEMORY (Works [tid], n, zsize) ;
            GB_FREE_MEMORY (Marks [tid], n, sizeof (bool)) ;
        }
    }

    //--------------------------------------------------------------------------
    // allocate T
    //--------------------------------------------------------------------------

    // since T is a GrB_Vector, it is CSC and not hypersparse
    GB_CREATE (&T, ttype, n, 1, GB_Ap_calloc, true,
        GB_FORCE_NONHYPER, GB_HYPER_DEFAULT, 1, tnz, true, Context) ;
    if (info != GrB_SUCCESS)
    { 
        // out of memory
        GB_FREE_MEMORY (Works [0], n, zsize) ;
        GB_FREE_MEMORY (Marks [0], n, sizeof (bool)) ;
        return (GB_OUT_OF_MEMORY) ;
    }

    T->p [0] = 0 ;
    T->p [1] = tnz ;
    int64_t  *restrict Ti = T->i ;
    GB_CTYPE *restrict Tx = T->x ;
    T->nvec_nonempty = (tnz > 0) ? 1 : 0 ;

    //--------------------------------------------------------------------------
    // gather the results into T
    //--------------------------------------------------------------------------

    if (tnz == n)
    {
        // T is dense: transplant Work0 into T->x
        #pragma omp parallel for num_threads(nthreads) schedule(static)
        for (int64_t i = 0 ; i < n ; i++)
        { 
            Ti [i] = i ;
        }
        GB_FREE_MEMORY (T->x, n, zsize) ;
        T->x = Work0 ;
        Work0 = NULL ;
    }
    else
    {
        // T is sparse: gather from Work0 and Mark0
        // FUTURE: this is not yet parallel
        int64_t p = 0 ;
        for (int64_t i = 0 ; i < n ; i++)
        {
            if (Mark0 [i])
            { 
                Ti [p] = i ;
                // Tx [p] = Work0 [i], no typecast
                GB_COPY_ARRAY_TO_ARRAY (Tx, p, Work0, i) ;
                p++ ;
            }
        }
        ASSERT (p == tnz) ;
    }

    // free workspace for thread 0 
    GB_FREE_MEMORY (Work0, n, zsize) ;
    GB_FREE_MEMORY (Mark0, n, sizeof (bool)) ;
}
