#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
struct Vertex
{
    Vertex(int vid, std::string_view seq) : id{ vid }, sequence{ seq }, flag{ false } { }

    bool flag;
    int id;
    std::string sequence;
    std::vector<int> out_edges;
    std::vector<int> in_edges;
};

struct Edge
{
    Edge(int eid, int sid, int tid, char c) :
            id{ eid }, source_id{ sid }, target_id{ tid }, append{ c }, flag{ false } { }

    bool flag;
    int id;
    int source_id;
    int target_id;
    char append;
};
//
//void create()

typedef std::pair<std::string, std::string> StringPair;

bool isCompatible(const StringPair& pair) {

    return true; // Placeholder, replace with your restriction logic
}

// Recursive function to generate combinations
void generateCombinations(std::vector<StringPair>& pairs, int index, int count, std::vector<StringPair>& combination, std::vector<std::vector<StringPair>>& validCombinations) {
    if (count == 0) {
        // Check compatibility of the combination
        bool isValid = true;
        for (const StringPair& pair : combination) {
            if (!isCompatible(pair)) {
                isValid = false;
                break;
            }
        }

        if (isValid) {
            validCombinations.push_back(combination);
        }
        return;
    }

    if (index >= pairs.size()) {
        return;
    }

    // Include current pair in the combination
    combination.push_back(pairs[index]);
    generateCombinations(pairs, index + 1, count - 1, combination, validCombinations);

    // Exclude current pair from the combination
    combination.pop_back();
    generateCombinations(pairs, index + 1, count, combination, validCombinations);
}

