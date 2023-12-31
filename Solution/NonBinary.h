//
// Created by Razan on 11/12/2023.
//

#ifndef DEBRUIJN_SEQUENCE_COMPLEXITY_NONBINARY_H
#define DEBRUIJN_SEQUENCE_COMPLEXITY_NONBINARY_H
#include "vector"
#include "string"

using namespace std;

class NonBinary {
    int field = 3;
    int order = 2;
    int complexity;

public:
    NonBinary(int field, int order, int complexity);
    void compute();
    int getOrder() const;
    int getComplexity() const;
    int getField() const;
};


#endif //DEBRUIJN_SEQUENCE_COMPLEXITY_NONBINARY_H
