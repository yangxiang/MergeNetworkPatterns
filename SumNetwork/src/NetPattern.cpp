#include "NetPattern.h"

NetPattern::NetPattern(){
	p_value=1;
}

NetPattern::NetPattern(const set<int>& A, const set<int>& B, const Graph& g){
		p_value=1;
		weight=0;
		for(set<int>::iterator ita=A.begin(); ita!=A.end(); ita++){
			bipartite_A.push_back(*ita);
		}
		
		for(set<int>::iterator itb=B.begin(); itb!=B.end(); itb++){
			bipartite_B.push_back(*itb);
		}
		
		for(vector<int>::const_iterator ita=bipartite_A.begin(); ita<bipartite_A.end(); ita++){
			for(vector<int>::const_iterator itb=bipartite_B.begin(); itb<bipartite_B.end(); itb++){
				weight=weight+g.get_weight(*ita, *itb);			
			}
		}
}

NetPattern::NetPattern(const set<int>& A, const set<int>& B, const Graph& g, const float& pvalue){//Same as the above except that p_value=pvalue;
		p_value=pvalue;
		weight=0;
		for(set<int>::iterator ita=A.begin(); ita!=A.end(); ita++){
			bipartite_A.push_back(*ita);
		}
		
		for(set<int>::iterator itb=B.begin(); itb!=B.end(); itb++){
			bipartite_B.push_back(*itb);
		}
		
		for(vector<int>::const_iterator ita=bipartite_A.begin(); ita<bipartite_A.end(); ita++){
			for(vector<int>::const_iterator itb=bipartite_B.begin(); itb<bipartite_B.end(); itb++){
				weight=weight+g.get_weight(*ita, *itb);			
			}
		}
}

NetPattern::~NetPattern(){
	bipartite_A.clear();
	bipartite_B.clear();
}

void NetPattern::printPattern(ofstream& outfile) const{
	for(vector<int>::const_iterator cit=bipartite_A.begin(); cit<bipartite_A.end(); cit++)
		outfile<<*cit<<" ";
	outfile<<"; ";
	for(vector<int>::const_iterator cit=bipartite_B.begin(); cit<bipartite_B.end(); cit++)
		outfile<<*cit<<" ";
	outfile<<"; density: "<<getDensity();
	
}

float NetPattern::getDensity() const{
	return weight*1.0/(bipartite_A.size()*bipartite_B.size());
}

WeightMeasure NetPattern::calculate_weight(const Graph& g) const{
	WeightMeasure total_weight=0;
	for(vector<int>::const_iterator itA=bipartite_A.begin(); itA<bipartite_A.end(); itA++)
		for(vector<int>::const_iterator itB=bipartite_B.begin(); itB<bipartite_B.end(); itB++)
			total_weight=total_weight+g.get_weight(*itA,*itB);
	return total_weight;
}

