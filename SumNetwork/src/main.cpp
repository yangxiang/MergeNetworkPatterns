#include "GraphMatrix.h"
#include "NetPattern.h"
#include "math.h"
#include <string>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <set>
#include <map>
#include <queue>
#include <algorithm>
using namespace std;


typedef struct merge_xy
{
	vector<NetPattern>::size_type x;
	vector<NetPattern>::size_type y;
}merge_xy;

typedef struct tree_node
{
	tree_node(): father(-1), sibling(-1), lchild(-1), rchild(-1), min_p(1), min_p_index(-1){};
	int father;
	int sibling;
	int lchild;
	int rchild;
	float min_p;//The minimum p-value of itself and all descendants;
	int min_p_index; //The index of the node with the above minimum p-value;
}tree_node;

void writePatterns(const vector<NetPattern>& network_patterns, ofstream& outfile){
	for(vector<NetPattern>::const_iterator cit=network_patterns.begin(); cit<network_patterns.end(); cit++){
		cit->printPattern(outfile);
		outfile<<endl;
	}
}

void writeMergedPatterns(const vector<NetPattern>& tmp_net_patterns, const vector<tree_node>& hierarchical_tree, ofstream& outfile_patterns){
	for(vector<NetPattern>::size_type i=0; i<tmp_net_patterns.size(); i++)
	{
		if(hierarchical_tree[i].sibling==-1){
			tmp_net_patterns[i].printPattern(outfile_patterns);
			outfile_patterns<<", min_p: "<<hierarchical_tree[i].min_p;
			outfile_patterns<<", min_p_index: "<<hierarchical_tree[i].min_p_index;
			outfile_patterns<<endl;
		}
	}
}

