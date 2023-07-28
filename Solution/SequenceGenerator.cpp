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
            for (const auto & sequence : this->sequences) {
                if (isRotation(current, sequence))
                    return;
            }
            this->sequences.push_back(current);
        }
        return;
    }

    for (char ch: alphabet) {
        string newCurrent = current + ch;
        generatePermutations(newCurrent);
    }
}

// Function to check if binomial coefficient n choose k is odd over Z_2
bool isOdd(int n, int k) {
    return (n & k) == k;
}

vector<int> expand_over_Z2(int n) {
    vector<int> expansion;
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

// Function to compute sum over Z_2
int sumZ2(const string& seq, const std::vector<int>& indices, int k,int n) {
    int sum = 1;
    for (int i = 1; i < indices.size(); i++) {
        sum += seq[(indices[i] + k)%n];
    }
    return sum % 2;
}

void SequenceGenerator::polynomial(const vector<int>& indices) {
    long long numSequences = pow(2, indices[0]);

    std::vector<std::string> sequences;

    for (int i = 0; i < numSequences && sequences.size() != pow(2, complexity - 6); i++) {
        std::string seq(this->seq_len, '0'); // Initialize string with '0's

        // Set values based on indices and the current sequence number
        for (int j = 0; j < indices[0]; j++) {
            if ((i >> j) & 1) {
                seq[j] = '1'; // Set the character to '1'
            }
        }

        for(int k = 0; k < seq_len; k++){
            int value = sumZ2(seq, indices, k, this->seq_len);
            seq[(indices[0] + k) % this->seq_len] = value + '0'; // Convert the integer value to char ('0' or '1')
        }

        bool r = false;
        for (const std::string& x : sequences) {
            if (isRotation(seq, x)) {
                r = true;
            }
        }

        if (!r) {
            sequences.push_back(seq);
        }
    }

    this->sequences = sequences;
}


void SequenceGenerator::generateSequences() {
    if (this->seq_len > 16){
        vector<int> indices = expand_over_Z2(complexity - 1);
        polynomial(indices);
    } else {
        generatePermutations("");
    }
    this->num_of_seq = this->sequences.size();
}

const vector<string> &SequenceGenerator::getSequences() const {
    return sequences;
}

int SequenceGenerator::getNumOfSeq() const {
    return num_of_seq;
}


