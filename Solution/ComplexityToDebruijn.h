//
// Created by Razan on 28/06/2023.
//

#ifndef DEBRUIJN_SEQUENCE_COMPLEXITY_COMPLEXITYTODEBRUIJN_H
#define DEBRUIJN_SEQUENCE_COMPLEXITY_COMPLEXITYTODEBRUIJN_H
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <cmath>
#include <map>
#include "SequenceGenerator.h"

using namespace std;

typedef long long ll;

class ComplexityToDebruijn {
    int complexity;
    int order;
    int sub_complexity;
    ll total_seq_num;
    char* up_to_1000;
    vector<pair<string, ll>> subseq_to_debruijn; //a map that maps the number of De bruijn sequences(value) to the sub seq that it came from
    void generateXORStrings(const char* s, char*& a, char*& b, int index, char**& options, bool* check, char*& db_seq, ll& options_size);
    ll  getAllXORStrings(const char* s, char*& db_seq, ll& options_size);
public:
    ComplexityToDebruijn(int complexity, int order);
    void compute();

    virtual ~ComplexityToDebruijn();

    ll fromSubseqToDebruijn(const char* seq, char*& db_seq);
    static bool isRotation(const char* s1, const char* s2);

    ll getTotalSeqNum() const;

    const vector<pair<string, ll>> &getSubseqToDebruijn() const;

};


#endif //DEBRUIJN_SEQUENCE_COMPLEXITY_COMPLEXITYTODEBRUIJN_H
