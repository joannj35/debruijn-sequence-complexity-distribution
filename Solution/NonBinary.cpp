//
// Created by Razan on 11/12/2023.
//

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <algorithm>
#include "NonBinary.h"
#include <cmath>
#include <map>

using namespace std;

NonBinary::NonBinary(int field, int order, int complexity) : field(field), order(order), complexity(complexity) {}

int NonBinary::getOrder() const {
    return order;
}

int NonBinary::getComplexity() const {
    return complexity;
}

int NonBinary::getField() const {
    return field;
}

static int checkComplexityNonBinary(const std::string& S, int m, int p) {
    if (m == 0){
        return 1;
    }
    bool allzeros = std::all_of(S.begin(), S.end(), [](char c) {
        return c == '0';
    });
    if(allzeros){
        return 0;
    }
    int c_m = pow(p,m-1);
    string A_1;
    string B_m = S; // of length p^m
    // split B_m into p substrings of length p^(m-1) take the first substring as A_1
    A_1 = B_m.substr(0, c_m);
    string B_m_aux = B_m.substr(c_m) + B_m.substr(0, c_m); // cyclic shift of B_m (of length p^m)
    string D_m;
    int D_m_int = 0;

    for (int i = 0; i < B_m.size(); ++i){
        //cout << B_m_aux[i] << " " << B_m[i] << " " << (B_m_aux[i] - B_m[i] + p) % p << endl;
        D_m_int += (B_m_aux[i] - B_m[i] + p) % p;
        D_m += to_string((B_m_aux[i] - B_m[i] + p) % p);
    }
    if (D_m_int == 0){
        return checkComplexityNonBinary(A_1, m - 1, p);
    } else{
        return checkComplexityNonBinary(D_m, m, p) + c_m;
    }
}

//generates all possible sequences of length p^order over the alphabet {0,1,...,p-1}
static void make_seq(string seq, int p, int order, vector<string>& sequences){
    if (seq.size() == pow(p, order)){
        sequences.push_back(seq);
        return;
    }
    for (int i = 0; i < p; i++){
        seq += to_string(i);
        make_seq(seq,p,order,sequences);
        seq.pop_back();
    }
}

static vector<string> generateSmallSequences(int p, int order, int complexity) {
    vector<string> sequences;
    vector<string> small_sequences;
    make_seq("", p, order, sequences);
    for (auto seq:sequences) {
        if (checkComplexityNonBinary(seq, order, p) == complexity) {
            small_sequences.push_back(seq);
        }
    }
    return small_sequences;
}

/***
 * compute the factorial of n
 * @param n
 * @return n!
 */
static unsigned long long factorial(int n) {
    if (n == 0)
        return 1;
    else
        return n * factorial(n - 1);
}

/***
 * compute the binomial coefficients of n
 * @param n
 * @return vector of coefficients
 */
static vector<unsigned long long> binomialCoefficients(int n) {
    vector<unsigned long long> coefficients;
    for (int k = 0; k <= n; k++) {
        unsigned long long coeff = factorial(n) / (factorial(k) * factorial(n - k));
        coefficients.push_back(coeff);
    }
    return coefficients;
}

static map<int, vector<string>> findSolutions_2p(int field, int n, string small_seq) {
    // Note: presume that n < field
    vector<string> sequences;
    map<int, vector<string>> solutions;
    make_seq("", field, 1, sequences);
    vector<int> variables(field);
    auto coefs = binomialCoefficients(n);
    for (auto seq:sequences) {
        for (int i = 0; i < small_seq.size(); ++i) {
            int sol = small_seq[i] - '0';
            int r = 0;
            bool sol_found = true;
            while (n+r < field && sol_found) {
                int sum = 0;
                for (int j = r; j <= n+r; ++j) {
                    variables[j] = (coefs[j-r] * (seq[j] - '0')) % field;
                    sum += pow(-1,j-r) * variables[j];
                }
                r++;
                if ((sum+(field*field)) % field == sol) {
                    sol_found = true;
                } else {
                    sol_found = false;
                    break;
                }
            }
            if (sol_found) {
                solutions[sol].push_back(seq);
                break;
            }
        }
    }
    return solutions;
}



//static map<int,vector<string>> findSolutions_2p(int field, string small_seq) {
//    vector<string> sequences;
//    map<int,vector<string>> solutions;
//    make_seq("",field,1,sequences);
//    for (auto seq:sequences) {
//        switch (field) {
//            case 3:
//                for (int i = 0; i < small_seq.size(); ++i) {
//                    int x = seq[0], y = seq[1], z = seq[2], sol = small_seq[i] - '0';
//                    if ((x - 2 * y + z + (field * field)) % field == sol) {
//                        solutions[sol].push_back(seq);
//                        break;
//                    }
//                }
//                break;
//            case 5:
//                for (int i = 0; i < small_seq.size(); ++i) {
//                    int x = seq[0], y = seq[1], z = seq[2], w = seq[3], v = seq[4], sol = small_seq[i] - '0';
//                    if ((x - 2 * y + z + (field * field)) % field == sol) {
//                        if ((y - 2 * z + w + (field * field)) % field == sol) {
//                            if ((z - 2 * w + v + (field * field)) % field == sol) {
//                                solutions[sol].push_back(seq);
//                                break;
//                            }
//                        }
//                    }
//                }
//                break;
//            case 7:
//                for (int i = 0; i < small_seq.size(); ++i) {
//                    int x = seq[0], y = seq[1], z = seq[2], w = seq[3], v = seq[4], u = seq[5], k = seq[6], sol =
//                            small_seq[i] - '0';
//                    if ((x - 2 * y + z + (field * field)) % field == sol) {
//                        if ((y - 2 * z + w + (field * field)) % field == sol) {
//                            if ((z - 2 * w + v + (field * field)) % field == sol) {
//                                if ((w - 2 * v + u + (field * field)) % field == sol) {
//                                    if ((v - 2 * u + k + (field * field)) % field == sol) {
//                                        solutions[sol].push_back(seq);
//                                        break;
//                                    }
//                                }
//                            }
//                        }
//                    }
//                }
//                break;
//            default:
//                cout << "field " << field << " is not supported" << endl;
//                exit(0);
//                break;
//        }
//
//    }
//    return solutions;
//}

