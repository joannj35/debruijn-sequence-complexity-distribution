#include "ComplexityToDebruijn.h"
#include "set"
int n;
#include <fstream>
#include <chrono>
#include <cstring>
#include <omp.h>
using namespace std::chrono;
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

static int binaryToDecimal1(const string& binaryString) {
    int decimalValue = 0;
    int length = binaryString.length();

    for (int i = 0; i < length; ++i) {
        if (binaryString[i] == '1') {
            decimalValue += pow(2, length - 1 - i);
        }
    }

    return decimalValue;
}

#pragma omp declare target
static int binaryToDecimal(const char binaryString[]) {
    int decimalValue = 0;
    int length = strlen(binaryString);

    for (int i = 0; i < length; ++i) {
        if (binaryString[i] == '1') {
            decimalValue += pow(2, length - 1 - i);
        }
    }

    return decimalValue;
}
#pragma omp end declare target

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
            int index = binaryToDecimal1(val);
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

#pragma omp declare target
static char* substring(const char* source, int start, int length) {
    char* result = new char[length + 1];  // +1 for the null terminator
    strncpy(result, source + start, length);
    result[length] = '\0';  // null terminate the resulting substring
    return result;
}
#pragma omp end declare target

#pragma omp declare target
bool ComplexityToDebruijn::isRotation(const char* s1, const char* s2) {
    size_t len1 = strlen(s1);
    size_t len2 = pow(2,n);
    char* s2_sub = substring(s2,0,len2);
    if (len1 != len2) {
        return false;
    }

    if (strcmp(s1, s2_sub) == 0) {
        return true;
    }

    char* temp = new char[2 * len1 + 1];  // +1 for the null terminator
    strcpy(temp, s1);
    strcat(temp, s1);
    temp[2 * len1] = '\0';
    bool result = (strstr(temp, s2_sub) != nullptr);
    delete[] s2_sub;
    delete[] temp;  // free the allocated memory
    return result;
}
#pragma omp end declare target