void patternMerge(const NetPattern& i, const NetPattern& j, NetPattern& merged_pattern, const Graph& g){
//assuming all vertices in a network pattern is sorted from small to large
	vector<int> alpha, beta, i_A_excl, i_B_excl, j_A_excl, j_B_excl;
	vector<int>::const_iterator itA=i.bipartite_A.begin();
	vector<int>::const_iterator jtA=j.bipartite_A.begin();
	while(itA<i.bipartite_A.end() && jtA<j.bipartite_A.end()){
		if(*itA<*jtA){
			merged_pattern.bipartite_A.push_back(*itA);
			i_A_excl.push_back(*itA);
			itA++;
		}else if (*itA>*jtA){
			merged_pattern.bipartite_A.push_back(*jtA);
			j_A_excl.push_back(*jtA);
			jtA++;
		}else{//*itA==*jtA
			merged_pattern.bipartite_A.push_back(*itA);
			alpha.push_back(*itA);
			itA++;
			jtA++;

		}
	}
	
	//At most one of the following for loop will take effective.
	for(; itA<i.bipartite_A.end(); itA++){
		merged_pattern.bipartite_A.push_back(*itA);
		i_A_excl.push_back(*itA);
	}
	for(; jtA<j.bipartite_A.end(); jtA++){
		merged_pattern.bipartite_A.push_back(*jtA);
		j_A_excl.push_back(*jtA);
	}

	vector<int>::const_iterator itB=i.bipartite_B.begin();
	vector<int>::const_iterator jtB=j.bipartite_B.begin();
	while(itB<i.bipartite_B.end() && jtB<j.bipartite_B.end()){
		if(*itB<*jtB){
			merged_pattern.bipartite_B.push_back(*itB);
			i_B_excl.push_back(*itB);
			itB++;
		}else if (*itB>*jtB){
			merged_pattern.bipartite_B.push_back(*jtB);
			j_B_excl.push_back(*jtB);
			jtB++;
		}else{//*itB==*jtB
			merged_pattern.bipartite_B.push_back(*itB);
			beta.push_back(*itB);
			itB++;
			jtB++;
		}
	}
	
	//At most one of the following for loop will take effective.
	for(; itB<i.bipartite_B.end(); itB++){
		merged_pattern.bipartite_B.push_back(*itB);
		i_B_excl.push_back(*itB);
	}
	for(; jtB<j.bipartite_B.end(); jtB++){
		merged_pattern.bipartite_B.push_back(*jtB);
		j_B_excl.push_back(*jtB);
	}
	
	merged_pattern.weight=i.weight+j.weight-g.get_weight(alpha,beta)+g.get_weight(i_A_excl, j_B_excl)+g.get_weight(j_A_excl, i_B_excl);
//The following is debugging code
	/*	if(merged_pattern.weight!=merged_pattern.calculate_weight(g)){
		cout<<"weight by fast update: "<<merged_pattern.weight<<endl;
		cout<<"weight in fact: "<<merged_pattern.calculate_weight(g)<<endl;
		
		
		cout<<"i.bipartite_A: ";
		for(vector<int>::const_iterator tmp_it=i.bipartite_A.begin(); tmp_it<i.bipartite_A.end(); tmp_it++)
			cout<<*tmp_it<<" ";
		cout<<endl;		
		
		cout<<"i_A_excl: ";
		for(vector<int>::const_iterator tmp_it=i_A_excl.begin(); tmp_it<i_A_excl.end(); tmp_it++)
			cout<<*tmp_it<<" ";
		cout<<endl;
		
		cout<<"alpha: ";
		for(vector<int>::const_iterator tmp_it=alpha.begin(); tmp_it<alpha.end(); tmp_it++)
			cout<<*tmp_it<<" ";
		cout<<endl;
		
		cout<<"j_A_excl: ";
		for(vector<int>::const_iterator tmp_it=j_A_excl.begin(); tmp_it<j_A_excl.end(); tmp_it++)
			cout<<*tmp_it<<" ";
		cout<<endl;
		
		cout<<"j.bipartite_A: ";
		for(vector<int>::const_iterator tmp_it=j.bipartite_A.begin(); tmp_it<j.bipartite_A.end(); tmp_it++)
			cout<<*tmp_it<<" ";
		cout<<endl;		
		
		cout<<"merged_pattern.bipartite_A: ";
		for(vector<int>::const_iterator tmp_it=merged_pattern.bipartite_A.begin(); tmp_it<merged_pattern.bipartite_A.end(); tmp_it++)
			cout<<*tmp_it<<" ";
		cout<<endl;
		
		cout<<"i.bipartite_B: ";
		for(vector<int>::const_iterator tmp_it=i.bipartite_B.begin(); tmp_it<i.bipartite_B.end(); tmp_it++)
			cout<<*tmp_it<<" ";
		cout<<endl;
		
		cout<<"i_B_excl: ";
		for(vector<int>::const_iterator tmp_it=i_B_excl.begin(); tmp_it<i_B_excl.end(); tmp_it++)
			cout<<*tmp_it<<" ";
		cout<<endl;
		
		cout<<"beta: ";
		for(vector<int>::const_iterator tmp_it=beta.begin(); tmp_it<beta.end(); tmp_it++)
			cout<<*tmp_it<<" ";
		cout<<endl;
		
		cout<<"j_B_excl: ";
		for(vector<int>::const_iterator tmp_it=j_B_excl.begin(); tmp_it<j_B_excl.end(); tmp_it++)
			cout<<*tmp_it<<" ";
		cout<<endl;
		
		cout<<"j.bipartite_B: ";
		for(vector<int>::const_iterator tmp_it=j.bipartite_B.begin(); tmp_it<j.bipartite_B.end(); tmp_it++)
			cout<<*tmp_it<<" ";
		cout<<endl;		
		
		cout<<"merged_pattern.bipartite_B: ";
		for(vector<int>::const_iterator tmp_it=merged_pattern.bipartite_B.begin(); tmp_it<merged_pattern.bipartite_B.end(); tmp_it++)
			cout<<*tmp_it<<" ";
		cout<<endl;
		
		cout<<"i.weight: "<<i.weight<<endl;
		cout<<"get_weight(i): "<<g.get_weight(i.bipartite_A, i.bipartite_B)<<endl;
		cout<<"j.weight: "<<j.weight<<endl;
		cout<<"get_weight(j): "<<g.get_weight(j.bipartite_A, j.bipartite_B)<<endl;
		cout<<"weight(alpha,beta): "<<g.get_weight(alpha,beta)<<endl;
		cout<<"weight(i_A_excl, j_B_excl)"<<g.get_weight(i_A_excl, j_B_excl) <<endl;
		cout<<"weight(j_A_excl, i_B_excl)"<<g.get_weight(j_A_excl, i_B_excl) <<endl;
		
		exit(-1);
	}
*/	
}

