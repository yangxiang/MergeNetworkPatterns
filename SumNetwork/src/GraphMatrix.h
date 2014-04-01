#ifndef _GRAPH_H
#define _GRAPH_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include <cmath>

using namespace std;

#define MAX_VAL 10000000
#define MIN_VAL -10000000
//#define WEIGHT_CUTOFF 0.5

struct Vertex {
	bool visited;
};

typedef float WeightMeasure; //for handling weighed graphs in matrix representation
typedef vector<WeightMeasure> EdgeList;	// edge list represented by vertex id list
typedef vector<Vertex> VertexList;	// vertices list (store real vertex property) indexing by id
typedef vector<EdgeList> GRA;	// index graph

enum construction_type {S_ADJ_GRAPH=0, P_ADJ_GRAPH=1, MATRIX_GRAPH=2};

class Graph {
	private:
		GRA graph;
		VertexList vl;
		int vsize;
		int col_size;
		int col_base;
		float WEIGHT_CUTOFF;
	public:
		Graph();
		Graph(int);
		Graph(GRA&, VertexList&);
		Graph(istream& in, construction_type& graph_format, const float& w_cutoff=0.5);
		~Graph();
		void readGraphSAdj(istream&);
		void readGraphPAdj(istream&);
		void readGraphMatrix(istream&);
		void writeGraphPADJ(ostream&);
		void writeGraphMatrix(ostream&);
		void printGraph();
		void printGraphMatrix();
		void addVertex(const size_t& vid);
		void addEdge(size_t, size_t);
		void addEdge(size_t, size_t, const WeightMeasure&);
		const int& num_vertices();
		const int& num_cols();
		int num_edges();
		VertexList& vertices();
		EdgeList& get_neighbors(int);
		int get_degree(int);
		const WeightMeasure& get_weight(const int&, const int&) const;
		WeightMeasure get_weight(const vector<int>& part_A, const vector<int>& part_B) const;
		bool hasEdge(int, int);	
		Graph& operator=(const Graph&);
		Vertex& operator[](const int);
		void clear();
		void strTrimRight(string& str);
};	

inline
const WeightMeasure& Graph::get_weight(const int& row, const int& col) const{//assuming both the input row and col start from 1. 
	return graph[row-1][col-col_base];
}

inline
WeightMeasure Graph::get_weight(const vector<int>& part_A, const vector<int>& part_B) const{//assuming both the input row and col start from 1. 
	WeightMeasure total_weight=0;
	for(vector<int>::const_iterator itA=part_A.begin(); itA<part_A.end(); itA++)
		for(vector<int>::const_iterator itB=part_B.begin(); itB<part_B.end(); itB++)
			total_weight=total_weight+get_weight(*itA,*itB);
	return total_weight;
}

inline
void Graph::addEdge(size_t sid, size_t tid) {
//	if (sid >= vl.size())
//		addVertex(sid);
//	if (tid >= vl.size())
//		addVertex(tid);
	// update edge list
	graph[sid][tid]=1;
}

inline
void Graph::addEdge(size_t sid, size_t tid, const WeightMeasure& edgeWeight){
	graph[sid][tid]=edgeWeight;
}

inline
const int& Graph::num_vertices() {
	return vsize;
}

inline
const int& Graph::num_cols() {
	return col_size;
}

// return neighbors of a specified vertex
inline
EdgeList& Graph::get_neighbors(int src) {
	return graph[src];
}

inline
int Graph::get_degree(int src) {
	return graph[src].size();
}

// return vertex list of graph
inline
VertexList& Graph::vertices() {
	return vl;
}

// get a specified vertex property
inline
Vertex& Graph::operator[](const int vid) {
	return vl[vid];
}
#endif