void BuildHierachicalTreeNum(const vector<NetPattern>& network_patterns, const size_t& num_clusters, const Graph& g, vector<tree_node>& hierarchical_tree, ofstream& outfile_patterns){//developed from mergeSingle
	hierarchical_tree.clear();
	hierarchical_tree.resize(network_patterns.size());
	vector<NetPattern> tmp_net_patterns=network_patterns;
	multimap<float, merge_xy> combo_list;
	for(vector<NetPattern>::size_type i=0; i<tmp_net_patterns.size(); i++){
		for(vector<NetPattern>::size_type j=0; j<i; j++){
			if (i==j)//must excluding self merging otherwise matching algorithm in the following will not work.
				continue;
			float merged_density=calculate_merged_density(tmp_net_patterns.at(i),tmp_net_patterns.at(j), g);
			merge_xy combo;
			combo.x=i;
			combo.y=j;
			combo_list.insert(pair<float,merge_xy>((-1)*merged_density, combo));
		}
		hierarchical_tree[i].min_p=tmp_net_patterns[i].get_pvalue();
		hierarchical_tree[i].min_p_index=i+1;//line start from 1;
	}
	
	
	
	int newly_appended_vertexID=tmp_net_patterns.size();
	bool network_pattern_combine_status=false;
	while (combo_list.size()>0){
		multimap<float, merge_xy>::iterator cl_iter=combo_list.begin();
		if(hierarchical_tree[(cl_iter->second).x].sibling!=-1 || hierarchical_tree[(cl_iter->second).y].sibling!=-1)
		{
			combo_list.erase(cl_iter);
			continue;
		}
		hierarchical_tree[(cl_iter->second).x].sibling=(cl_iter->second).y;
		hierarchical_tree[(cl_iter->second).x].father=hierarchical_tree.size();
		hierarchical_tree[(cl_iter->second).y].sibling=(cl_iter->second).x;
		hierarchical_tree[(cl_iter->second).y].father=hierarchical_tree.size();
		
		if ((cl_iter->second).x==(cl_iter->second).y)
			std::cout << "Single Merging Internal matching error "<<"\n";
		
		NetPattern merged_pattern;
		patternMerge(tmp_net_patterns.at((cl_iter->second).x), tmp_net_patterns.at((cl_iter->second).y), merged_pattern, g);
		tmp_net_patterns.push_back(merged_pattern);
		
		tree_node new_node;
		if((cl_iter->second).x<(cl_iter->second).y){
			new_node.lchild=(cl_iter->second).x;
			new_node.rchild=(cl_iter->second).y;
		}else{//(cl_iter->second).x>(cl_iter->second).y
			new_node.lchild=(cl_iter->second).y;
			new_node.rchild=(cl_iter->second).x;
		}
		
		if(hierarchical_tree[(cl_iter->second).x].min_p <= hierarchical_tree[(cl_iter->second).y].min_p){
			new_node.min_p=hierarchical_tree[(cl_iter->second).x].min_p;
			new_node.min_p_index=hierarchical_tree[(cl_iter->second).x].min_p_index;
		}else{//hierarchical_tree[(cl_iter->second).x].min_p > hierarchical_tree[(cl_iter->second).y
			new_node.min_p=hierarchical_tree[(cl_iter->second).y].min_p;
			new_node.min_p_index=hierarchical_tree[(cl_iter->second).y].min_p_index;
		}
		
		hierarchical_tree.push_back(new_node);
		
		for(vector<NetPattern>::size_type j=0; j<tmp_net_patterns.size()-1; j++){
			if(hierarchical_tree[j].sibling!=-1)
				continue;
			float merged_density=calculate_merged_density(tmp_net_patterns.at(j), tmp_net_patterns.at(tmp_net_patterns.size()-1), g);
			merge_xy combo;
			combo.x=newly_appended_vertexID;
			combo.y=j;
			combo_list.insert(pair<float,merge_xy>((-1)*merged_density, combo));
		}
		newly_appended_vertexID++;
		//cout<<"2*network_patterns.size()-newly_appended_vertexID: "<<2*network_patterns.size()-newly_appended_vertexID<<endl;
		//if(outfile_patterns)
		//	cout<<"outfile_patterns is ready to write."<<endl;
		if(2*network_patterns.size()-newly_appended_vertexID<=num_clusters && !network_pattern_combine_status && outfile_patterns){
			writeMergedPatterns(tmp_net_patterns, hierarchical_tree, outfile_patterns);
			network_pattern_combine_status=true;
		}
//		cout<<"newly_appended_vertexID: "<<newly_appended_vertexID<<endl;
	}
	
//	cout<< "Single Merging Iteration: "<<tmp_net_patterns.size()-network_patterns.size()<<"\n";
//	cout << "Hierarchical Tree construction done." <<std::endl;
	tmp_net_patterns.clear();	
}


