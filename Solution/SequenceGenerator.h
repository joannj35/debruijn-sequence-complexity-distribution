#ifndef DEBRUIJN_SEQUENCE_COMPLEXITY_SEQUENCEGENERATOR_H
#define DEBRUIJN_SEQUENCE_COMPLEXITY_SEQUENCEGENERATOR_H

#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <cmath>
#include <map>
using namespace std;
// creates all sequences that have the given complexity

class SequenceGenerator {
    int seq_len;
    int complexity;
    vector<string> sequences;
    int num_of_seq{};
    vector<char> alphabet;
    void generateSequences();
    void generatePermutations(const string& current);
public:
    int getNumOfSeq() const;

    explicit SequenceGenerator(int complexity, const vector<char> &alphabet = {'0', '1'}) : seq_len(pow(2,ceil(log2(complexity)))),
                                                                                  complexity(complexity),
                                                                                  alphabet(alphabet), num_of_seq(0) {
        this->generateSequences();
    }
    const vector<string> &getSequences() const;
};


#endif //DEBRUIJN_SEQUENCE_COMPLEXITY_SEQUENCEGENERATOR_H
