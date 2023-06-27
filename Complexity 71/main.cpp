#include "SequenceGenerator.h"
#include <iostream>
#include <vector>

using namespace std;


int main(){
    SequenceGenerator seq(16, 16);
    auto s = seq.getSequences();
    for (const auto& i : s) {
        cout << i << endl;
    }
    cout << seq.getNumOfSeq() << endl;
}