void BuildHierachicalTreeDensity(const vector<NetPattern>& network_patterns, const float& density, const Graph& g, vector<tree_node>& hierarchical_tree, ofstream& outfile_patterns){//developed from mergeSingle
	hierarchical_tree.clear();
	hierarchical_tree.resize(network_patterns.size());
	vector<NetPattern> tmp_net_patterns=network_patterns;
	multimap<float, merge_xy> combo_list;
	for(vector<NetPattern>::size_type i=0; i<tmp_net_patterns.size(); i++){
		for(vector<NetPattern>::size_type j=0; j<i; j++){
			if (i==j)//must excluding self merging otherwise matching algorithm in the following will not work.
				continue;
			float merged_density=calculate_merged_density(tmp_net_patterns.at(i),tmp_net_patterns.at(j), g);
			merge_xy combo;
			combo.x=i;
			combo.y=j;
			combo_list.insert(pair<float,merge_xy>((-1)*merged_density, combo));
		}
		hierarchical_tree[i].min_p=tmp_net_patterns[i].get_pvalue();
		hierarchical_tree[i].min_p_index=i+1;//line start from 1;
	}
	
	
	
	int newly_appended_vertexID=tmp_net_patterns.size();
	bool network_pattern_combine_status=false;
	while (combo_list.size()>0){
		multimap<float, merge_xy>::iterator cl_iter=combo_list.begin();
		
		if(fabs(cl_iter->first)<density && !network_pattern_combine_status && outfile_patterns){//if reaching below lowerbound, output clusters before further merging.
			writeMergedPatterns(tmp_net_patterns, hierarchical_tree, outfile_patterns);
			network_pattern_combine_status=true;
		}
		
		if(hierarchical_tree[(cl_iter->second).x].sibling!=-1 || hierarchical_tree[(cl_iter->second).y].sibling!=-1)
		{
			combo_list.erase(cl_iter);
			continue;
		}
		hierarchical_tree[(cl_iter->second).x].sibling=(cl_iter->second).y;//sibling change occurs
		hierarchical_tree[(cl_iter->second).x].father=hierarchical_tree.size();
		hierarchical_tree[(cl_iter->second).y].sibling=(cl_iter->second).x;//sibling change occurs
		hierarchical_tree[(cl_iter->second).y].father=hierarchical_tree.size();
		
		if ((cl_iter->second).x==(cl_iter->second).y)
			std::cout << "Single Merging Internal matching error "<<"\n";
			
		
		NetPattern merged_pattern;
		patternMerge(tmp_net_patterns.at((cl_iter->second).x), tmp_net_patterns.at((cl_iter->second).y), merged_pattern, g);
		tmp_net_patterns.push_back(merged_pattern);
		
		tree_node new_node;
		if((cl_iter->second).x<(cl_iter->second).y){
			new_node.lchild=(cl_iter->second).x;
			new_node.rchild=(cl_iter->second).y;
		}else{//(cl_iter->second).x>(cl_iter->second).y
			new_node.lchild=(cl_iter->second).y;
			new_node.rchild=(cl_iter->second).x;
		}
		
		if(hierarchical_tree[(cl_iter->second).x].min_p <= hierarchical_tree[(cl_iter->second).y].min_p){
			new_node.min_p=hierarchical_tree[(cl_iter->second).x].min_p;
			new_node.min_p_index=hierarchical_tree[(cl_iter->second).x].min_p_index;
		}else{//hierarchical_tree[(cl_iter->second).x].min_p > hierarchical_tree[(cl_iter->second).y
			new_node.min_p=hierarchical_tree[(cl_iter->second).y].min_p;
			new_node.min_p_index=hierarchical_tree[(cl_iter->second).y].min_p_index;
		}
		
		hierarchical_tree.push_back(new_node);
		
		for(vector<NetPattern>::size_type j=0; j<tmp_net_patterns.size()-1; j++){
			if(hierarchical_tree[j].sibling!=-1)
				continue;
			float merged_density=calculate_merged_density(tmp_net_patterns.at(j), tmp_net_patterns.at(tmp_net_patterns.size()-1), g);
			merge_xy combo;
			combo.x=newly_appended_vertexID;
			combo.y=j;
			combo_list.insert(pair<float,merge_xy>((-1)*merged_density, combo));
		}
		newly_appended_vertexID++;
	}
	
	if(!network_pattern_combine_status && outfile_patterns){//never run below density. This implies one cluster will be outputted.
		writeMergedPatterns(tmp_net_patterns, hierarchical_tree, outfile_patterns);
	}
//	cout<< "Single Merging Iteration: "<<tmp_net_patterns.size()-network_patterns.size()<<"\n";
//	cout << "Hierarchical Tree construction done." <<std::endl;
	tmp_net_patterns.clear();	
}

