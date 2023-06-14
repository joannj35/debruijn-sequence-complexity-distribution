#include "DeBruijnGraph.h"

#include <iostream>
#include <algorithm>

void DebruijnGraph::InitGraph()
{
    _vertices.clear();
    _edges.clear();
    _vid_source = 0;
    _eid_source = 0;
    std::string sub_sequence{ "" };
    ComputeVertices(sub_sequence);
    ComputeEdges();
}

std::vector<std::string> DebruijnGraph::HamiltonianPaths(std::string_view starting_subsequence, int limit)
{
    std::vector<std::string> debruijn_sequences;

    auto v = FindVertex(starting_subsequence);
    if (v)
    {
        HamiltonianPaths(v, debruijn_sequences, limit);
    }
    else
    {
        if (_compute_and_rotate)
        {
            v = _vertices[0].get();
            HamiltonianPaths(v, debruijn_sequences, limit);

            int count = std::numeric_limits<int>::max();
            size_t sz = debruijn_sequences.size();
            for (int i = 0; i < sz; i++)
            {
                if (debruijn_sequences.size() == limit)
                    break;

                if (limit > 0 && limit > debruijn_sequences.size())
                    count = limit - debruijn_sequences.size();

                GenerateRotations(debruijn_sequences[i], debruijn_sequences, count);
            }
        }
        else
        {
            for (auto& vt : _vertices)
                HamiltonianPaths(vt.get(), debruijn_sequences, limit);
        }
    }

    return debruijn_sequences;
}

std::vector<std::string> DebruijnGraph::EulerCycles(std::string_view starting_subsequence, int limit)
{
    auto dg = std::make_unique<DebruijnGraph>();
    dg->SetOrder(_order - 1);
    dg->SetAlphabet(_alphabet);
    dg->InitGraph();

    std::vector<std::string> debruijn_sequences;

    Vertex* v = nullptr;
    if (starting_subsequence.size() > (size_t)(_order - 1))
    {
        auto ss = starting_subsequence.substr(0, _order - 1);
        v = FindVertex(starting_subsequence);
        EulerCycles(dg.get(), v, debruijn_sequences, limit);
    }
    else
    {
        for (auto& vt : _vertices)
            EulerCycles(dg.get(), vt.get(), debruijn_sequences, limit);
    }

    return debruijn_sequences;
}

std::unordered_map<int, std::vector<std::string>> DebruijnGraph::FindRotationGroups(const std::vector<std::string>& sequences)
{
    std::unordered_map<int, std::vector<std::string>> groups;
    int group_num{ 0 };
    for (const auto& seq : sequences)
    {
        bool group_found{ false };
        for (const auto& kvp : groups)
        {
            if (IsRotation(kvp.second[0], seq))
            {
                group_found = true;
                groups[kvp.first].push_back(seq);
                break;
            }
        }

        if (!group_found)
            groups[++group_num] = std::vector<std::string>{ seq };
    }

    return groups;
}

void DebruijnGraph::ComputeVertices(std::string& seq)
{
    if (seq.size() == _order)
    {
        _vertices.emplace_back(std::make_unique<Vertex>(_vid_source++, seq));
        return;
    }

    for (char s : _alphabet)
    {
        seq.push_back(s);
        ComputeVertices(seq);
        seq.pop_back();
    }
}

void DebruijnGraph::ComputeEdges()
{
    for (auto& curr_v : _vertices)
    {
        auto sw = static_cast<std::string_view>(curr_v->sequence).substr(1);

        for (auto& v : _vertices)
        {
            if (v->sequence.starts_with(sw))
            {
                _edges.emplace_back(std::make_unique<Edge>(_eid_source, curr_v->id, v->id, v->sequence.back()));
                curr_v->out_edges.push_back(_eid_source);
                v->in_edges.push_back(_eid_source);
                _eid_source++;
            }
        }
    }
}

void DebruijnGraph::ClearVertexFlags()
{
    std::for_each(_vertices.begin(), _vertices.end(), [](auto& vp) { vp->flag = false; });
}

void DebruijnGraph::ClearEdgeFlags()
{
    std::for_each(_edges.begin(), _edges.end(), [](auto& ep) { ep->flag = false; });
}

