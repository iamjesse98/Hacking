// -------------------------------------------------------------------------------------
//         Author: Sourabh S Joshi (cbrghostrider); Copyright - All rights reserved.
//                       For email, run on linux (perl v5.8.5):
//   perl -e 'print pack "H*","736f75726162682e732e6a6f73686940676d61696c2e636f6d0a"'
// -------------------------------------------------------------------------------------
class Solution {
private:
    unordered_map<int, string> roman = {
        {1000, "M"}, {900, "CM"}, {500, "D"}, {400, "CD"}, 
        {100, "C"}, {90, "XC"}, {50, "L"}, {40, "XL"}, 
        {10, "X"}, {9, "IX"}, {5, "V"}, {4, "IV"}, {1, "I"}};

public: 
    string intToRoman(int num) {
        string retval;
        if (num == 0) return retval;
        
        vector<int> keys;
        for (auto kv : roman) keys.push_back(kv.first);
        sort(keys.begin(), keys.end(), std::greater<int>());
        
        for (const int& key : keys) {
            int numq = num / key;
            for (int i=0; i<numq; i++) retval += roman[key];
            num = num % key;
        }
        
        return retval;
    }
};