void mergeSingle(vector<NetPattern>& network_patterns, const Graph& g, const float& min_density){
	multimap<float, merge_xy> combo_list;
	for(vector<NetPattern>::size_type i=0; i<network_patterns.size(); i++){
		for(vector<NetPattern>::size_type j=0; j<i; j++){
			if (i==j)//must excluding self merging otherwise matching algorithm in the following will not work.
				continue;
			float merged_density=calculate_merged_density(network_patterns.at(i),network_patterns.at(j), g);
			if(merged_density>=min_density){
				merge_xy combo;
				combo.x=i;
				combo.y=j;
				combo_list.insert(pair<float,merge_xy>((-1)*merged_density, combo));
			}
		}
	}
	vector<int> selected_list(network_patterns.size(),-1);
	vector<NetPattern> tmp_net_patterns=network_patterns;
	
	
	int newly_appended_vertexID=tmp_net_patterns.size();
	while (combo_list.size()>0){
		multimap<float, merge_xy>::iterator cl_iter=combo_list.begin();
		if(selected_list[(cl_iter->second).x]!=-1 || selected_list[(cl_iter->second).y]!=-1)
		{
			combo_list.erase(cl_iter);
			continue;
		}
		selected_list[(cl_iter->second).x]=(cl_iter->second).y;
		selected_list[(cl_iter->second).y]=(cl_iter->second).x;
		
		if ((cl_iter->second).x==(cl_iter->second).y)
			std::cout << "Single Merging Internal matching error "<<"\n";
		
		NetPattern merged_pattern;
		patternMerge(tmp_net_patterns.at((cl_iter->second).x), tmp_net_patterns.at((cl_iter->second).y), merged_pattern, g);
		tmp_net_patterns.push_back(merged_pattern);
		selected_list.push_back(-1);
		
		for(vector<NetPattern>::size_type j=0; j<tmp_net_patterns.size()-1; j++){
			if(selected_list[j]!=-1)
				continue;
				float merged_density=calculate_merged_density(tmp_net_patterns.at(j), tmp_net_patterns.at(tmp_net_patterns.size()-1), g);
				if(merged_density>=min_density){
				merge_xy combo;
				combo.x=newly_appended_vertexID;
				combo.y=j;
				combo_list.insert(pair<float,merge_xy>((-1)*merged_density, combo));
			}			
		}
		newly_appended_vertexID++;
		
	}
	
//	cout<< "Single Merging Iteration: "<<tmp_net_patterns.size()-network_patterns.size()<<"\n";//please comment out during time test
//	cout << "Merging completes. Copy back to network_patterns." <<std::endl;//please comment out during time test
	network_patterns.clear();
	for(int i=0; i<newly_appended_vertexID; i++)
	{
		if(selected_list[i]==-1 && tmp_net_patterns[i].getDensity()>=min_density)
			network_patterns.push_back(tmp_net_patterns[i]);
	}
	tmp_net_patterns.clear();	
}

void mergeBatch(vector<NetPattern>& network_patterns, const Graph& g, const float& min_density){
	int iteration_count=0;
	int new_vertexID=0;
	vector<int> selected_list(network_patterns.size(),-1);
	vector<NetPattern> tmp_net_patterns=network_patterns;
	while(true){
		multimap<float, merge_xy> combo_list;
		for(vector<NetPattern>::size_type i=new_vertexID; i<tmp_net_patterns.size(); i++){
			for(vector<NetPattern>::size_type j=0; j<i; j++){
				if (i==j)//must excluding self merging otherwise matching algorithm in the following will not work.
					continue;
				
				if (selected_list[j]!=-1)
					continue;
				float merged_density=calculate_merged_density(tmp_net_patterns.at(i),tmp_net_patterns.at(j), g);
				if(merged_density>=min_density){
					merge_xy combo;
					combo.x=i;
					combo.y=j;
					combo_list.insert(pair<float,merge_xy>((-1)*merged_density, combo));
				}
			}
		}
		
		new_vertexID=tmp_net_patterns.size();
		if (combo_list.size()==0)
			break;
		
		for (multimap<float, merge_xy>::const_iterator it=combo_list.begin(); it!=combo_list.end(); it++){//build the selected_list vector
			if ((selected_list[(it->second).x]==-1)&&(selected_list[(it->second).y]==-1)){
				if((it->second).x==(it->second).y){
					std::cout<<"Internal matching error"<<"\n";
					exit(-1);
				}
				selected_list[(it->second).x]=(it->second).y;
				selected_list[(it->second).y]=(it->second).x;
				NetPattern merged_pattern;
				patternMerge(tmp_net_patterns.at((it->second).x), tmp_net_patterns.at((it->second).y), merged_pattern, g);
				tmp_net_patterns.push_back(merged_pattern);
				selected_list.push_back(-1);
			}
		}
		
		iteration_count++;
//		cout << "Merging Iteration: "<<iteration_count<<"\n";//please comment out during time test
	}
	
//	cout << "Merging completes. Copy back to network_patterns." <<std::endl;//please comment out during time test
	
	network_patterns.clear();
	for(vector<NetPattern>::size_type i=0; i<tmp_net_patterns.size(); i++)
	{
		if(selected_list[i]==-1 && tmp_net_patterns[i].getDensity()>=min_density)
			network_patterns.push_back(tmp_net_patterns[i]);
	}
	tmp_net_patterns.clear();	
}

