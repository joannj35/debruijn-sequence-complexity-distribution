//
// Created by Razan on 11/12/2023.
//

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "NonBinary.h"
#include "stdlib.h"
#include "math.h"

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

static vector<string> generateSequences(int p, int order){
    vector<string> sequences;
    make_seq("", p, order, sequences);
    return sequences;
}

static vector<vector<int>> findSolutions(int field, int sol) {
    vector<vector<int>> solutions;
    for (int x = 0; x < field; ++x) {
        for (int y = 0; y < field; ++y) {
            for (int z = 0; z < field; ++z) {
                if ((x - 2*y + z + (field*field)) % field == sol){
                    if(field == 3){
                        solutions.push_back({x, y, z});
                    }
                    if (field == 5 || field == 7){
                        for (int w = 0; w < field; ++w) {
                            if((y - 2*z + w + (field*field)) % field == sol){
                                for(int v = 0; v < field; ++v){
                                    if((z - 2*w + v + (field*field)) % field == sol){
                                        if(field == 5)
                                            solutions.push_back({x, y, z, w, v});
                                        else {
                                            for (int u = 0; u < field; ++u) {
                                                if ((w - 2 * v + u + (field * field)) % field == sol) {
                                                    for (int i = 0; i < field; ++i) {
                                                        if ((v - 2 * u + i + (field * field)) % field == sol)
                                                            if (field == 7)
                                                                solutions.push_back({x, y, z, w, v, u, i});
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                }
            }
        }
    }
    return solutions;
}

// Convert a pair of integers to a string
static string pairToString(int a, int b) {
    return std::to_string(a) + std::to_string(b);
}

static bool isDeBruijnSequence(const std::vector<int>& sequence, int p) {
    if (sequence.size() != p * p) return false;

    std::set<std::string> substrings;

    for (size_t i = 0; i < sequence.size(); ++i) {
        std::string sub = pairToString(sequence[i], sequence[(i + 1) % sequence.size()]);
        if (substrings.find(sub) != substrings.end()) {
            return false;
        }
        substrings.insert(sub);
    }

    return substrings.size() == p * p;
}

static void generateSequences(vector<vector<int>>& sequences, vector<int> sequence, int field, int index, vector<vector<int>> &solutions) {
    if (index == field) {
        if (isDeBruijnSequence(sequence, field)) {
            sequences.push_back(sequence);
        }

        return;
    }
    for (auto sol : solutions) {
        if(index == 0 && sol[0] != 0) continue;
        if(index == 1 && sol[0] != 0) continue;
        for (int i = 0; i < field; ++i) {
            sequence[index + i*field] = sol[i];
        }
        generateSequences(sequences, sequence, field, index + 1, solutions);

    }
}


void NonBinary::compute() {
    std::string fileName = "sequences_of_field_"+ to_string(field) +"_complexity_"+ to_string(complexity) +".txt";
    std::ofstream fileout(fileName);
    for (int i = 1; i < field; ++i) {
        vector<vector<int>> solutions = findSolutions(field, i);
        vector<vector<int>> sequences;
        generateSequences(sequences, vector<int>(field * field), field, 0, solutions);
        for (auto &sol : sequences) {
            for (auto &x : sol) {
                fileout << x ;
            }
            fileout << endl;
        }
    }
}

