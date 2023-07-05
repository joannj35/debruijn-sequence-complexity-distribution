#include "SequenceGenerator.h"

#include <utility>

static void generatePermutations(const vector<char>& alphabet, int size, const string& current, vector<string>& permutations) {
    if (current.size() == size) {
        permutations.push_back(current);
        return;
    }

    for (char ch: alphabet) {
        string newCurrent = current + ch;
        generatePermutations(alphabet, size, newCurrent, permutations);
    }
}

static vector<string> permutations(const vector<char>& alphabet, int size) {
    vector<string> result;
    generatePermutations(alphabet, size, "", result);
    return result;
}
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


void SequenceGenerator::generateSequences() {
    vector<string> sequences_perm = permutations(this->alphabet,this->seq_len);
    for (const auto& seq: sequences_perm) {
        if (checkComplexity(seq, log2(this->seq_len)) == this->complexity - 1){
            this->sequences.push_back(seq);
            this->num_of_seq++;
        }
    }
}

const vector<string> &SequenceGenerator::getSequences() const {
    return sequences;
}

int SequenceGenerator::getNumOfSeq() const {
    return num_of_seq;
}


