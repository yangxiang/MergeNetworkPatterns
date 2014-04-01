#include "GraphMatrix.h"

Graph::Graph() {
	graph = GRA();
	vl = VertexList();
	WEIGHT_CUTOFF=0.5;
}

Graph::Graph(int size) {
	vsize = size;
	col_size=vsize;
	vl = VertexList(size);
	graph = GRA(size, EdgeList(size, 0));
	WEIGHT_CUTOFF=0.5;
}

Graph::Graph(GRA& g, VertexList& vlist) {
	vsize = vlist.size();
	col_size=vsize;
	graph = g;
	vl = vlist;
	WEIGHT_CUTOFF=0.5;
}

Graph::Graph(istream& in, construction_type& graph_format, const float& w_cutoff) {
	WEIGHT_CUTOFF=w_cutoff;
	if (graph_format==S_ADJ_GRAPH)
		readGraphSAdj(in);
	else if(graph_format==P_ADJ_GRAPH)
		readGraphPAdj(in);
	else if (graph_format==MATRIX_GRAPH)
		readGraphMatrix(in);
	else
		cout<<"unrecognized graph construction type"<<endl;
}

Graph::~Graph() {}

void Graph::printGraph() {
	writeGraphPADJ(cout);
}

void Graph::printGraphMatrix() {
	writeGraphMatrix(cout);
}

void Graph::clear() {
	vsize = 0;
	col_size=0;
	graph.clear();
	vl.clear();
}

void Graph::strTrimRight(string& str) {
	string whitespaces(" \t");
	size_t index = str.find_last_not_of(whitespaces);
	int length=str.size();
	if (index != string::npos) 
		str.erase(index+1);
	else
		str.clear();
	if(int(str[length-1])==13)
		str.erase(length-1);		
}

void Graph::readGraphSAdj(istream& in) {
	// initialize
	vsize = 0;
	col_size=0;

	int sid = 0;
	int max_col_num = 0;
	int min_col_num = MAX_VAL;
	string buf;
	//determine row and col
	while (getline(in, buf)) {
		while (buf.find(" ") != string::npos) {
			string sub = buf.substr(0, buf.find(" "));
			int tid;
			istringstream(sub) >> tid;
			if(tid>max_col_num)
				max_col_num=tid;
			if(tid<min_col_num)
				min_col_num=tid;
			buf.erase(0, buf.find(" ")+1);
		}
		++sid;
	}
	vsize=sid;
	col_base=min_col_num;
	col_size=max_col_num-col_base+1;
	vl = VertexList(vsize);
	graph = GRA(vsize, EdgeList(col_size,0));
	//determine row and col done
	cout<<"The graph column base is: "<<col_base<<endl;
	//build edges
	in.clear();
	in.seekg(0, ios::beg);
	sid=0;
	while (getline(in, buf)) {
		while (buf.find(" ") != string::npos) {
			string sub = buf.substr(0, buf.find(" "));
			int tid;
			istringstream(sub) >> tid;
			addEdge(sid,tid-col_base);
			buf.erase(0, buf.find(" ")+1);
		}
		++sid;
	}
	//build edges done

}

void Graph::readGraphPAdj(istream& in) {
	string buf;
	getline(in, buf);

	strTrimRight(buf);
	if (buf.compare("graph_for_mining")) {
		cout << "BAD FILE FORMAT!" << endl;
	//	exit(0);
	}
	
	int n;
	getline(in, buf);
	istringstream(buf) >> n;
	// initialize
	vsize = n;
	col_size=n;
	col_base=0;//this is fixed for path-tree data.
	vl = VertexList(n);
	graph = GRA(vsize, EdgeList(col_size,0));	

	string sub;
	int idx;
	int sid = 0;
	int tid = 0;
	while (getline(in, buf)) {
		strTrimRight(buf);
		idx = buf.find(":");
		buf.erase(0, idx+2);
		while (buf.find(" ") != string::npos) {
			sub = buf.substr(0, buf.find(" "));
			istringstream(sub) >> tid;
			buf.erase(0, buf.find(" ")+1);
			addEdge(sid, tid);
		}
		++sid;
	}
}	

