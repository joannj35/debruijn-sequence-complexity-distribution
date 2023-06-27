#include "SequenceGenerator.h"
#include <iostream>
#include <vector>

using namespace std;

int main(){
    SequenceGenerator seq(8, 7);
    auto s = seq.getSequences();
    for (const auto& i : s) {
        cout << i << endl;
    }
}