// -------------------------------------------------------------------------------------
//         Author: Sourabh S Joshi (cbrghostrider); Copyright - All rights reserved.
//                       For email, run on linux (perl v5.8.5):
//   perl -e 'print pack "H*","736f75726162682e732e6a6f73686940676d61696c2e636f6d0a"'
// -------------------------------------------------------------------------------------
// Functionally correct version, using independence of dimensions
// also using divide and conquer for the sets of dimensions
// 
// Credit: Part of the credit for this final version goes to Peter Shor from MIT, 
// who's suggestions on stackexchange helped me optimize the algorithm!
//
#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <assert.h>
#include <unordered_map>

using namespace std;
typedef unsigned int Ways;
#define MODNUM (1000000007)

/////////////////////////////////////////////////////////////////////////////////////////

// pascal's triangle to compute (n `choose` k) ; use dynamic programming for this as well!!
// PROGRAMMER BEWARE!! n choose k will overflow for example for 300 choose 150
// therefore I am only storing a modified pascals' triangle, where each value is mod MODNUM
class PascalsTriangle {
private:
    vector<vector<unsigned long long>> pt;
public:
    PascalsTriangle(int n);
    unsigned long long choose(int n, int k);
};

PascalsTriangle::PascalsTriangle(int n) 
: pt(vector<vector<unsigned long long>>(n+1, vector<unsigned long long>(n+1, 1))) { //init [n+1][n+1] matrix to 1s
    for (int row=2; row<=n; row++) {
        for (int col=1; col<row; col++) {
            pt[row][col] = pt[row-1][col-1] + pt[row-1][col];
            pt[row][col] %= (unsigned long long) MODNUM;
        }
    }
}

unsigned long long PascalsTriangle::choose(int n, int k) {
    if (n <= k || k == 0) return 1;
    return pt[n][k];
}

/////////////////////////////////////////////////////////////////////////////////////////

// runs dynamic programming algorithm to compute ways to take x steps
// in each dimension *independent* of any other dimension
class DimCache {
private:
    const int start;
    vector<vector<Ways>> ways; //how many ways to take K steps in this dimension from each point
public:
    DimCache(int st, int dimsize, int maxSteps); //starting from this location in the dimension
    Ways waysForSteps(int steps); //how many ways to walk in this dim for these many steps
};

//can take [0, maxSteps] steps, and at start pos [0, dimsize-1]
DimCache::DimCache(int st, int dimsize, int maxSteps) 
:start(st), ways(maxSteps+1, vector<Ways>(dimsize, 1)) {
    for (int s=1; s<=maxSteps; s++) {   //over all steps possible
        for (int d=0; d<dimsize; d++) { //for each location in this dim
            Ways totways = 0;
            if (d != 0) {         //if not currently lowest index
                totways += ways[s-1][d-1];
                totways %= ((Ways) MODNUM);
            }
            if (d != dimsize-1) { //if not currently highest index
                totways += ways[s-1][d+1];
                totways %= ((Ways) MODNUM);
            }
            ways[s][d] = totways;
        }
    }
}

Ways DimCache::waysForSteps(int steps) {
    return (ways[steps][start]);
}

/////////////////////////////////////////////////////////////////////////////////////////

class DAndCCache {
private:
    unordered_map<unsigned int, Ways> cache; 

    unsigned int makeKey(int fromind, int toind, int steps);
public:
    DAndCCache();
    bool exists(int fromind, int toind, int steps);
    Ways lookup(int fromind, int toind, int steps);
    void record(int fromind, int toind, int steps, Ways val);
};

DAndCCache::DAndCCache()
: cache() {
}

//uses domain knowledge fromind, toind <- [0..9], and steps <- [1..300]
//indices each take 4 bits, and steps take 9 bits to store
unsigned int DAndCCache::makeKey(int fromind, int toind, int steps) {
    unsigned int key = 0;
    key |= (fromind & 0x0F); key <<= 4;
    key |= (toind & 0x0F); key <<= 9;
    key |= (steps & 0x1FF);
    return key;
}

