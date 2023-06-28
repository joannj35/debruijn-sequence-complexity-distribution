//
// Created by Razan on 28/06/2023.
//

#include "ComplexityToDebruijn.h"
#include "set"
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

static void generateXORStrings(const string& s, string& a, string& b, int index, vector<pair<string,string>>& options) {
    if (index == s.size()) {
        options.emplace_back(a, b);
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

static vector<pair<string,string>>  getAllXORStrings(const string& s) {
    vector<pair<string,string>> options;
    string a, b;
    generateXORStrings(s, a, b, 0, options);
    return options;
}
bool comparePairs(const pair<string, string>& a, const pair<string, string>& b) {
    return a.first < b.first;
}

static vector<string> generateStrings(const vector<pair<string, string>>& input) {
    vector<string> result;
    string prefix;
    int inputSize = 8;

    // Sort the input vector based on the first element of each pair
    vector<pair<string, string>> sortedInput = input;
    sort(sortedInput.begin(), sortedInput.end(), comparePairs);


    // Generate all possible combinations of first 64 chars
    do {
        vector<bool> check(128, false);
        bool debruijn = true;
        prefix.clear();
        for (int i = 0; i < inputSize; i++) {
            auto val = sortedInput[i].first.substr(0, 7);
            int index = binaryToDecimal(val);
            int index2 = binaryToDecimal(sortedInput[i].first.substr(1, 7));
            if(!check[index] && !check[index2] && index != index2){
                check[index] = true;
                check[index2] = true;
            } else {
                debruijn = false;
                break;
            }
            prefix += sortedInput[i].first;
        }
        if (!debruijn) continue;
        else {
            cout << "im alive" << endl;
        }
        // Generate the corresponding string of the next 64 chars
        string suffix;
        for (int i = 0; i < inputSize; i++) {
            int index = binaryToDecimal(sortedInput[i].second.substr(0, 7));
            int index2 = binaryToDecimal(sortedInput[i].second.substr(1, 7));
            if(!check[index] && !check[index2] && index != index2){
                check[index] = true;
                check[index2] = true;
            } else {
                debruijn = false;
                break;
            }
            suffix += sortedInput[i].second;
        }
        if (!debruijn){
            continue;
        }else {
            cout << "Actually made it " << endl;
        }
        // Combine prefix and suffix
        result.push_back(prefix + suffix);

    } while (next_permutation(sortedInput.begin(), sortedInput.end(), comparePairs));

    return result;
}
//static vector<string> generateDebruijn( const vector<pair<string,string>>& options, int idx,set<string> used, const vector<bool> check, int length, const string& A, const string& B){
//    if(A.length() == length || B.length() == length){
//        if(validate(A+B))
//            return {A+B};
//    }
//    if (used.contains(options[idx].first)){
//        return generateDebruijn(options, )
//    }
//}

ll ComplexityToDebruijn::fromSubseqToDebruijn(string seq) {
    ll count = 0;
    vector<pair<string,string>> options = getAllXORStrings(seq);
    std::vector<std::string> filtered_de_bruijn;
    vector<pair<string,string>> filtered_options;
    n = this->order;
    std::copy_if(options.begin(), options.end(), std::back_inserter(filtered_options), [](const pair<string,string>& s) {
        int i_f1 = binaryToDecimal(s.first.substr(0, 7));
        int i_f2 = binaryToDecimal(s.first.substr(1, 7));
        int i_s1 = binaryToDecimal(s.second.substr(0, 7));
        int i_s2 = binaryToDecimal(s.second.substr(1, 7));
        return i_f1 != i_f2 && i_s1 != i_s2 && i_f1 != i_s2 && i_s1 != i_f2 && i_s2 != i_f2 && i_s1 != i_f1;
    });
    auto all_sequences = generateStrings(filtered_options);
    std::copy_if(all_sequences.begin(), all_sequences.end(), std::back_inserter(filtered_de_bruijn), [](const std::string& s) {
        return validate(s);
    });
    int i = 0;
    for (const auto& s : all_sequences) {
        i++;
        cout << s << endl;
        if(i == 10){
            break;
        }
    }
//    set<string> used;
//    generateDebruijn(options, 0, used, check, pow(2, this->order - 1), "", "");
    return filtered_de_bruijn.size();
}
