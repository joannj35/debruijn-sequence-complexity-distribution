#include "SequenceGenerator.h"
#include <iostream>
#include <vector>
#include "ComplexityToDebruijn.h"

using namespace std;


int main(){
    ComplexityToDebruijn C(6, 6);
//
    SequenceGenerator seq(6);
    auto s = seq.getSequences();
    for (const auto& i : s) {
        cout << i << " : ";
        cout << C.fromSubseqToDebruijn(i+i+i+i) << endl;
    }
    cout << seq.getNumOfSeq() << endl;
}