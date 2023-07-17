#include "ComplexityToDebruijn.h"
#include "set"
int n;
#include <omp.h>
map<string, int> bin_to_dec1;

ComplexityToDebruijn::ComplexityToDebruijn(int complexity, int order) : order(order), total_seq_num(0) {
    this->complexity = complexity;
    this->sub_complexity = this->complexity - pow(2,order - 1);
}

map<string, int> generateStringMap() {
    map<string, int> stringMap;

    for (int i = 0; i < pow(2,n); i++) {
        string binaryString;
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
}

void ComplexityToDebruijn::compute() {
    SequenceGenerator sub_sequences(this->sub_complexity);
    auto sub_seq = removeRotations(sub_sequences.getSequences());
    vector<pair<string,ll>> subseq_to_db(sub_seq.size());
    int i;
    #pragma omp parallel for schedule(dynamic) shared(subseq_to_db,sub_seq,n) private(i) default(none)
    for(i = 0; i < sub_seq.size(); i++) {
        auto seq = sub_seq[i];
        string x = seq + seq;
        if (seq.size() == 8) {
            x += seq + seq;
        }
        ll num = fromSubseqToDebruijn(x);
        #pragma omp critical
        subseq_to_db[i] = {seq, num};
    }
    this->subseq_to_debruijn = subseq_to_db;
}

void ComplexityToDebruijn::generateXORStrings(const string& s, string& a, string& b, int index, vector<pair<string,string>>& options, vector<bool> check) {
    if (index == s.size()) {
        auto a_b = a + b;
        auto b_a = b + a;
        for (int i = n - 1; i > 0; i--) {
            auto a_sub = binaryToDecimal(a_b.substr(a.size() - i, n));
            auto b_sub = binaryToDecimal(b_a.substr(a.size() - i, n));
            if (!check[a_sub] && !check[b_sub] && a_sub != b_sub) {
                check[a_sub] = true;
                check[b_sub] = true;
            } else {
                return;
            }
        }
        if (find(options.begin(), options.end(), make_pair(b, a)) == options.end()) {
            for (const auto& op: options){
                if (isRotation(a_b, op.first + op.second)){
                    return;
                }
            }
//            if(options.size() <= 1000)
//                cout << a+b << endl;
            options.emplace_back(a, b);
        }
        return;
    }

    vector<pair<char, char>> combinations;
    if (s[index] == '0') {
        combinations = {{'0', '0'}, {'1', '1'}};
    } else if (s[index] == '1') {
        combinations = {{'1', '0'}, {'0', '1'}};
    }

    for (const auto& combination : combinations) {
        a += combination.first;
        b += combination.second;
        if (a.size() >= n) {
            auto a_sub = binaryToDecimal(a.substr(a.size() - n, n));
            auto b_sub = binaryToDecimal(b.substr(b.size() - n, n));
            if (!check[a_sub] && !check[b_sub] && a_sub != b_sub) {
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
vector<pair<string,string>> ComplexityToDebruijn::getAllXORStrings(string s) {
//static ll  getAllXORStrings(const string& s) {
    vector<pair<string,string>> options, filtered_options;
//    ll options = 0;
    string a = "", b = "";
    vector<bool> check(pow(2,n), false);
    //#pragma omp task private(s, a, b, options, bin_to_dec1,check) default(none)
    generateXORStrings(s, a, b, 0, options, check);
    return options;
}

ll ComplexityToDebruijn::fromSubseqToDebruijn(string seq) {
    ll count = 0;
    n = this->order;
    //bin_to_dec1 = generateStringMap();
    vector<pair<string,string>> options;
//
//    #pragma omp task private(seq, options, bin_to_dec1) default(none)
    options = getAllXORStrings(seq);
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

const vector<pair<string, ll>> &ComplexityToDebruijn::getSubseqToDebruijn() const {
    return subseq_to_debruijn;
}
