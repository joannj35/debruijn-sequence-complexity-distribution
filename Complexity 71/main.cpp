#include "SequenceGenerator.h"
#include <iostream>
#include <vector>
#include "ComplexityToDebruijn.h"

using namespace std;


int main(){
    ComplexityToDebruijn C(7, 7);
    C.fromSubseqToDebruijn("00000011");
//    SequenceGenerator seq(7);
//    auto s = seq.getSequences();
//    for (const auto& i : s) {
//        cout << i << endl;
//    }
//    cout << seq.getNumOfSeq() << endl;
}