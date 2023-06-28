#include "ComplexityToDebruijn.h"
#include "set"
int n;

ComplexityToDebruijn::ComplexityToDebruijn(int complexity, int order) : complexity(complexity), order(order), total_seq_num(0) {}

map<string, int> generateStringMap() {
    map<string, int> stringMap;

    for (int i = 0; i < 128; i++) {
        string binaryString = "";
        int value = i;

        for (int j = 0; j < 7; j++) {
            binaryString = (char)('0' + (value % 2)) + binaryString;
            value /= 2;
        }

        stringMap[binaryString] = i;
    }

    return stringMap;
}

static int binaryToDecimal(const string& binaryString) {
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
    return ranges::all_of(sub_seq, [](bool i) { return i; });
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
        if (find(options.begin(), options.end(), make_pair(b, a)) == options.end()) {
            options.emplace_back(a, b);
        }
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

static vector<string> generateStrings(const vector<pair<string, string>>& input,map<string, int> bin_to_dec, vector<string>& result) {
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
                int index = bin_to_dec[sortedInput[i].first.substr(0, 7)];
                int index2 = bin_to_dec[sortedInput[i].first.substr(1, 7)];
                if(!check[index] && !check[index2] && index != index2){
                    check[index] = true;
                    check[index2] = true;
                } else {
                    debruijn = false;
                    break;
                }
                prefix += sortedInput[i].first;
            }
            if (!debruijn) break;
            else {
                cout << "im alive" << endl;
            }
            // Generate the corresponding string of the next 64 chars
            string suffix;
            for (int i = 0; i < inputSize; i++) {
                int index = bin_to_dec[sortedInput[i].second.substr(0, 7)];
                int index2 = bin_to_dec[sortedInput[i].second.substr(1, 7)];
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
                break;
            }else {
                cout << "Actually made it " << endl;
            }
            // Combine prefix and suffix
            result.push_back(prefix + suffix);

        } while (next_permutation(sortedInput.begin(), sortedInput.end(), comparePairs));
    return result;
}

// Function to generate all combinations of choosing k pairs from the remaining pairs
static void generateCombinations(const vector<pair<string, string>>& pairs,
                                 int k, int index, vector<pair<string, string>>& currentCombination,
                                 vector<vector<pair<string, string>>>& combinations, const map<string, int>& bin_to_dec, vector<string>& all_sequences) {
    if (k == 0) {
        generateStrings(currentCombination,bin_to_dec, all_sequences);
        combinations.push_back(currentCombination);
        return;
    }

    int n_p = pairs.size();
    for (int i = index; i <= n_p - k; ++i) {
        currentCombination.push_back(pairs[i]);
        generateCombinations(pairs, k - 1, i + 1, currentCombination, combinations, bin_to_dec, all_sequences);
        currentCombination.pop_back();
    }
}

// Function to perform the combination and return all combinations of choosing 8 pairs
vector<vector<pair<string, string>>> choosePairs(const vector<pair<string, string>>& pairs, const map<string, int>& bin_to_dec, vector<string>& result) {
    vector<vector<pair<string, string>>> combinations;
    vector<pair<string, string>> currentCombination;

    generateCombinations(pairs, 8, 0, currentCombination, combinations, bin_to_dec,result);

    return combinations;
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
    vector<string> filtered_de_bruijn;
    vector<pair<string,string>> filtered_options;
    n = this->order;
    map<string, int> bin_to_dec = generateStringMap();
    copy_if(options.begin(), options.end(), back_inserter(filtered_options), [](const pair<string,string>& s) {
        map<string, int> bin_to_dec = generateStringMap();
        int i_f1 = bin_to_dec[s.first.substr(0, 7)];
        int i_f2 = bin_to_dec[s.first.substr(1, 7)];
        int i_s1 = bin_to_dec[s.second.substr(0, 7)];
        int i_s2 = bin_to_dec[s.second.substr(1, 7)];
        return i_f1 != i_f2 && i_s1 != i_s2 && i_f1 != i_s2 && i_s1 != i_f2 && i_s2 != i_f2 && i_s1 != i_f1;
    });
    vector<string> all_sequences;
    auto pair_comb = choosePairs(filtered_options, bin_to_dec, all_sequences);

    copy_if(all_sequences.begin(), all_sequences.end(), back_inserter(filtered_de_bruijn), [](const string& s) {
        return validate(s);
    });
//    set<string> used;
//    generateDebruijn(options, 0, used, check, pow(2, this->order - 1), "", "");
    return filtered_de_bruijn.size();
}