float calculate_merged_density(const NetPattern& i, const NetPattern& j, const Graph& g){
//assuming all vertices in a network pattern is sorted from small to large
	vector<int> alpha, beta, i_A_excl, i_B_excl, j_A_excl, j_B_excl;
	vector<int>::const_iterator itA=i.bipartite_A.begin();
	vector<int>::const_iterator jtA=j.bipartite_A.begin();
	while(itA<i.bipartite_A.end() && jtA<j.bipartite_A.end()){
		if(*itA<*jtA){
			i_A_excl.push_back(*itA);
			itA++;
		}else if (*itA>*jtA){
			j_A_excl.push_back(*jtA);
			jtA++;
		}else{//*itA==*jtA
			alpha.push_back(*itA);
			itA++;
			jtA++;

		}
	}
	
	//At most one of the following for loop will take effective.
	for(; itA<i.bipartite_A.end(); itA++){
		i_A_excl.push_back(*itA);
	}
	for(; jtA<j.bipartite_A.end(); jtA++){
		j_A_excl.push_back(*jtA);
	}

	vector<int>::const_iterator itB=i.bipartite_B.begin();
	vector<int>::const_iterator jtB=j.bipartite_B.begin();
	while(itB<i.bipartite_B.end() && jtB<j.bipartite_B.end()){
		if(*itB<*jtB){
			i_B_excl.push_back(*itB);
			itB++;
		}else if (*itB>*jtB){
			j_B_excl.push_back(*jtB);
			jtB++;
		}else{//*itB==*jtB
			beta.push_back(*itB);
			itB++;
			jtB++;
		}
	}
	
	//At most one of the following for loop will take effective.
	for(; itB<i.bipartite_B.end(); itB++){
		i_B_excl.push_back(*itB);
	}
	for(; jtB<j.bipartite_B.end(); jtB++){
		j_B_excl.push_back(*jtB);
	}
	
	float new_density=(i.weight+j.weight-g.get_weight(alpha,beta)+g.get_weight(i_A_excl, j_B_excl)+g.get_weight(j_A_excl, i_B_excl))*1.0/((i.bipartite_A.size()+j.bipartite_A.size()-alpha.size())*(i.bipartite_B.size()+j.bipartite_B.size()-beta.size()));
	//cout<<"new density: "<<new_density<<endl;
	return new_density;
}
