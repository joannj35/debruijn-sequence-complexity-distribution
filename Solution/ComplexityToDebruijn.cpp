#include "ComplexityToDebruijn.h"
#include "set"
int n;
#include <omp.h>
#include <fstream>
#include <chrono>
using namespace std::chrono;
map<string, int> bin_to_dec1;

ComplexityToDebruijn::ComplexityToDebruijn(int complexity, int order, vector<string> skip_sequences, bool read_file, ll total_seq_num) : order(order), skip_sequences(skip_sequences), read_file(read_file), total_seq_num(total_seq_num){
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
    cout << "small sequence start..." << endl;
    auto start = std::chrono::high_resolution_clock::now();
    vector<string> sub_seq;
    if (this->read_file){
        std::string fileName = "sequences_of_complexity_" + to_string(this->sub_complexity) +".txt"; // replace with your file name
        std::ifstream file(fileName);
        std::string line;

            if (file.is_open()) {
                while (getline(file, line)) {
                    sub_seq.push_back(line);
                }

                file.close();
            } else {
                std::cerr << "Unable to open the file: " << fileName << std::endl;
            }

    } else {
        SequenceGenerator sub_sequences(this->sub_complexity);
        sub_seq = sub_sequences.getSequences();
    }
    vector<pair<string,ll>> subseq_to_db(sub_seq.size());
    this->up_to_1000 = vector<vector<string>>(sub_seq.size());
    int i;

    if (skip_sequences.size() == sub_seq.size()){
        cout << "all sequences are skipped, file is complete" << endl;
        return;
    }
    auto mode = std::ios::trunc;
    if (!skip_sequences.empty()) {
        mode = std::ios::app;
    }
    std::ofstream fileout("field_2_span_"+ to_string(order)+"_complexity_"+ to_string(complexity) +".txt",mode);
    if(skip_sequences.empty()) {
        fileout << "For span "<< order << " complexity "<< complexity<< ":" << endl;
    }
    ll continue_from = skip_sequences.size();
    start = std::chrono::high_resolution_clock::now();
    while(!skip_sequences.empty()){
        sub_seq.erase(remove(sub_seq.begin(), sub_seq.end(), skip_sequences.back()));
        skip_sequences.pop_back();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration= duration_cast<std::chrono::seconds>(end - start);
    cout << "sub seq calc done in " << duration.count() << " seconds" << endl;
    skip_sequences.clear();
    #pragma omp parallel for schedule(dynamic) shared(subseq_to_db,sub_seq,n,cout,fileout,continue_from) private(i) default(none)
    for(i = 0; i < sub_seq.size(); i++) {
        auto seq = sub_seq[i];
        string x = seq;
        vector<string> db_seq;
        ll num = fromSubseqToDebruijn(x,db_seq);

        #pragma omp critical
        {
            fileout << "Debruijn Sequences generated by the sub sequence " << seq << " : " << endl;
            for (const auto &str: db_seq) {
                fileout << str << endl;
            }
            fileout << "the number of Debruijn sequences: " << num << endl << endl;
            total_seq_num += num;
            cout << "Sequence #" << i+continue_from << ": " << seq << " - " << num << endl;
        }
    }
    fileout << "number of small sequences with complexity " << complexity - pow(2,order - 1)<< " is: " << sub_seq.size() << endl;
    fileout << "total number of sequences of complexity " << complexity << " is: " << total_seq_num << endl;
    end = high_resolution_clock::now();
    duration= duration_cast<seconds>(end - start);
    cout << "total number of sequences of complexity " << complexity << " is: " << total_seq_num << endl;
    if(duration.count() < 1) {
        cout << "overall execution time is " << duration_cast<milliseconds>(end - start).count() << " milliseconds"
             << endl;
    } else if(duration.count() > 60 && duration.count() < 3600){
        cout << "overall execution time is " << duration_cast<minutes>(end - start).count() << " minutes" << endl;
    } else if(duration.count() >= 3600) {
        cout << "overall execution time is " << duration_cast<hours>(end - start).count() << " hours, " << duration_cast<minutes>(end - start).count() % 60 << " minutes" << endl;
    }
    else {
        cout << "overall execution time is " << duration.count() << " seconds" << endl;
    }
}

void ComplexityToDebruijn::generateXORStrings(const string& s, string& a, string& b, int index, vector<pair<string,string>>& options, vector<bool> check, vector<string>& db_seq) {
    if (index == pow(2, order - 1)) {
        //if(options.size() % 1000 == 0 && options.size() != 0){
          //  cout << "Current size = " << options.size() << endl;
        //}
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
            for (const auto &op: options) {
                if (isRotation(a_b, op.first + op.second)) {
                    return;
                }
            }
            if (db_seq.size() < 1000)
                db_seq.push_back(a + b);
            options.emplace_back(a, b);
        }
        return;
    }

    vector<pair<char, char>> combinations;
    if (s[index%s.size()] == '0') {
        combinations = {{'0', '0'}, {'1', '1'}};
    } else if (s[index%s.size()] == '1') {
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
                generateXORStrings(s, a, b, index + 1, options, check, db_seq);
                check[a_sub] = false;
                check[b_sub] = false;
            }
        } else {
            generateXORStrings(s, a, b, index + 1, options, check,db_seq);
        }
        a.pop_back();
        b.pop_back();
    }
}


/*
 * TODO:
 * remove rotations in an efficient way
 */
vector<pair<string,string>> ComplexityToDebruijn::getAllXORStrings(string s, vector<string>& db_seq) {
//static ll  getAllXORStrings(const string& s) {
    vector<pair<string,string>> options, filtered_options;
//    ll options = 0;
    string a = "", b = "";
    vector<bool> check(pow(2,n), false);
    //#pragma omp task private(s, a, b, options, bin_to_dec1,check) default(none)
    generateXORStrings(s, a, b, 0, options, check,db_seq);
    return options;
}

ll ComplexityToDebruijn::fromSubseqToDebruijn(string seq, vector<string>& db_seq) {
    ll count = 0;
    n = this->order;
    //bin_to_dec1 = generateStringMap();
    vector<pair<string,string>> options;
//
//    #pragma omp task private(seq, options, bin_to_dec1) default(none)
    options = getAllXORStrings(seq,db_seq);
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

const vector<vector<string>> &ComplexityToDebruijn::getUpTo1000() const {
    return up_to_1000;
}

ll ComplexityToDebruijn::getTotalSeqNum() const {
    return total_seq_num;
}