void ComplexityToDebruijn::compute() {
    cout << "small sequence start..." << endl;
    auto start = std::chrono::high_resolution_clock::now();
    SequenceGenerator sub_sequences(this->sub_complexity);
    auto sub_seq =sub_sequences.getSequences();
    int i;
    auto end = std::chrono::high_resolution_clock::now();
    auto duration= duration_cast<std::chrono::seconds>(end - start);
    cout << "sub seq calc done in " << duration.count() << " seconds" << endl;
    std::ofstream fileout("order_"+ to_string(order)+"_complexity_"+ to_string(complexity) +"_omp_gpu.txt");
    cout << "For order "<< order << " complexity "<< complexity<< ":" << endl;
    fileout << "For order "<< order << " complexity "<< complexity<< ":" << endl;
    start = std::chrono::high_resolution_clock::now();
    ll* subseq_to_db_flatten = new ll[sub_seq.size()];
    int num_sub_seq = sub_seq.size();
    int sub_size = sub_seq[0].size();
    int db_size = pow(2,order);
    this->up_to_1000 =  new char[1000 * num_sub_seq * db_size];
    auto up_to_1000_flatten = this->up_to_1000;
    // Flatten the sub_seq data
    char* flattened_data = new char[num_sub_seq * sub_size + 1];
    for (int k = 0; k < num_sub_seq; ++k) {
        for (int j = 0; j < sub_size; ++j) {
            flattened_data[k * sub_size + j] = sub_seq[k][j];
        }
    }
    flattened_data[num_sub_seq * sub_size] = '\0';
    #pragma omp target data map(to: flattened_data[0:num_sub_seq*sub_size], db_size, num_sub_seq, sub_size) map(tofrom: subseq_to_db_flatten[0:num_sub_seq]) map(tofrom: up_to_1000_flatten[0:num_sub_seq*1000])
    {
        // Parallelize the loop
        #pragma omp target teams distribute parallel for schedule(dynamic) shared(db_size,flattened_data,sub_size,num_sub_seq,n, up_to_1000_flatten,subseq_to_db_flatten) private(i) default(none) num_teams(4) thread_limit(1024)
        for (i = 0; i < num_sub_seq; i++) {
            char seq[sub_size + 1];  // Use stack allocation

            // Replace strcpy with a manual loop
            for (int j = 0; j < sub_size; j++) {
                seq[j] = flattened_data[i * sub_size + j];
            }
            seq[sub_size] = '\0';
            char* db_seq = up_to_1000_flatten + (1000 * i * db_size);
            ll num = fromSubseqToDebruijn(seq, db_seq);
            subseq_to_db_flatten[i] = num;
        }
    }
    for (int c = 0; c < num_sub_seq; c++){
        auto seq = sub_seq[c];
        auto num = subseq_to_db_flatten[c];
        fileout << "Debruijn Sequences generated by the sub sequence " << seq << " : " << endl;
        char* str = up_to_1000_flatten + (c * 1000 * db_size);
        int s = 0;
        while (str != nullptr && s < 1000 && s < num) {
            char* sub_str = substring(str,0,db_size);
            fileout << sub_str << endl;
            str = str + db_size;
            s++;
            delete[] sub_str;
        }
        fileout << "the number of Debruijn sequences: " << num << endl << endl;
        total_seq_num += num;
    }
    fileout << "number of small sequences with complexity " << complexity - pow(2,order - 1)<< " is: " << sub_seq.size() << endl;
    fileout << "total number of sequences of complexity " << complexity << " is: " << total_seq_num << endl;
    end = high_resolution_clock::now();
    duration= duration_cast<seconds>(end - start);
    cout << "total number of sequences of complexity " << complexity << " is: " << total_seq_num << endl;
    if(duration.count() < 1) {
        cout << "overall execution time is " << duration_cast<milliseconds>(end - start).count() << " milliseconds"
             << endl;
        fileout << "overall execution time is " << duration_cast<milliseconds>(end - start).count() << " milliseconds"
                << endl;
    } else if(duration.count() > 60 && duration.count() < 3600){
        cout << "overall execution time is " << duration_cast<minutes>(end - start).count() << " minutes" << endl;
        fileout << "overall execution time is " << duration_cast<minutes>(end - start).count() << " minutes" << endl;
    } else if(duration.count() >= 3600) {
        cout << "overall execution time is " << duration_cast<hours>(end - start).count() << " hours, " << duration_cast<minutes>(end - start).count() % 60 << " minutes" << endl;
        fileout << "overall execution time is " << duration_cast<hours>(end - start).count() << " hours, " << duration_cast<minutes>(end - start).count() % 60 << " minutes" << endl;
    }
    else {
        cout << "overall execution time is " << duration.count() << " seconds" << endl;
        fileout << "overall execution time is " << duration.count() << " seconds" << endl;
    }
    delete[] flattened_data;
    delete[] subseq_to_db_flatten;
}

