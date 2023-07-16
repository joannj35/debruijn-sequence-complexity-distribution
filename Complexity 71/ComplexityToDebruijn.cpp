#include "ComplexityToDebruijn.h"
#include "set"
int n;
ll amount_of_sequences_overall = 0;
map<string, int> bin_to_dec1;

ComplexityToDebruijn::ComplexityToDebruijn(int complexity, int order) : order(order), total_seq_num(0) {
    this->complexity = complexity;
    this->sub_complexity = this->complexity - pow(2,order - 1);
}

map<string, int> generateStringMap() {
    map<string, int> stringMap;

    for (int i = 0; i < pow(2,n); i++) {
        string binaryString = "";
        int value = i;

        for (int j = 0; j < n; j++) {
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

bool ComplexityToDebruijn::isRotation(const std::string& s1, std::string s2)
{
    if (s1.size() != s2.size())
        return false;

    if (s1 == s2)
        return true;

    string temp = s1 + s1;
    return (temp.find(s2) != string::npos);

//    for (size_t i = 0; i < s2.size(); i++)
//    {
//        std::rotate(s2.begin(), s2.begin() + 1, s2.end());
//        if (s1 == s2)
//            return true;
//    }
//
//    return false;
}

static bool _isRotation(const std::string& s1, std::string s2)
{
    if (s1.size() != s2.size())
        return false;

    if (s1 == s2)
        return true;

    for (size_t i = 0; i < s2.size()/16; i++)
    {
        std::rotate(s2.begin(), s2.begin() + 16, s2.end());
        if (s1 == s2)
            return true;
    }

    return false;
}
void ComplexityToDebruijn::compute() {
    SequenceGenerator sub_sequences(this->sub_complexity);
    auto sub_seq = removeRotations(sub_sequences.getSequences());
    for (auto seq : sub_seq) {
        this->subseq_to_debruijn.insert({seq, fromSubseqToDebruijn(seq)});
    }
}

void ComplexityToDebruijn::generateXORStrings(const string& s, string& a, string& b, int index, vector<pair<string,string>>& options, vector<bool> check) {
//static void generateXORStrings(const string& s, string& a, string& b, int index, ll& options, vector<bool> check) {
    if (index == s.size()) {
        auto a_b = a+b;
        auto b_a = b+a; //b concat a
        for (int i = n - 1; i > 0; i--) {
            auto a_sub = bin_to_dec1[a_b.substr(a.size() - i, n)];
            auto b_sub = bin_to_dec1[b_a.substr(a.size() - i, n)];
            if (!check[a_sub] && !check[b_sub] && a_sub != b_sub) {
                check[a_sub] = true;
                check[b_sub] = true;
            }
            else return;
        }

        //cout << a+b << endl;
        if (find(options.begin(), options.end(), make_pair(b, a)) == options.end()) {
            for (auto aux: options){
                if (isRotation(a_b, aux.first+aux.second)){
                    return;
                }
            }
            //cout << a+b << endl;
            options.emplace_back(a, b);
        }
//        if (validate(a+b)) options++;
        return;
    }

    if (s[index] == '0') {
        a += '0';
        b += '0';
        if(a.size() >= n){
            auto a_sub = bin_to_dec1[a.substr(a.size() - n, n)];
            auto b_sub = bin_to_dec1[b.substr(b.size() - n, n)];
            if (!check[a_sub] && !check[b_sub] && a_sub!=b_sub){
                check[a_sub] = true;
                check[b_sub] = true;
                generateXORStrings(s, a, b, index + 1, options, check);
                check[a_sub] = false;
                check[b_sub] = false;
            }
        } else {
            generateXORStrings(s, a, b, index + 1, options, check);
        }
        a.pop_back();
        b.pop_back();

        a += '1';
        b += '1';
        if(a.size() >= n){
            auto a_sub = bin_to_dec1[a.substr(a.size() - n, n)];
            auto b_sub = bin_to_dec1[b.substr(b.size() - n, n)];
            if (!check[a_sub] && !check[b_sub] && a_sub != b_sub){
                check[a_sub] = true;
                check[b_sub] = true;
                generateXORStrings(s, a, b, index + 1, options, check);
                check[a_sub] = false;
                check[b_sub] = false;
            }
        } else {
            generateXORStrings(s, a, b, index + 1, options, check);
        }
        a.pop_back();
        b.pop_back();

    } else if (s[index] == '1') {
        a += '1';
        b += '0';
        if(a.size() >= n){
            auto a_sub = bin_to_dec1[a.substr(a.size() - n, n)];
            auto b_sub = bin_to_dec1[b.substr(b.size() - n, n)];
            if (!check[a_sub] && !check[b_sub] && a_sub != b_sub){
                check[a_sub] = true;
                check[b_sub] = true;
                generateXORStrings(s, a, b, index + 1, options, check);
                check[a_sub] = false;
                check[b_sub] = false;
            }
        } else {
            generateXORStrings(s, a, b, index + 1, options, check);
        }
        a.pop_back();
        b.pop_back();

        a += '0';
        b += '1';
        if(a.size() >= n){
            auto a_sub = bin_to_dec1[a.substr(a.size() - n, n)];
            auto b_sub = bin_to_dec1[b.substr(b.size() - n, n)];
            if (!check[a_sub] && !check[b_sub] && a_sub != b_sub){
                check[a_sub] = true;
                check[b_sub] = true;
                generateXORStrings(s, a, b, index + 1, options, check);
                check[a_sub] = false;
                check[b_sub] = false;
            }
        } else {
            generateXORStrings(s, a, b, index + 1, options, check);
        }
        a.pop_back();
        b.pop_back();
    }
}

/*
 * TODO:
 * remove rotations in an efficient way
 */
vector<pair<string,string>> ComplexityToDebruijn::getAllXORStrings(const string& s) {
//static ll  getAllXORStrings(const string& s) {
    vector<pair<string,string>> options, filtered_options;
//    ll options = 0;
    string a = "", b = "";
    vector<bool> check(pow(2,n), false);
    generateXORStrings(s, a, b, 0, options, check);
    return options;
}

bool comparePairs(const pair<string, string>& a, const pair<string, string>& b) {
    return a.first < b.first;
}



static vector<string> generateStrings(const vector<pair<string, string>>& input,map<string, int> bin_to_dec, vector<string>& result) {
    string prefix;
    int inputSize = 4;

    // Sort the input vector based on the first element of each pair
    vector<pair<string, string>> sortedInput = input;
    sort(sortedInput.begin(), sortedInput.end(), comparePairs);
        // Generate all possible combinations of first 64 chars
        do {
            vector<bool> check(pow(2,n), false);
            bool debruijn = true;
            prefix.clear();
            for (int i = 0; i < inputSize; i++) {
                int index = bin_to_dec[sortedInput[i].first.substr(0, n)];
                int index2 = bin_to_dec[sortedInput[i].first.substr(1, n)];
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
                int index = bin_to_dec[sortedInput[i].second.substr(0, n)];
                int index2 = bin_to_dec[sortedInput[i].second.substr(1, n)];
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
    n = this->order;
    bin_to_dec1 = generateStringMap();
    vector<pair<string,string>> options = getAllXORStrings(seq);
//    for(auto p : options){
//        cout << "(" << p.first+p.second << ")" << endl;
//    }

    return options.size();
}

vector<string> ComplexityToDebruijn::removeRotations(const vector<string> &sequences) {
    vector<string> distinct_sequences;
    for (auto seq:sequences) {
        bool rotation = false;
        for (auto d_sub:distinct_sequences){
            if(isRotation(d_sub,seq)){
                rotation = true;
                break;
            }
        }
        if (!rotation){
            distinct_sequences.emplace_back(seq);
        }
    }
    return distinct_sequences;
}