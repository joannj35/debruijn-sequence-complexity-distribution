#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <unordered_map>

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

inline static constexpr int MIN_ORDER = 2;

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

class DebruijnGraph
{
public:

	DebruijnGraph() :
		_alphabet{ "" },
		_order{ -1 },
		_vid_source{ 0 },
		_eid_source{ 0 },
		_compute_and_rotate{ true } { }

	std::string GetAlphabet() const { return _alphabet; }
	void SetAlphabet(std::string_view alphabet) { _alphabet = alphabet; }
	int GetOrder() const { return _order; }
	void SetOrder(int order) { _order = order; }
	size_t GetNumberOfVertices() const { return _vertices.size(); }
	size_t GetNumberOfEdges() const { return _edges.size(); }
	void SetComputeAndRotate(bool flag) { _compute_and_rotate = flag; }

	void InitGraph();

	std::vector<std::string> HamiltonianPaths(std::string_view starting_subsequence, int limit);
	std::vector<std::string> EulerCycles(std::string_view starting_subsequence, int limit);

private:

	std::string _alphabet;
	int _order;
	std::vector<std::unique_ptr<Vertex>> _vertices;
	std::vector<std::unique_ptr<Edge>>   _edges;
	bool _compute_and_rotate;

	int _vid_source;
	int _eid_source;

	void ComputeVertices(std::string& seq);
	void ComputeEdges();
	void ClearVertexFlags();
	void ClearEdgeFlags();

	Vertex* FindVertex(std::string_view seq);
	std::string ExtractDebruijnSequence(const std::vector<Vertex*>& vertices);

	void HamiltonianPaths(Vertex* v, std::vector<std::string>& sequences, int limit);
	void ComputeHamiltonianPaths(Vertex* v, std::vector<Vertex*>& vertices, std::vector<std::string>& sequences, int limit);

	void EulerCycles(DebruijnGraph* dg, Vertex* v, std::vector<std::string>& sequences, int limit);
	void ComputeEulerCycles(Vertex* v, std::vector<std::string>& sequences, std::string& seq, int limit);

	static bool IsRotation(const std::string& s1, std::string s2);
	static void GenerateRotations(std::string seq, std::vector<std::string>& sequences, int count);
	static std::unordered_map<int, std::vector<std::string>> FindRotationGroups(const std::vector<std::string>& sequences);

	friend std::ostream& operator<<(std::ostream& os, const DebruijnGraph& dg);
};