void writeTree_debug(const vector<tree_node>& hierarchical_tree){
	if(hierarchical_tree.size()==0)
		return;
	
	for(vector<tree_node>::const_iterator itt=hierarchical_tree.begin(); itt<hierarchical_tree.end(); itt++)
		cout<<"father: "<<itt->father<<" sibling: "<<itt->sibling<<" lchild: "<<itt->lchild<<" rchild: "<<itt->rchild<<" min_p: "<<itt->min_p<<" min_p_index: "<<itt->min_p_index<<endl;
}


void writeTree(const vector<tree_node>& hierarchical_tree, ofstream& outfile_tree){
	if(hierarchical_tree.size()==0)
		return;
		
	queue<int> Q;
	Q.push(hierarchical_tree.size()-1);
	
	vector<int> node_height(hierarchical_tree.size(), -1);
	//vector<vector<int> > heights_of_nodes(1, vector<int>());
	
	int MaxHeight=0;

	for(int i=0; i<int(hierarchical_tree.size()); i++){
		if (hierarchical_tree[i].lchild==-1){
			node_height[i]=0;
			//heights_of_nodes[node_height[i]].push_back(i);
		}else{
			if(node_height[hierarchical_tree[i].lchild]<0 || node_height[hierarchical_tree[i].rchild]<0)
				cout<<"Warning: A logic error occurs in height calculation. Results are invalid."<<endl;
				
			node_height[i]=max(node_height[hierarchical_tree[i].lchild], node_height[hierarchical_tree[i].rchild])+1;
			if(node_height[i]>MaxHeight)
				MaxHeight=node_height[i];
			//if(node_height[i]>int(heights_of_nodes.size())-1)
			//	heights_of_nodes.resize(node_height[i]+1, vector<int>());
			//heights_of_nodes[node_height[i]].push_back(i);
		}
	}
	
	cout<<"MaxHeight of the hierarchical tree: "<<MaxHeight<<endl;
	float segment_size=1.0/(MaxHeight+1);
	// for(int i=1; i<=MaxHeight; i++){
		// for(vector<int>::const_iterator itv=heights_of_nodes[i].begin(); itv<heights_of_nodes[i].end(); itv++){
			// outfile_tree<<hierarchical_tree[*itv].lchild+1<<"	"<<hierarchical_tree[*itv].rchild+1<<"	"<<i*segment_size<<endl;
		// }
	// }
	for(size_t i=(hierarchical_tree.size()+1)/2; i<hierarchical_tree.size(); i++){
		outfile_tree<<hierarchical_tree[i].lchild+1<<"	"<<hierarchical_tree[i].rchild+1<<"	"<<node_height[i]*segment_size<<endl;
	}
}

enum merge_type {S_MERGE=0, B_MERGE=1};

