#include "SequenceGenerator.h"
#include <iostream>
#include <vector>
#include "ComplexityToDebruijn.h"
#include <fstream>
#include <chrono>
#include <omp.h>
using namespace std::chrono;
using namespace std;

int main(){
    omp_set_num_threads(24);
    cout << "Starting..." << endl;
    /*auto start = high_resolution_clock::now();
    SequenceGenerator se(21);
    auto d = se.getSequences();
    cout << se.getNumOfSeq() << endl;
    for (int i = 0; i < d.size(); ++i) {
        cout << d[i] << endl;
    }
    auto end = high_resolution_clock::now();
    auto duration= duration_cast<seconds>(end - start);
    cout << duration.count() << " seconds" << endl;
    return 0;*/
    int order = 6;
    for(int c = 38; c <= 63; c++){
        auto start = high_resolution_clock::now();

        int complexity = c;
        ll total = 0;
        ComplexityToDebruijn C(complexity,order);
        C.compute();
    }
    cout << "Done with all complexities!!!" << endl;
}