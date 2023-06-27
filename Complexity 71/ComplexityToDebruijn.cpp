//
// Created by Razan on 28/06/2023.
//

#include "ComplexityToDebruijn.h"
int n;

ComplexityToDebruijn::ComplexityToDebruijn(int complexity, int order) : complexity(complexity), order(order), total_seq_num(0) {}

static int binaryToDecimal(const std::string& binaryString) {
    int decimalValue = 0;
    int length = binaryString.length();

    for (int i = 0; i < length; ++i) {
        if (binaryString[i] == '1') {
            decimalValue += pow(2, length - 1 - i);
        }
    }

    return decimalValue;
}

static bool validate(string seq){
    int seq_size = pow(2, n);
    if(seq.length() != seq_size){
        return false;
    }
    vector<bool> sub_seq(seq_size, false);
    string rotation = seq;
    for (int i = 0; i < n; ++i) {
        rotation += seq[i];
    }
    string val = "";
    for (int i = 0; i < seq_size+n; ++i) {
        if(i >= n){
            int index = binaryToDecimal(val);
            if(!sub_seq[index]){
                sub_seq[index] = true;
            } else {
                return false;
            }
            val = val.substr(1);
        }
        val += rotation[i];
    }
    return std::ranges::all_of(sub_seq, [](bool i) { return i; });
}

void ComplexityToDebruijn::compute() {
    SequenceGenerator sub_sequences(7);
    auto sub_seq = sub_sequences.getSequences();
    for (auto seq : sub_seq) {
        this->subseq_to_debruijn.insert({seq, fromSubseqToDebruijn(seq)});
    }
}

static void generateXORStrings(const string& s, string& a, string& b, int index, vector<string>& options) {
    if (index == s.size()) {
        options.push_back(a + " " + b);
        return;
    }

    if (s[index] == '0') {
        a += '0';
        b += '0';
        generateXORStrings(s, a, b, index + 1, options);
        a.pop_back();
        b.pop_back();

        a += '1';
        b += '1';
        generateXORStrings(s, a, b, index + 1, options);
        a.pop_back();
        b.pop_back();

    } else if (s[index] == '1') {
        a += '1';
        b += '0';
        generateXORStrings(s, a, b, index + 1, options);
        a.pop_back();
        b.pop_back();

        a += '0';
        b += '1';
        generateXORStrings(s, a, b, index + 1, options);
        a.pop_back();
        b.pop_back();
    }
}

static vector<string> getAllXORStrings(const string& s) {
    vector<string> options;
    string a, b;
    generateXORStrings(s, a, b, 0, options);
    return options;
}

ll ComplexityToDebruijn::fromSubseqToDebruijn(string seq) {
    ll count = 0;
    vector<bool> check(pow(2, this->order), false);
    vector<string> options;
    string a, b;
    generateXORStrings(seq, a, b, 0, options);
    std::vector<std::string> filtered_de_bruijn;
    n = this->order;
//    std::copy_if(options.begin(), options.end(), std::back_inserter(filtered_de_bruijn), [](const std::string& s) {
//        return validate(s);
//    }); //wrong need to do after building all of the seq.
    cout << options[1] << endl;
//    for (auto s: filtered_de_bruijn) {
//        cout << s << endl;
//    }
    return filtered_de_bruijn.size();
}
