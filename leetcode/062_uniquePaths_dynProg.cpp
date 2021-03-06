// -------------------------------------------------------------------------------------
//         Author: Sourabh S Joshi (cbrghostrider); Copyright - All rights reserved.
//                       For email, run on linux (perl v5.8.5):
//   perl -e 'print pack "H*","736f75726162682e732e6a6f73686940676d61696c2e636f6d0a"'
// -------------------------------------------------------------------------------------
//O(mn) time and O(mn) space. Space can be optimized to O(min(m, n))
//
class Solution {
public:
    int uniquePaths(int m, int n) {
        if (m <= 0 || n <= 0) return 0; //ill-defined problem
        
        //dynamic programming cache
        int cache[m][n];
        cache[0][0] = 1;
        
        //dynamic programming loop
        for (int r=0; r<m; r++) {
            for (int c=0; c<n; c++) {
             if (r == 0 &&  c == 0) continue;
             int pathsTop  = (r == 0) ? 0 : cache[r-1][c];
             int pathsLeft = (c == 0) ? 0 : cache[r][c-1];
             cache[r][c] = pathsTop + pathsLeft;
            }
        }
        return cache[m-1][n-1];
    }
};