bool DAndCCache::exists(int fromind, int toind, int steps) {
    return (cache.find(makeKey(fromind, toind, steps)) != cache.end());
}

Ways DAndCCache::lookup(int fromind, int toind, int steps) {
    auto it = cache.find(makeKey(fromind, toind, steps));
    if (it == cache.end()) return 0;
    return it->second;
}

void DAndCCache::record(int fromind, int toind, int steps, Ways val) {
    unsigned int key = makeKey(fromind, toind, steps);
    cache.insert(make_pair(key, val));
}

/////////////////////////////////////////////////////////////////////////////////////////

class HyperCuboid {
private:
    const int nsteps; //the number of steps to be taken
    vector<int> spos; //the starting position in the hypercube
    vector<int> dims; //the dimensions of the hypercube
    vector<DimCache> dimCache; //caches for each of the dimensions, each dim evaluated independently
    DAndCCache dnccache;   //caches the divide and conquer values
    PascalsTriangle pt;

    Ways computeWays(int fromdim, int todim, int remsteps);
public:
    HyperCuboid(int steps, const vector<int>& start, const vector<int>& dims);
    Ways computeWays();
};

HyperCuboid::HyperCuboid(int steps, const vector<int>& start, const vector<int>& d)
: nsteps(steps), spos(start), dims(d), dimCache(), dnccache(), pt(nsteps) {
    for (int i=0; i<dims.size(); i++) {
        dimCache.push_back(DimCache(spos[i], dims[i], nsteps));
    }
}

Ways HyperCuboid::computeWays(int fromdim, int todim, int remsteps) {
    if (remsteps <= 0) {    //base case, can't move anymore
        return 1;   
    }
    if (fromdim == todim) { //base case, no more dims left to d&c on
        return dimCache[fromdim].waysForSteps(remsteps);
    }
    if (dnccache.exists(fromdim, todim, remsteps)) { //top-down memoization, lookup value if exists
        return dnccache.lookup(fromdim, todim, remsteps);
    }

    int mid = (todim + fromdim) / 2;
    int midplus = mid+1;

    Ways totways = 0;
    for (int s=0; s<=remsteps; s++) {
        //divide and conquer!
        Ways lhsways = computeWays(fromdim, mid, s);
        Ways rhsways = computeWays(midplus, todim, remsteps-s);\

        //PROGRAMMER BEWARE: multiplication of 2 32-bit numbers can be 64-bit!
        unsigned long long lways = (unsigned long long)lhsways * (unsigned long long)rhsways;
        lways %= (unsigned long long) MODNUM;

        // of course, I could choose any sequence of interleaved (s) and (remsteps-s) ways
        // i.e. choose (remsteps `choose` s) ways to walk s steps in lhs dimensions
        // and remaining (remsteps-s) walks in rhs dimensions
        unsigned long long choices = pt.choose(remsteps, s);
        lways *= choices;
        lways %= (unsigned long long) MODNUM;

        totways += (Ways) lways;
        totways %= ((Ways) MODNUM);
    }
    dnccache.record(fromdim, todim, remsteps, totways);
    return totways;
}


Ways HyperCuboid::computeWays() {
    Ways ways = computeWays(0, dims.size()-1, nsteps);
    ways = ways % ((Ways) MODNUM);
    return ways;
}

/////////////////////////////////////////////////////////////////////////////////////////

int main() {
    int tc; std::cin >> tc;
    for (int t=0; t<tc; t++) {
        int n, m;
        std::cin >> n >> m;
        vector<int> dims, pos;
        for (int i=0; i<n; i++)  {
            int p; std::cin >> p;
            pos.push_back(p-1); //make this 0-indexed right here
        }
        for (int i=0; i<n; i++)  {
            int d; std::cin >> d;
            dims.push_back(d);
        }
        HyperCuboid hc(m, pos, dims);
        Ways ans = hc.computeWays();
        std::cout << ans << std::endl;
    }
}

