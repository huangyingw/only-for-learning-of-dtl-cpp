namespace dtl {
    
    /**
     * diff class template
     * sequence must support random_access_iterator.
     */
    template <typename elem, typename sequence = vector< elem >, typename comparator = Compare< elem > >
    class Diff
    {
    private :
        dtl_typedefs(elem, sequence)
        sequence           A;
        sequence           B;
        size_t             M;
        size_t             N;
        size_t             delta;
        size_t             offset;
        long long          *fp;
        long long          editDistance;
        Lcs< elem >        lcs;
        Ses< elem >        ses;
        editPath           path;
        editPathCordinates pathCordinates;
        bool               reverse;
        bool               huge;
        bool               unserious;
        bool               onlyEditDistance;
        uniHunkVec         uniHunks;
        comparator         cmp;
    public :
        Diff () {}
        
        Diff (const sequence& a, 
              const sequence& b) : A(a), B(b) {
            init();
        }
        
        Diff (const sequence& a, 
              const sequence& b, 
              const comparator& comp) : A(a), B(b), cmp(comp) {
            init();
        }
        
        ~Diff() {}
        
        long long getEditDistance () const {
            return editDistance;
        }
        
        Lcs< elem > getLcs () const {
            return lcs;
        }
        
        elemVec getLcsVec () const {
            return lcs.getSequence();
        }
        
        Ses< elem > getSes () const {
            return ses;
        }
        
        uniHunkVec getUniHunks () const {
            return uniHunks;
        }
        
        bool isHuge () const {
            return huge;
        }
        
        void onHuge () {
            this->huge = true;
        }
        
        void offHuge () {
            this->huge = false;
        }
        
        bool isUnserious () const {
            return unserious;
        }
        
        void onUnserious () {
            this->unserious = true;
        }
        
        void offUnserious () {
            this->unserious = false;
        }
        
        void onOnlyEditDistance () {
            this->onlyEditDistance = true;
        }
        
        /**
         * patching with Unified Format Hunks
         */
        sequence uniPatch (const sequence& seq) {
            elemList        seqLst(seq.begin(), seq.end());
            sesElemVec      shunk;
            sesElemVec_iter vsesIt;
            elemList_iter   lstIt         = seqLst.begin();
            long long       inc_dec_total = 0;
            long long       gap           = 1;
            for (uniHunkVec_iter it=uniHunks.begin();it!=uniHunks.end();++it) {
                joinSesVec(shunk, it->common[0]);
                joinSesVec(shunk, it->change);
                joinSesVec(shunk, it->common[1]);
                it->a         += inc_dec_total;
                inc_dec_total += it->inc_dec_count;
                for (long long i=0;i<it->a - gap;++i) {
                    ++lstIt;
                }
                gap = it->a + it->b + it->inc_dec_count;
                vsesIt = shunk.begin();
                while (vsesIt!=shunk.end()) {
                    switch (vsesIt->second.type) {
                    case SES_ADD :
                        seqLst.insert(lstIt, vsesIt->first);
                        break;
                    case SES_DELETE :
                        if (lstIt != seqLst.end()) {
                            lstIt = seqLst.erase(lstIt);
                        }
                        break;
                    case SES_COMMON :
                        if (lstIt != seqLst.end()) {
                            ++lstIt;
                        }
                        break;
                    default :
                        // no through
                        break;
                    }
                    ++vsesIt;
                }
                shunk.clear();
            }
            
            sequence patchedSeq(seqLst.begin(), seqLst.end());
            return patchedSeq;
        }
        
        /**
         * patching with Shortest Edit Script
         */
        sequence patch (const sequence& seq) const {
            sesElemVec    sesSeq = ses.getSequence();
            elemList      seqLst(seq.begin(), seq.end());
            elemList_iter lstIt  = seqLst.begin();
            for (sesElemVec_iter sesIt=sesSeq.begin();sesIt!=sesSeq.end();++sesIt) {
                switch (sesIt->second.type) {
                case SES_ADD :
                    seqLst.insert(lstIt, sesIt->first);
                    break;
                case SES_DELETE :
                    lstIt = seqLst.erase(lstIt);
                    break;
                case SES_COMMON :
                    ++lstIt;
                    break;
                default :
                    // no through
                    break;
                }
            }
            sequence patchedSeq(seqLst.begin(), seqLst.end());
            return patchedSeq;
        }
        
        /**
         * compose Longest Common Subsequence and Shortest Edit Script.
         * The algorithm implemented here is based on "An O(NP) Sequence Comparison Algorithm"
         * described by Sun Wu, Udi Manber and Gene Myers
         */
        void compose() {
            
            if (isHuge()) pathCordinates.reserve(MAX_CORDINATES_SIZE + 50000);
            
            long long p = -1;
            fp = new long long[M + N + 3];
            fill(&fp[0], &fp[M + N + 3], -1);
            path = editPath(M + N + 3);
            fill(path.begin(), path.end(), -1);
        ONP:
            do {
                ++p;
                for (long long k=-p;k<=static_cast<long long>(delta)-1;++k) {
                    fp[k+offset] = snake(k, fp[k-1+offset]+1, fp[k+1+offset]);
                }
                for (long long k=static_cast<long long>(delta)+p;k>=static_cast<long long>(delta)+1;--k) {
                    fp[k+offset] = snake(k, fp[k-1+offset]+1, fp[k+1+offset]);
                }
                fp[delta+offset] = snake(static_cast<long long>(delta), fp[delta-1+offset]+1, fp[delta+1+offset]);
            } while (fp[delta+offset] != static_cast<long long>(N) && pathCordinates.size() < MAX_CORDINATES_SIZE);
            
            editDistance += static_cast<long long>(delta) + 2 * p;
            long long r = path[delta+offset];
            P cordinate;
            editPathCordinates epc(0);
            
            // only recoding editdistance
            if (onlyEditDistance) {
                delete[] this->fp;
                return;
            }
            
            while(r != -1){
                cordinate.x = pathCordinates[(size_t)r].x;
                cordinate.y = pathCordinates[(size_t)r].y;
                epc.push_back(cordinate);
                r = pathCordinates[(size_t)r].k;
            }
            
            // record Longest Common Subsequence & Shortest Edit Script
            if (!recordSequence(epc)) {
                pathCordinates.resize(0);
                epc.resize(0);
                p = -1;
                goto ONP;
            }
            delete[] this->fp;
        }
        
        /**
         * print difference between A and B with SES
         */
        void printSES (ostream& out = cout) const {
            sesElemVec ses_v = ses.getSequence();
            for_each(ses_v.begin(), ses_v.end(), ChangePrinter< sesElem >(out));
        }
        
        /**
         * print difference with given SES
         */
        static void printSES (const Ses< elem >& s, ostream& out = cout) {
            sesElemVec ses_v = s.getSequence();
            for_each(ses_v.begin(), ses_v.end(), ChangePrinter< sesElem >(out));
        }
        
        /**
         * print difference between A and B with the format such as Unified Format
         */
        void printUnifiedFormat (ostream& out = cout) const {
            for_each(uniHunks.begin(), uniHunks.end(), UniHunkPrinter< sesElem >(out));
        }
        
        /**
         * print unified format difference with given unified format hunks
         */
        static void printUnifiedFormat (const uniHunkVec& hunks, ostream& out = cout) {
            for_each(hunks.begin(), hunks.end(), UniHunkPrinter< sesElem >(out));
        }

        /**
         * compose Unified Format Hunks from Shortest Edit Script
         */
        void composeUnifiedHunks () {
            sesElemVec       common[2];
            sesElemVec       change;
            sesElemVec       ses_v  = ses.getSequence();
            long long        l_cnt  = 1;
            long long        length = distance(ses_v.begin(), ses_v.end());
            long long        middle = 0;
            bool             isMiddle, isAfter;
            elem             e;
            elemInfo         einfo;
            long long        a, b, c, d;        // @@ -a,b +c,d @@
            long long        inc_dec_count = 0;
            uniHunk<sesElem> hunk;
            sesElemVec       adds;
            sesElemVec       deletes;
            
            isMiddle = isAfter = false;
            a = b = c = d = 0;
            
            for (sesElemVec_iter it=ses_v.begin();it!=ses_v.end();++it, ++l_cnt) {
                e = it->first;
                einfo = it->second;
                switch (einfo.type) {
                case SES_ADD :
                    middle = 0;
                    ++inc_dec_count;
                    adds.push_back(*it);
                    if (!isMiddle)       isMiddle = true;
                    if (isMiddle)        ++d;
                    if (l_cnt >= length) {
                        joinSesVec(change, deletes);
                        joinSesVec(change, adds);
                        isAfter = true;
                    }
                    break;
                case SES_DELETE :
                    middle = 0;
                    --inc_dec_count;
                    deletes.push_back(*it);
                    if (!isMiddle)       isMiddle = true;
                    if (isMiddle)        ++b;
                    if (l_cnt >= length) {
                        joinSesVec(change, deletes);
                        joinSesVec(change, adds);
                        isAfter = true;
                    }
                    break;
                case SES_COMMON :
                    ++b;++d;
                    if (common[1].empty() && adds.empty() && deletes.empty() && change.empty()) {
                        if (static_cast<long long>(common[0].size()) < DTL_CONTEXT_SIZE) {
                            if (a == 0 && c == 0) {
                                a = einfo.beforeIdx;
                                c = einfo.afterIdx;
                            }
                            common[0].push_back(*it);
                        } else {
                            rotate(common[0].begin(), common[0].begin() + 1, common[0].end());
                            common[0].pop_back();
                            common[0].push_back(*it);
                            ++a;++c;
                            --b;--d;
                        }
                    }
                    if (isMiddle && !isAfter) {
                        ++middle;
                        joinSesVec(change, deletes);
                        joinSesVec(change, adds);
                        change.push_back(*it);
                        if (middle >= DTL_SEPARATE_SIZE || l_cnt >= length) {
                            isAfter = true;
                        }
                        adds.clear();
                        deletes.clear();
                    }
                    break;
                default :
                    // no through
                    break;
                }
                // compose unified format hunk
                if (isAfter && !change.empty()) {
                    sesElemVec_iter cit = it;
                    long long       cnt = 0;
                    for (long long i=0;i<DTL_SEPARATE_SIZE && (cit != ses_v.end());++i, ++cit) {
                        if (cit->second.type == SES_COMMON) {
                            ++cnt;
                        }
                    }
                    if (cnt < DTL_SEPARATE_SIZE && l_cnt < length) {
                        middle = 0;
                        isAfter = false;
                        continue;
                    }
                    if (static_cast<long long>(common[0].size()) >= DTL_SEPARATE_SIZE) {
                        long long c0size = static_cast<long long>(common[0].size());
                        rotate(common[0].begin(), 
                               common[0].begin() + (size_t)c0size - DTL_SEPARATE_SIZE, 
                               common[0].end());
                        for (long long i=0;i<c0size - DTL_SEPARATE_SIZE;++i) {
                            common[0].pop_back();
                        }
                        a += c0size - DTL_SEPARATE_SIZE;
                        c += c0size - DTL_SEPARATE_SIZE;
                    }
                    if (a == 0) ++a;
                    if (c == 0) ++c;
                    if (isReverse()) swap(a, c);
                    hunk.a = a;hunk.b = b;hunk.c = c;hunk.d = d;
                    hunk.common[0] = common[0];
                    hunk.change = change;
                    hunk.common[1] = common[1];
                    hunk.inc_dec_count = inc_dec_count;
                    uniHunks.push_back(hunk);
                    isMiddle = false;
                    isAfter = false;
                    common[0].clear();
                    common[1].clear();
                    adds.clear();
                    deletes.clear();
                    change.clear();
                    a = b = c = d = middle = inc_dec_count = 0;
                }
            }
        }
    private :
        /**
         * initialize
         */
        void init () {
            M = distance(A.begin(), A.end());
            N = distance(B.begin(), B.end());
            if (M < N) {
                reverse = false;
            } else {
                swap(A, B);
                swap(M, N);
                reverse = true;
            }
            editDistance     = 0;
            delta            = N - M;
            offset           = M + 1;
            huge             = false;
            unserious        = false;
            onlyEditDistance = false;
            fp               = NULL;
        }
        
        /**
         * search shortest path and record the path
         */
        long long snake(const long long& k, const long long& above, const long long& below) {
            long long r = above > below ? path[(size_t)k-1+offset] : path[(size_t)k+1+offset];
            long long y = max(above, below);
            long long x = y - k;
            while ((size_t)x < M && (size_t)y < N && cmp.impl(A[(size_t)x], B[(size_t)y])) {
                ++x;++y;
            }
            
            path[(size_t)k+offset] = static_cast<long long>(pathCordinates.size());
            if (!onlyEditDistance) {
                P p;
                p.x = x;p.y = y;p.k = r;
                pathCordinates.push_back(p);      
            }
            return y;
        }
        
        /**
         * record SES and LCS
         */
        bool recordSequence (const editPathCordinates& v) {
            sequence_const_iter x(A.begin());
            sequence_const_iter y(B.begin());
            long long           x_idx,  y_idx;  // line number for Unified Format
            long long           px_idx, py_idx; // cordinates
            bool                complete = false;
            x_idx  = y_idx  = 1;
            px_idx = py_idx = 0;
            for (size_t i=v.size()-1;!complete;--i) {
                while(px_idx < v[i].x || py_idx < v[i].y) {
                    if (v[i].y - v[i].x > py_idx - px_idx) {
                        if (!isReverse()) {
                            ses.addSequence(*y, y_idx, 0, SES_ADD);
                        } else {
                            ses.addSequence(*y, y_idx, 0, SES_DELETE);
                        }
                        ++y;
                        ++y_idx;
                        ++py_idx;
                    } else if (v[i].y - v[i].x < py_idx - px_idx) {
                        if (!isReverse()) {
                            ses.addSequence(*x, x_idx, 0, SES_DELETE);
                        } else {
                            ses.addSequence(*x, x_idx, 0, SES_ADD);
                        }
                        ++x;
                        ++x_idx;
                        ++px_idx;
                    } else {
                        lcs.addSequence(*x);
                        ses.addSequence(*x, x_idx, y_idx, SES_COMMON);
                        ++x;
                        ++y;
                        ++x_idx;
                        ++y_idx;
                        ++px_idx;
                        ++py_idx;
                    }
                }
                if (i == 0) complete = true;
            }
            
            if (x_idx > static_cast<long long>(M) && y_idx > static_cast<long long>(N)) {
                // all recording success
            } else {
                // unserious difference
                if (isUnserious()) {
                    if (!isReverse()) {
                        recordOddSequence(x_idx, M, x, SES_DELETE);
                        recordOddSequence(y_idx, N, y, SES_ADD);
                    } else {
                        recordOddSequence(x_idx, M, x, SES_ADD);
                        recordOddSequence(y_idx, N, y, SES_DELETE);
                    }
                    return true;
                }
                
                // decent difference
                sequence A_(A.begin() + (size_t)x_idx - 1, A.end());
                sequence B_(B.begin() + (size_t)y_idx - 1, B.end());
                A        = A_;
                B        = B_;
                M        = distance(A.begin(), A.end());
                N        = distance(B.begin(), B.end());
                delta    = N - M;
                offset   = M + 1;
                delete[] fp;
                fp = new long long[M + N + 3];
                fill(&fp[0], &fp[M + N + 3], -1);
                fill(path.begin(), path.end(), -1);
                return false;
            }
            return true;
        }
        
        /**
         * record odd sequence to ses
         */
        void inline recordOddSequence (long long idx, long long length, sequence_const_iter it, const edit_t et) {
            while(idx < length){
                ses.addSequence(*it, idx, 0, et);
                ++it;
                ++idx;
                ++editDistance;
            }
            ses.addSequence(*it, idx, 0, et);
            ++editDistance;
        }
        
        /**
         * join ses vectors
         */
        void inline joinSesVec (sesElemVec& s1, sesElemVec& s2) const {
            if (!s2.empty()) {
                for (sesElemVec_iter vit=s2.begin();vit!=s2.end();++vit) {
                    s1.push_back(*vit);
                }
            }      
        }
        
        /**
         * check sequence is replaced each other
         */
        bool inline isReverse () const {
            return reverse;
        }
    };
}

#endif // DTL_DIFF_H
