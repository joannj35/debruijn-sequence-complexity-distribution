#include "SequenceGenerator.h"

#include <utility>

static bool isRotation(const std::string& s1, const std::string& s2)
{
    if (s1.size() != s2.size())
        return false;

    if (s1 == s2)
        return true;

    string temp = s1 + s1;
    return (temp.find(s2) != string::npos);
}

/// runs games-chan algorithm
static int checkComplexity(const std::string& S, int m) {
    if (m == 0){
        return 0;
    }
    bool allzeros = std::all_of(S.begin(), S.end(), [](char c) {
        return c == '0';
    });
    if(allzeros){
        return 0;
    }
    string A_m, L, R, B_m = "";
    A_m = S;
    int c_m = pow(2,m-1);
    L = A_m.substr(0, c_m);
    R = A_m.substr(c_m);
    bool b_allzero = true;
    for (int i = 0; i < c_m; ++i) {
        if (L[i] != R[i]) {
            B_m += "1";
            b_allzero = false;
        } else {
            B_m += "0";
        }
    }
    int m_1;
    if (b_allzero){
        m_1 = checkComplexity(L, m - 1);
    } else{
        m_1 = checkComplexity(B_m, m - 1) + c_m;
    }
    return m_1;
}

void SequenceGenerator::generatePermutations(const string& current) {
    if (current.size() == this->seq_len) {
        if (checkComplexity(current, log2(this->seq_len)) == this->complexity - 1) {
//            for (const auto & sequence : this->sequences) {
////                if (isRotation(current, sequence))
////                    return;
//            }
            this->sequences.push_back(current);
        }
        return;
    }

    for (char ch: alphabet) {
        string newCurrent = current + ch;
        generatePermutations(newCurrent);
    }
}

void SequenceGenerator::generateSequences() {
    if (seq_len <= 16){
        //generate all possible binary sequences of length seq_len and return all sequences with the desired complexity
        generatePermutations("");
        this->num_of_seq = this->sequences.size();
    } else {
        auto indices = expand_over_Z2(complexity - 1);

    }

}

const vector<string> &SequenceGenerator::getSequences() const {
    return sequences;
}

int SequenceGenerator::getNumOfSeq() const {
    return num_of_seq;
}

// Function to check if binomial coefficient n choose k is odd over Z_2
static bool isOdd(int n, int k) {
    return (n & k) == k;
}

vector<int> SequenceGenerator::expand_over_Z2(int n) {
    std::vector<int> expansion;
    for (int k = 0; k <= n; k++) {
        if (isOdd(n, k)) {
            if (k == n) {
                expansion.push_back(0);
            } else if (k == n - 1) {
                expansion.push_back(1);
            } else {
                expansion.push_back(n - k);
            }
        }
    }
    return expansion;
}

void SequenceGenerator::sequencesFromIndices(const vector<int>& indices, int index, const string &current) {
    if(index == indices.size()){
        return;
    }
}


//int main() {
//    int n;
//    std::cout << "Enter a value for n: ";
//    while (true){
//        std::cin >> n;
//        if(n == 0) break;
//        std::cout << "The expansion of (x + 1)^" << n << " over Z_2 is:\n";
//        std::cout << expand_over_Z2(n) << std::endl;
//    }
//    return 0;
//}

