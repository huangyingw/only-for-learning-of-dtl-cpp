#ifndef DTL_VARIABLES_H
#define DTL_VARIABLES_H

#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <iostream>

namespace dtl {
    
    using std::vector;
    using std::string;
    using std::pair;
    using std::ostream;
    using std::list;
    using std::for_each;
    using std::distance;
    using std::fill;
    using std::cout;
    using std::endl;
    using std::rotate;
    using std::swap;
    using std::max;
    
    /**
     * type of edit for SES
     */
    typedef int edit_t;
    const   edit_t SES_DELETE = -1;
    const   edit_t SES_COMMON = 0;
    const   edit_t SES_ADD    = 1;
    
    /**
     * mark of SES
     */
    const string SES_MARK_DELETE = "-";
    const string SES_MARK_COMMON = " ";
    const string SES_MARK_ADD    = "+";
    
    /**
     * info for Unified Format
     */
    typedef struct eleminfo {
        long long beforeIdx;           // index of prev sequence
        long long afterIdx;            // index of after sequence
        edit_t    type;                // type of edit(Add, Delete, Common)
    } elemInfo;
    
    const long long DTL_SEPARATE_SIZE = 3;
    const long long DTL_CONTEXT_SIZE  = 3;
    
    /**
     * cordinate for registering route
     */
    typedef struct Point {
        long long x;                         // x cordinate
        long long y;                         // y cordinate
        long long k;                         // vertex
    } P;
    
    /**
     * limit of cordinate size
     */
    const unsigned long long MAX_CORDINATES_SIZE = 2000000;
    
    typedef vector< long long > editPath;
    typedef vector< P >         editPathCordinates;
    
    /**
     * Structure of Unified Format Hunk
     */
    template <typename sesElem>
    struct uniHunk {
        long long a, b, c, d;        // @@ -a,b +c,d @@
        vector< sesElem > common[2]; // anteroposterior commons on changes
        vector< sesElem > change;    // changes
        long long inc_dec_count;     // count of increace and decrease
    };
    
#define dtl_typedefs(elem, sequence)                                    \
    typedef pair< elem, elemInfo >            sesElem;                  \
    typedef vector< sesElem >                 sesElemVec;               \
    typedef vector< uniHunk< sesElem > >      uniHunkVec;               \
    typedef list< elem >                      elemList;                 \
    typedef vector< elem >                    elemVec;                  \
    typedef typename uniHunkVec::iterator     uniHunkVec_iter;          \
    typedef typename sesElemVec::iterator     sesElemVec_iter;          \
    typedef typename elemList::iterator       elemList_iter;            \
    typedef typename sequence::iterator       sequence_iter;            \
    typedef typename sequence::const_iterator sequence_const_iter;      \
    typedef typename elemVec::iterator        elemVec_iter;


} 

#endif // DTL_VARIABLES_H