static bool isDeBruijnSequence(const string& sequence, int p) {
    if (sequence.size() != p * p) return false;

    std::set<std::string> substrings;

    for (size_t i = 0; i < sequence.size(); ++i) {
        std::string sub = to_string(sequence[i])+ to_string(sequence[(i + 1) % sequence.size()]);
        if (substrings.find(sub) != substrings.end()) {
            return false;
        }
        substrings.insert(sub);
    }

    return substrings.size() == p * p;
}

///<summary>
/// generate all possible debruijn sequences of length p^order over the alphabet {0,1,...,p-1}
/// using the given small sequence
///</summary>
static void generateSequences(vector<string>& sequences, string sequence, int field, int index, map<int,vector<string>> &solutions, string small_sequence, map<string,bool> checkList, long long& num) {
    if (index == field) {
        if (sequence.size() < 1000)
            sequences.push_back(sequence);
        num++;
        return;
    }
    for (auto sol : solutions[small_sequence[index] - '0']) { //small_sequence[index] - '0' is the solution for the small sequence
        if(index == 0 && sol[0] != '0') continue;
        if(index == 1 && sol[0] != '0') continue;
        vector<string> reset;
        bool is_db = true;
        for (int i = 0; i < field; ++i) {
            sequence[index + i*field] = sol[i];

            if(index > 0){
                string sub = "";
                sub.push_back(sequence[index + i*field - 1]);
                sub.push_back(sequence[index + i*field]);
                if(checkList[sub]) {
                    for (auto &r: reset) {
                        checkList[r] = false;
                    }
                    is_db = false;
                    break;
                }
                checkList[sub] = true;
                reset.push_back(sub);
            }
            if(index == field - 1){
                string sub;
                if(i == field-1) {
                    sub.push_back(sequence[index + i * field]);
                    sub.push_back(sequence[0]);
                }else{//TODO check this
                    sub.push_back(sequence[index + i * field]);
                    sub.push_back(sequence[index + i*field + 1]);
                }
                if(checkList[sub]) {
                    for (auto &r: reset) {
                        checkList[r] = false;
                    }
                    is_db = false;
                    break;
                }
                checkList[sub] = true;
                reset.push_back(sub);
            }
        }
        if (is_db){
            generateSequences(sequences, sequence, field, index + 1, solutions, small_sequence, checkList, num);
            for (auto &r: reset) {
                checkList[r] = false;
            }
        }
    }
}

/// <summary>
/// generate a map of all possible sequences of length=order over the alphabet {0,1,...,field-1}
/// the value of each sequence is false by default
/// this map will be used to check if a sequence is debruijn or not
/// </summary>
static map<string,bool> debriujnCheckList(int field){
    map<string,bool> checkList;
    for(int i = 0; i<field ; i++){
        for(int j = 0; j<field ; j++){
            string seq = to_string(i) + to_string(j);
            checkList[seq] = false;
        }
    }
    return checkList;
}

void NonBinary::compute() {
    //TODO: add omp
    std::ofstream fileout("field_"+ to_string(field)+"_span_"+ to_string(order) +"_complexity_"+ to_string(complexity) +".txt");
    int small_complexity = complexity%field, n = complexity/field;
    if (small_complexity == 0){
        small_complexity = field;
        n--;
    }
    auto small_sequences = generateSmallSequences(field, order-1, small_complexity); // 2*field is Hardcoded for now
    fileout << "For order " << order << " complexity " << complexity << ": " << endl << endl;
    long long total = 0;
    #pragma omp parallel for schedule(dynamic) shared(small_sequences,n,cout,fileout) reduction(+:total) default(none)
    for (auto &small_sequence : small_sequences) {
        map<int, vector<string>> solutions = findSolutions_2p(field, n, small_sequence);
        auto checkList = debriujnCheckList(field);
        vector<string> sequences;
        long long num = 0;
        generateSequences(sequences, string(pow(field, order), '0'), field, 0, solutions, small_sequence, checkList,num);
        #pragma omp critical
        {
            fileout << "Debruijn Sequences generated by the sequence " << small_sequence << " : " << endl;
            //cout << small_sequence << " : " << sequences.size() << endl;

        total += num;
        int limit = 0;
        for (auto &sol: sequences) {
            if (limit > 1000) break;
                fileout << sol << endl;
            limit++;
        }
            fileout << "the number of Debruijn sequences: " << num << endl << endl;
        };
    }
    fileout << "total number of sequences of small complexity " << small_complexity << " is: " << small_sequences.size() << endl;
    fileout << "total number of debruijn sequences of complexity " << complexity << " is: " << total << endl;
}