void DebruijnGraph::HamiltonianPaths(Vertex* v, std::vector<std::string>& sequences, int limit)
{
    if (limit > 0 && sequences.size() == limit)
        return;

    std::vector<Vertex*> vertices;
    ClearVertexFlags();
    ComputeHamiltonianPaths(v, vertices, sequences, limit);
}

void DebruijnGraph::ComputeHamiltonianPaths(Vertex* v, std::vector<Vertex*>& vertices, std::vector<std::string>& sequences, int limit)
{
    if (limit > 0 && sequences.size() == limit)
        return;

    v->flag = true;
    vertices.push_back(v);

    if (vertices.size() == _vertices.size())
    {
        sequences.emplace_back(ExtractDebruijnSequence(vertices));
    }
    else
    {
        for (const auto eid : v->out_edges)
        {
            if (_vertices[_edges[eid]->target_id]->flag)
                continue;

            ComputeHamiltonianPaths(_vertices[_edges[eid]->target_id].get(), vertices, sequences, limit);
        }
    }

    vertices.pop_back();
    v->flag = false;
}

std::string DebruijnGraph::ExtractDebruijnSequence(const std::vector<Vertex*>& vertices)
{
    int sequence_length = std::pow(_alphabet.size(), _order);
    std::string debruijn{ vertices[0]->sequence };
    for (int i = _order, j = 1; i < sequence_length; i++, j++)
        debruijn.push_back(vertices[j]->sequence.back());

    return debruijn;
}

bool DebruijnGraph::IsRotation(const std::string& s1, std::string s2)
{
    if (s1.size() != s2.size())
        return false;

    if (s1 == s2)
        return true;

    for (size_t i = 0; i < s2.size(); i++)
    {
        std::rotate(s2.begin(), s2.begin() + 1, s2.end());
        if (s1 == s2)
            return true;
    }

    return false;
}

void DebruijnGraph::EulerCycles(DebruijnGraph* dg, Vertex* v, std::vector<std::string>& sequences, int limit)
{
    if (limit > 0 && sequences.size() == limit)
        return;

    std::string seq{ "" };
    dg->ClearEdgeFlags();
    dg->ComputeEulerCycles(v, sequences, seq, limit);
}

void DebruijnGraph::ComputeEulerCycles(Vertex* v, std::vector<std::string>& sequences, std::string& seq, int limit)
{
    if (limit > 0 && sequences.size() == limit)
        return;

    if (seq.size() == std::pow(_alphabet.size(), _order + 1))
        sequences.push_back(seq);

    for (auto eid : v->out_edges)
    {
        if (_edges[eid]->flag)
            continue;

        _edges[eid]->flag = true;
        seq.push_back(_edges[eid]->append);
        ComputeEulerCycles(_vertices[_edges[eid]->target_id].get(), sequences, seq, limit);
        _edges[eid]->flag = false;
        seq.pop_back();
    }
}

void DebruijnGraph::GenerateRotations(std::string seq, std::vector<std::string>& sequences, int count)
{
    auto num_rotations = seq.size() - 1;
    if (count < num_rotations)
        num_rotations = count;

    for (auto i = 0; i < num_rotations; i++)
    {
        std::rotate(seq.begin(), seq.begin() + 1, seq.end());
        sequences.push_back(seq);
    }
}

Vertex* DebruijnGraph::FindVertex(std::string_view seq)
{
    if (seq.empty())
        return nullptr;

    auto it = std::find_if(_vertices.begin(), _vertices.end(), [seq](const auto& vp) { return vp->sequence == seq; });
    return it == _vertices.end() ? nullptr : it->get();
}

std::ostream& operator<<(std::ostream& os, const DebruijnGraph& dg)
{
    os << "Debruijn graph of order: " << dg._order << " for the alphabet: " << dg._alphabet << std::endl;
    os << "# of Vertices: " << dg._vertices.size() << std::endl;
    os << "# of Edges: " << dg._edges.size() << std::endl;
    os << "Vertices: " << std::endl;
    for (const auto& v : dg._vertices)
    {
        os << v->id << ": " << v->sequence << std::endl;
        os << "\t out edges:";
        for (int i : v->out_edges)
            os << "\t" << dg._vertices[dg._edges[i]->target_id]->sequence;
        os << std::endl;

        os << "\t in edges :";
        for (int i : v->in_edges)
            os << "\t" << dg._vertices[dg._edges[i]->source_id]->sequence;
        os << std::endl;
    }
    os << std::endl;
    return os;
}