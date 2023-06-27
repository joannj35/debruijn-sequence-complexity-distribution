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
    ll total_seq_num;
    map<string,ll> subseq_to_debruijn; //a map that maps the number of De bruijn sequences(value) to the sub seq that it came from
public:
    ComplexityToDebruijn(int complexity, int order);
    void compute();
    ll fromSubseqToDebruijn(string seq);


};


#endif //DEBRUIJN_SEQUENCE_COMPLEXITY_COMPLEXITYTODEBRUIJN_H
