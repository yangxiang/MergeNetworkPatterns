#ifndef _NETPATTERN_H
#define _NETPATTERN_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include "GraphMatrix.h"


using namespace std;

class NetPattern{
	private:
		vector<int> bipartite_A;
		vector<int> bipartite_B;
		WeightMeasure weight;
		float p_value;
	public:
	void set_pvalue(const float&);
	const float& get_pvalue() const;
	NetPattern();
	NetPattern(const set<int>&, const set<int>&, const Graph&);
	NetPattern(const set<int>&, const set<int>&, const Graph&, const float&);
	~NetPattern();
	float getDensity() const;
	WeightMeasure calculate_weight(const Graph& g) const;
	void printPattern(ofstream& outfile) const;
	friend void ::patternMerge(const NetPattern& i, const NetPattern& j, NetPattern& merged_pattern, const Graph& g);
	friend float ::calculate_merged_density(const NetPattern& i, const NetPattern& j, const Graph& g);
};

inline
void NetPattern::set_pvalue(const float& x){
	p_value=x;
}

inline
const float& NetPattern::get_pvalue() const{
	return p_value;
}
#endif