#pragma omp declare target
void ComplexityToDebruijn::generateXORStrings(const char* s, char*& a, char*& b, int index, char**& options, bool* check, char*& db_seq, ll& options_size) {
    int size = pow(2,order);
    if (index == pow(2, order - 1)) {
        char* a_b = new char[size + 1];
        strcpy(a_b, a);
        strcat(a_b,b);
        a_b[size] = '\0';
        char* b_a = new char[size + 1];
        strcpy(b_a, b);
        strcat(b_a,a);
        b_a[size] = '\0';
        bool* check_temp = new bool[size];

        for(int i = 0; i < size; ++i) {
            check_temp[i] = false;
        }
        for (int i = n - 1; i > 0; i--) {
            char* a_b_substr = substring(a_b, index - i, n);
            char* b_a_substr = substring(b_a, index - i, n);
            int a_sub = binaryToDecimal(a_b_substr);
            int b_sub = binaryToDecimal(b_a_substr);
            delete[] a_b_substr;
            delete[] b_a_substr;
            if (!check[a_sub] && !check[b_sub] && a_sub != b_sub && !check_temp[a_sub] && !check_temp[b_sub]) {
                check_temp[a_sub] = true;
                check_temp[b_sub] = true;
            } else {
                delete[] check_temp;
                delete[] a_b;
                delete[] b_a;
                return;
            }
        }
        delete[] check_temp;
        for (int op = 0; op < options_size; op++) {
            if (isRotation(a_b, options[op])) {
                delete[] a_b;
                delete[] b_a;
                return;
            }
        }
        if (options_size < 1000){
            strcpy(db_seq,a_b);
            db_seq = db_seq + size;
        }
        strcpy(options[options_size],a_b);
        options_size++;
        delete[] a_b;
        delete[] b_a;
        return;
    }

    char* combinations = new char[4];
    int s_size = strlen(s);
    if (s[index%s_size] == '0') {
        combinations[0] = '0';
        combinations[1] = '0';
        combinations[2] = '1';
        combinations[3] = '1';
    } else if (s[index%s_size] == '1') {
        combinations[0] = '1';
        combinations[1] = '0';
        combinations[2] = '0';
        combinations[3] = '1';
    }

    for (int c = 0; c < 2; c++) {
        a[index] = combinations[c*2];
        b[index] = combinations[c*2 + 1];
        a[index + 1] = '\0';
        b[index + 1] = '\0';
        if (index >= (n - 1)) {
            char* a_substr = substring(a, index - n + 1, n);
            char* b_substr = substring(b, index - n + 1, n);
            int a_sub = binaryToDecimal(a_substr);
            int b_sub = binaryToDecimal(b_substr);
            delete a_substr;
            delete b_substr;
            if (!check[a_sub] && !check[b_sub] && a_sub != b_sub) {
                check[a_sub] = true;
                check[b_sub] = true;
                generateXORStrings(s, a, b, index + 1, options, check, db_seq, options_size);
                check[a_sub] = false;
                check[b_sub] = false;
            }
        } else {
            generateXORStrings(s, a, b, index + 1, options, check,db_seq, options_size);
        }
        a[index] = '\0';
        b[index] = '\0';
    }
}
#pragma omp end declare target

/*
 * TODO:
 * remove rotations in an efficient way
 */
#pragma omp declare target
ll ComplexityToDebruijn::getAllXORStrings(const char* s, char*& db_seq, ll& options_size) {
    const int maxPairs = 5000;
    // TODO: make options dynamic
    int maxStringSize = pow(2, n);

    // Allocate a 1D array to hold all strings
    char* strings = new char[2 * maxPairs * maxStringSize];

    // Allocate array of pointers
    char** options = new char*[2 * maxPairs];

    // Initialize pointers
    for(int i = 0; i < 2 * maxPairs; ++i) {
        options[i] = &strings[i * maxStringSize];
    }
//    ll options = 0;
    char* a = new char[maxStringSize + 1];
    char* b = new char[maxStringSize + 1];
    bool* check = new bool[maxStringSize];

    for(int i = 0; i < maxStringSize; ++i) {
        check[i] = false;
    }
    generateXORStrings(s, a, b, 0, options, check, db_seq, options_size);
    // Initialize pointers
    delete[] a;
    delete[] b;
    delete[] check;
    delete[] strings;
    delete[] options;
    return options_size;
}
#pragma omp end declare target

#pragma omp declare target
ll ComplexityToDebruijn::fromSubseqToDebruijn(const char* seq, char*& db_seq) {
    ll count = 0;
    n = this->order;
    ll options_size = 0;
    ll options;
    options = getAllXORStrings(seq, db_seq, options_size);
    return options;
}
#pragma omp end declare target

const vector<pair<string, ll>> &ComplexityToDebruijn::getSubseqToDebruijn() const {
    return subseq_to_debruijn;
}

ll ComplexityToDebruijn::getTotalSeqNum() const {
    return total_seq_num;
}

ComplexityToDebruijn::~ComplexityToDebruijn() {
    delete [] up_to_1000;
}