void Graph::readGraphMatrix(istream& in) {
	string fi_line;
	
	//determine the number of rows and the number of cols.
	uint64_t Number_of_Rows=0;
	uint64_t Number_of_Cols=0;
	if(getline(in, fi_line)){
		Number_of_Rows++;
		int item_digit_len;
		WeightMeasure item_weight;
		char *fi_line_scan=new char [fi_line.size()+1];
		strcpy(fi_line_scan, fi_line.c_str());
		char *fi_line_pointer=fi_line_scan;
		//char *fi_line_scan = fi_line.c_str();
		while (sscanf(fi_line_pointer, "%f%n", &item_weight, &item_digit_len) == 1)
		{
			fi_line_pointer += item_digit_len;
			Number_of_Cols++;
		}
		while(getline(in, fi_line))
			Number_of_Rows++;
		delete[] fi_line_scan;
		fi_line_pointer=NULL;
	}
	vsize=Number_of_Rows;
	col_size=Number_of_Cols;
	col_base=1;
	vl = VertexList(vsize);
	graph = GRA(vsize, EdgeList(col_size,0));
	in.clear();//Must clear before seekg if reach the EOF.
	in.seekg (0, ios::beg);//reset in location to beginning
	//determine the number of rows and the number of cols done.
	
  uint64_t row_index=1;
  uint64_t previous_col_index=0;
  cout<<"start reading matrix"<<endl;
  while (getline(in, fi_line))
  {

    int item_digit_len;

    char *fi_line_scan=new char[fi_line.size()+1]; 
	strcpy(fi_line_scan, fi_line.c_str());
	uint64_t col_index=1;
	WeightMeasure weight;
	char *fi_line_pointer=fi_line_scan;
    while (sscanf(fi_line_pointer, "%f%n", &weight, &item_digit_len) == 1)
    {
      if (weight < -std::numeric_limits<WeightMeasure>::max() || weight > std::numeric_limits<WeightMeasure>::max())
      {
		std::cout << "weight out of range: " << weight << "\n";
		exit(1);
      }
	  else
	  {
			addEdge(row_index-1,col_index-1, weight);//vertex id start from 0
	  }
      fi_line_pointer += item_digit_len;
	  col_index++;
    }
	delete[] fi_line_scan;
	fi_line_pointer=NULL;
	
	//Check equality of column numbers.
	if(previous_col_index==0)
	{
		previous_col_index=col_index;
	}
	else
	{
		if(previous_col_index!=col_index)
		{
			std::cout << "Dataset format is wrong. Column numbers are not equal. Exit \n";
			exit(1);
		}
	}
	//Check equality of column numbers done.

	row_index++;
  }
}

void Graph::writeGraphPADJ(ostream& out) {
	cout << "Graph size = " << graph.size() << endl;
	out << "graph_for_mining" << endl;
	out << vl.size() << endl;

	GRA::iterator git;
	EdgeList el;
	EdgeList::iterator eit;
	VertexList::iterator vit;
	size_t i = 0;
	for (i = 0; i < vl.size(); i++) {
		out << i << ": ";
		el = graph[i];
		int j=0;
		for (eit = el.begin(); eit != el.end(); eit++, j++)
			if(*eit>=WEIGHT_CUTOFF)
				out << j << " ";
		out << "#" << endl;
	}
}

void Graph::writeGraphMatrix(ostream& out) {
	cout << "Graph size = " << graph.size() << endl;
	out << "graph_for_mining" << endl;
	out << vl.size() << endl;

	GRA::iterator git;
	EdgeList el;
	EdgeList::iterator eit;
	VertexList::iterator vit;
	size_t i = 0;
	for (i = 0; i < vl.size(); i++) {
		el = graph[i];
		for (eit = el.begin(); eit != el.end(); eit++)
			out << *eit << " ";
		out << endl;
	}
}

void Graph::addVertex(const size_t& vid) {
	if (vid >= vl.size()) {
		size_t size = vl.size();
		for (size_t i = 0; i < (vid-size+1); i++) {
			graph.push_back(EdgeList(col_size,0));
			vl.push_back(Vertex());
		}
		vsize = vl.size();
	}
}


int Graph::num_edges() {
	EdgeList el;
	GRA::iterator git;
	int num = 0;
	for (git = graph.begin(); git != graph.end(); git++) {
		el = *git;
		int neighbors=0;
		for (EdgeList::iterator eit = el.begin(); eit != el.end(); eit++)
			if(*eit>=WEIGHT_CUTOFF)
				neighbors++;
		num += neighbors;
	}
	return num;
}

// check whether the edge from src to trg is in the graph
bool Graph::hasEdge(int src, int trg) {
	EdgeList el = graph[src];
	EdgeList::iterator ei;
	for (ei = el.begin(); ei != el.end(); ei++)
		if ((*ei) == trg)
			return true;
	return false;

}


Graph& Graph::operator=(const Graph& g) {
	if (this != &g) {
		graph = g.graph;
		vl = g.vl;
		vsize = g.vsize;
		col_size=g.col_size;
	}
	return *this;
}