static void usage() {
	cout << "\nUsage:\n"
		"	GraphMining [-h] [-g graph_input_format] [-d density] [-m merge_method] [-b batch_size] [-p output_patternfile] [-t tree_file] [-n num_clusters]input_graphfile input_patternfile \n"
		"Description:\n"
		"	-h	Print the help message.\n"
		"	-g	graph_input_format. default value 0 (simple adjacent list); range graph_input_format={0,1,2}. (1: adjacency list for pathtree; 2: matrix)\n"
		"	-d	density. default value -1 (invalid); range 0<density<=1\n"
		"	-m	merge_method. default value 0 (S_MERGE); range merge_method={0,1}\n"
		"	-b	batch_size. default value 10000; range batch_size>=100\n"
		"	-p	output_patternfile. default none.\n"
		"	-t	tree_file. default none.\n"
		"	-n	num_clusters. default 3; The number of clusters for output when building the hierachical tree; range={1,2,3,4,5,6,7,8}.\n"
		"	input_graphfile	The graph file.\n"
		"	input_patternfile	The pattern file.\n"
		<< endl;
}

int main(int argc, char* argv[]) {
	if (argc == 1) {
		usage();
		return 1;
	}

	string input_graphfile, input_patternfile, output_patternfile, tree_file;
	int input_para_counter=1;
	int num_clusters=3;
	construction_type graph_input_format=S_ADJ_GRAPH;
	float density=-1;
	merge_type merge_method=S_MERGE;
	long int batch_size=10000;
	while (input_para_counter < argc) {
		if (strcmp("-h", argv[input_para_counter]) == 0) {
			usage();
			return 1;
		}
		
		if (strcmp("-g", argv[input_para_counter]) == 0) {
			input_para_counter++;
			graph_input_format=static_cast<construction_type>(atoi(argv[input_para_counter++]));
		}else if(strcmp("-d", argv[input_para_counter]) == 0) {
			input_para_counter++;
			density=atof(argv[input_para_counter++]);
		}else if(strcmp("-m", argv[input_para_counter]) == 0) {
			input_para_counter++;
			merge_method=static_cast<merge_type>(atoi(argv[input_para_counter++]));
		}else if(strcmp("-s", argv[input_para_counter]) == 0) {
			input_para_counter++;
			batch_size=atol(argv[input_para_counter++]);
		}else if(strcmp("-p", argv[input_para_counter])==0){
			input_para_counter++;
			output_patternfile=argv[input_para_counter++];
		}else if(strcmp("-t", argv[input_para_counter])==0){
			input_para_counter++;
			tree_file=argv[input_para_counter++];
		}else if(strcmp("-n", argv[input_para_counter])==0){
			input_para_counter++;
			num_clusters=atoi(argv[input_para_counter++]);
		}
		else{
			input_graphfile = argv[input_para_counter++];
			input_patternfile = argv[input_para_counter++];
		}
	}
		
	ifstream infile_graph(input_graphfile.c_str());
	ifstream infile_pattern(input_patternfile.c_str());
	ofstream outfile_tree(tree_file.c_str());
	ofstream outfile_patterns(output_patternfile.c_str());
	if (!infile_graph || !infile_pattern) {
		cout << "Error: Cannot open " << input_graphfile << " or "<< input_patternfile <<endl;
		return -1;
	}
	
	
	
	Graph g(infile_graph, graph_input_format);
	cout<<"graph construction done."<<endl;
	
	//determine number of patterns
	// long int number_of_patterns=0;
	string buf;
	// while(getline(infile_pattern, buf))
		// number_of_patterns++;
	// infile_pattern.clear();
	// infile_pattern.seekg(0, ios::beg);
	
	//summarize patterns
	vector<NetPattern> network_patterns;
	long int pattern_counter=0;
	float total_merge_time=0;
	while (getline(infile_pattern, buf)) {
		pattern_counter++;
		
		string partA=buf.substr(0, buf.find(";"));
		string partB;
		float pvalue=1;
		if(!outfile_tree){//in this case, assume input patterns do not contain pvalues 
			buf=buf.substr(buf.find(";")+1);
			size_t found=buf.find(";");
			if(found==string::npos)
				partB=buf;
			else//this is designed for 2nd processing
				partB=buf.substr(0, found);
		}else{
			buf=buf.substr(buf.find(";")+1);
			partB=buf.substr(0, buf.find(";"));
			pvalue=atof((buf.substr(buf.find(";")+1)).c_str());
		}
		set<int> bipartiteA;
		while (partA.find(" ") != string::npos){
			string tmp = partA.substr(0, partA.find(" "));
			int num;
			istringstream(tmp) >> num;
			bipartiteA.insert(num);
			partA.erase(0, partA.find(" ")+1);
		}
		
		set<int> bipartiteB;
		while (partB.find(" ") != string::npos){
			string tmp = partB.substr(0, partB.find(" "));
			int num;
			istringstream(tmp) >> num;
			bipartiteB.insert(num);
			partB.erase(0, partB.find(" ")+1);
		}
//		cout<<"Start construct one network patterns"<<endl;
		network_patterns.push_back(NetPattern(bipartiteA, bipartiteB, g, pvalue));
//		cout<<"One network pattern construction is done."<<endl;
		
		if(pattern_counter % batch_size==0 && !outfile_tree){//for hierachical tree construction. No batch processing is performed.
				struct timeval after_time, before_time;
				if(merge_method==S_MERGE){
					cout<<"pattern_counter: "<<pattern_counter<<endl;
					gettimeofday(&before_time, NULL);
					mergeSingle(network_patterns, g, density);
					gettimeofday(&after_time, NULL);
					cout<<"Single Merge is applied."<<endl;
				}
				else{//merge_method==B_MERGE
					cout<<"pattern_counter: "<<pattern_counter<<endl;
					gettimeofday(&before_time, NULL);
					mergeBatch(network_patterns, g, density);
					gettimeofday(&after_time, NULL);
					cout<<"Batch Merge is applied."<<endl;
				}
				total_merge_time+=(after_time.tv_sec - before_time.tv_sec)*1000.0+(after_time.tv_usec - before_time.tv_usec)*1.0/1000.0;
				if(outfile_patterns)
					writePatterns(network_patterns, outfile_patterns);
				network_patterns.clear();
		}
	}

	if(network_patterns.size()>0){
		if(!outfile_tree){
			struct timeval after_time, before_time;
			if(merge_method==S_MERGE){
				cout<<"pattern_counter: "<<pattern_counter<<endl;
				gettimeofday(&before_time, NULL);
				mergeSingle(network_patterns, g, density);
				gettimeofday(&after_time, NULL);
				cout<<"Single Merge is applied."<<endl;
			}
			else{//merge_method==B_MERGE
				cout<<"pattern_counter: "<<pattern_counter<<endl;
				gettimeofday(&before_time, NULL);
				mergeBatch(network_patterns, g, density);
				gettimeofday(&after_time, NULL);
				cout<<"Batch Merge is applied."<<endl;
			}
			total_merge_time+=(after_time.tv_sec - before_time.tv_sec)*1000.0+(after_time.tv_usec - before_time.tv_usec)*1.0/1000.0;

			if(outfile_patterns)
				writePatterns(network_patterns, outfile_patterns);
		}else{//merge_method void
				vector<tree_node> hierarchical_tree;
				cout<<"Start building hierachical tree."<<endl;
				if(density<0)//no density information or invalid density information
					BuildHierachicalTreeNum(network_patterns, num_clusters, g, hierarchical_tree, outfile_patterns);
				else//valid density information provided; ignore num_clusters.
					BuildHierachicalTreeDensity(network_patterns, density, g, hierarchical_tree, outfile_patterns);
				cout<<"Finish building hierachical tree."<<endl;
				cout<<"hierarchical_tree.size(): "<<hierarchical_tree.size()<<endl;
//				writeTree_debug(hierarchical_tree);
				writeTree(hierarchical_tree, outfile_tree);
		}
		network_patterns.clear();
	}
	
	//finish summarize patterns
	
	cout<<"Finish summarize network patterns."<<endl;
	
	if(!outfile_tree)
		cout<<"Total merge time: "<<total_merge_time<<endl;
	infile_graph.close();
	infile_pattern.close();
	
	cout << "#vertex size:" << g.num_vertices() << "\t#edges size:" << g.num_edges() << endl;
	outfile_patterns.close();
	outfile_tree.close();
	cout<<"end of the program"<<endl;
}


