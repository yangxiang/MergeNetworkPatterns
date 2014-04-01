#include <algorithm>
#include <vector>
#include <map>
#include <assert.h>
#include <errno.h>
#include <ext/algorithm>
#include <ext/functional>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <math.h>
#include <queue>
#include <string>
#include <string.h>
#include <sstream>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "PTNode.hh"
#include "TreeWriter.hh"
#include "CartesianProductDb.hh"
#include "MBCStatic.hh"

#define NON_TRIVIAL_CLIQUE_SIZE 3 //3 vertices is a non-trivial clique

int main(int argc, char **argv)
{
 
  if (argc < 6)
  {
    std::cerr << "Usage: " << argv[0] << " fi|fci|mfi dataset support biclique_outputfile force_symmetric(0 No, 1 Yes) [minMBCwidth=1] \n";
    std::cerr << "  fi => (normal) frequent itemsets, fci => frequent closed itemsets, mfi => maximal frequent itemsets\n";
    exit(1);
  }
  
  if (!(strcmp(argv[1], "fi") == 0) && !(strcmp(argv[1], "fci") == 0) && !(strcmp(argv[1], "mfi") == 0))
  {
    std::cerr << "Error: invalid frequent itemset type " << argv[1] << ".  Valid types are fi, fci, and mfi.\n";
    exit(1);
  }
  const char* fi_type = argv[1];

  struct timeval tv;
  gettimeofday(&tv, 0);
  srand48(tv.tv_sec + tv.tv_usec);

  MBCStatic::init_timer();

  errno = 0;
  double support = strtod(argv[3], NULL);
  if (errno != 0)
  {
    std::cerr << "Error converting support " << argv[3] << " to double.\n";
    exit(1);
  }
  
  std::string  biclique_outputfile;
  biclique_outputfile = argv[4];
  std::ofstream out_biclique(biclique_outputfile.c_str());
  if (!out_biclique && biclique_outputfile.size()>0) {
  	std::cout << "Error: Cannot open " << biclique_outputfile << std::endl;
	return -1;
  }	
  
  int force_symmetric=atoi(argv[5]);
  if(force_symmetric!=0 && force_symmetric!=1){
	std::cout<<"force_symmetric parameter out of range, exit."<<std::endl;
	exit(-1);
  }
  
  unsigned int minMBCwidth=1;
  if (argc >=7)
	minMBCwidth=atoi(argv[6]);
  
  std::cout << "Starting MBC\n";

  MBCStatic::ds_fpath = argv[2];
  std::string::size_type last_slash_pos = MBCStatic::ds_fpath.rfind('/');
  std::string ds_dir(MBCStatic::ds_fpath.substr(0, last_slash_pos + 1));
  std::string ds_fname(MBCStatic::ds_fpath.substr(last_slash_pos + 1));
  std::string ds_fbase(ds_fname.substr(0, ds_fname.find('.')));

  std::string fi_dir(ds_dir + "freq_itemsets");
  if (mkdir(fi_dir.c_str(), 0775) != 0)
  {
    if (errno != EEXIST)
    {
      perror("mkdir freq_itemsets");
      exit(1);
    }
  }

  std::ostringstream fi_fname_str;
  fi_fname_str << fi_dir << "/" << ds_fbase << "_" << support << "_" << fi_type << ".mafia";
  const std::string fi_fname(fi_fname_str.str());

  // regenerate frequent itemset file if necessary
  struct stat ds_stat, fi_stat;
  if (stat(MBCStatic::ds_fpath.c_str(), &ds_stat) == -1)
  {
    perror("stat dataset file");
    exit(1);
  }
  if (stat(fi_fname.c_str(), &fi_stat) != -1)
  {
    if (fi_stat.st_mtime <= ds_stat.st_mtime)
    {
      std::cout << "frequent itemset file " << fi_fname << " is not newer than dataset file " << MBCStatic::ds_fpath << "; deleting frequent itemset file\n";
      if (unlink(fi_fname.c_str()) == -1)
      {
        perror("unlink fi_fname");
        exit(1);
      }
    }
  }

  FILE *fi_file;
  if ((fi_file = fopen(fi_fname.c_str(), "rb")) == NULL)
  {
    if (errno != ENOENT)
    {
      perror("fopen fi_fname (try 1)");
      exit(1);
    }
    std::cout << "freq itemset file " << fi_fname << " does not exist; creating.\n";

    std::ostringstream mk_fi_cmd;
    //mk_fi_cmd << "bin/mafia -fi " << support / 100.0 << " -ascii " << MBCStatic::ds_fpath << " " << fi_fname;
    mk_fi_cmd << "bin/mafia -" << fi_type << " " << support / 100.0 << " -ascii " << MBCStatic::ds_fpath << " " << fi_fname;
    std::cout << "Executing " << mk_fi_cmd.str() << "\n";
    int fi_cmd_st;
    if ((fi_cmd_st = system(mk_fi_cmd.str().c_str())) == -1)
    {
      std::cerr << "call to `" << mk_fi_cmd.str() << "` failed\n";
      perror("make fi file");
      exit(1);
    }

    if (WEXITSTATUS(fi_cmd_st) != 0)
    {
       std::cerr << "command `" << mk_fi_cmd.str() << "` returned " << WEXITSTATUS(fi_cmd_st) << "\n";
       exit(1);
    }

    if ((fi_file = fopen(fi_fname.c_str(), "rb")) == NULL)
    {
      perror("fopen fi_fname (try 2)");
      exit(1);
    }
  }

  std::cout << "using freq itemset file " << fi_fname << "\n";

  PTNode* root = new PTNode();

  size_t line_size = 0;
  char *fi_line = NULL;

  MBCStatic::max_item = std::numeric_limits<Itemset::item_t>::min();
  MBCStatic::min_item = std::numeric_limits<Itemset::item_t>::max(); 
  uint64_t trans_num_int64 = 0;
  while (getline(&fi_line, &line_size, fi_file) != -1)
  {
    ++trans_num_int64;
	//std::cout << "Reading line " << fi_line;
    int item_digit_len, item_int;
    std::set<Itemset::item_t> items;
    char *fi_line_scan = fi_line;
    while (sscanf(fi_line_scan, "%d%n", &item_int, &item_digit_len) == 1)
    {
      //std::cout << "  item " << item_int << "\n";
      if (item_int < std::numeric_limits<Itemset::item_t>::min() || item_int > std::numeric_limits<Itemset::item_t>::max())
        {
        std::cout << "Item out of range: " << item_int << "\n";
          exit(1);
        }
        const Itemset::item_t item = static_cast<Itemset::item_t>(item_int);
		
      items.insert(item);
      fi_line_scan += item_digit_len;
    }
    long abs_support;
    // TODO: adapt to mafia output format
    if (sscanf(fi_line_scan, " (%ld)", &abs_support) != 1)
    {
      std::cerr << "Error parsing absolute support from '" << fi_line_scan << "'\n";
      std::cerr << "Expected a string like \" (abs_support)\"\n";
      exit(1);
    }
    //std::cout << "  absolute support: " << abs_support << "\n";

    if (items.size() > 0)
    {
      
      PTNode *ptNode = root->build_node(items);
  
      ptNode->coverable_itemset = true;
   
      ptNode->gamma = (float)(items.size() + abs_support) / (float)(items.size() * abs_support);
      //std::cout << "adding node " << ptNode << " with calculated gamma " << ptNode->gamma << " from items.size() " << items.size() << ", abs_support " << abs_support << "\n";
    }
  }
  MBCStatic::num_transactions = static_cast<Transactionset::transaction_t>(trans_num_int64);

  if (NULL != fi_line)
  {
    free(fi_line);
  }

  if (fclose(fi_file) == -1)
  {
    perror("fclose fi_file");
    exit(1);
  }




  FILE *ds_file;
  if ((ds_file = fopen(MBCStatic::ds_fpath.c_str(), "rb")) == NULL)
  {
    std::cerr << MBCStatic::ds_fpath << "\n";
    perror("fopen ds_fpath");
    exit(1);
  }

  char *ds_line;
  if ((ds_line = (char*)malloc(line_size)) == NULL)
  {
    perror("malloc ds_line");
    exit(1);
  }

  int item_int, item_digit_len;
  char *ds_line_scan;  
  // populate first level of nodes with transactions by scanning the dataset  
 
  uint64_t tot_cells_num = 0;
  trans_num_int64 = 0;
  PTNode::PTChildMap::iterator item_child;
  std::set<Itemset::item_t> added_single_items;//for removal single items added as non-MBC;  
  while (getline(&ds_line, &line_size, ds_file) != -1)
  {
    ++trans_num_int64;
    if (trans_num_int64 < std::numeric_limits<Transactionset::transaction_t>::min() || trans_num_int64 > std::numeric_limits<Transactionset::transaction_t>::max())
    {
      std::cout << "Transaction id out of range: " << trans_num_int64 << "\n";
      exit(1);
    }
    ds_line_scan = ds_line;
    while (sscanf(ds_line_scan, "%d%n", &item_int, &item_digit_len) != EOF)
    {
      if (item_int < std::numeric_limits<Itemset::item_t>::min() || item_int > std::numeric_limits<Itemset::item_t>::max())
      {
        std::cout << "Error: item out of range: " << item_int << "\n";
        exit(1);
      }
      const Itemset::item_t item = static_cast<Itemset::item_t>(item_int);
	  MBCStatic::max_item = std::max(MBCStatic::max_item, item);
      MBCStatic::min_item = std::min(MBCStatic::min_item, item);
      //std::cout << "Processing transaction " << trans_num_int64 << ", item " << item << ", item_digit_len " << item_digit_len << "\n";

	  PTNode *child;
      if ((item_child = root->children.find(item)) == root->children.end())
      {
        std::set<Itemset::item_t> single_item;
        single_item.insert(item);
        child = root->build_node(single_item);
		added_single_items.insert(item);
        child->coverable_itemset = true;
        //std::cout << "built single node child with item " << child->itemset << " (size " << child->itemset.size() << ")\n";
      } else {
        child = item_child->second;
        //std::cout << "retreived single node " << child << "\n";
      }
      //std::cout << "  child " << item << " matches\n";
      child->transactionset.push_back(static_cast<Transactionset::transaction_t>(trans_num_int64));
      ds_line_scan += item_digit_len;
      ++tot_cells_num;
    }
  }
  std::cout<<"min_item: "<<MBCStatic::min_item<<std::endl;
  std::cout<<"max_item: "<<MBCStatic::max_item<<std::endl;
  //std::cout<<"reach 1."<<std::endl;
  // populate with a bit for every zero
  BitDb uncov_zero_db(MBCStatic::num_transactions, MBCStatic::min_item, MBCStatic::max_item);

  if (fseek(ds_file, 0, SEEK_SET) != 0)
  {
    perror("fseek ds_file");
    exit(1);
  }

  Transactionset::transaction_t cur_trans = 0;
  while (getline(&ds_line, &line_size, ds_file) != -1)
  {
    ++cur_trans;
    //std::cout << "populating uncov_zero_db for line " << ds_line;
    std::vector<bool> line_items(MBCStatic::max_item);
    ds_line_scan = ds_line;
    while (sscanf(ds_line_scan, "%d%n", &item_int, &item_digit_len) != EOF)
    {
      const Itemset::item_t item = static_cast<Itemset::item_t>(item_int);
      line_items[item] = true;
      ds_line_scan += item_digit_len;
    }
    for (Itemset::item_t i=MBCStatic::min_item; i<=MBCStatic::max_item; ++i)
    {
      if (!line_items[i])
      {
        //std::cout << "  inserting uncov bit for trans " << cur_trans << ", item " << i << "\n";
        uncov_zero_db.insert(cur_trans, i);
      }
    }
  }

  //std::cout<<"reach 2."<<std::endl;
  free(ds_line);

  if (fclose(ds_file) == -1)
  {
    perror("fclose ds_fpath");
    exit(1);
  }

  //std::cout << "uncov_zero_db:\n" <<  uncov_zero_db << "\n";


  for (PTNode::PTChildMap::iterator rcIt=root->children.begin(); rcIt!=root->children.end(); ++rcIt)
  {
    PTNode *child = rcIt->second;
    //std::cout << "checking root child with itemset: " << child->itemset << "\n";
    if (child->gamma == std::numeric_limits<float>::max())
    {
      child->gamma = (float)(child->transactionset.size() + child->itemset.size()) / (float)((double)child->transactionset.size() * (double)child->itemset.size());
      //std::cout << "updated root child's init_support gamma to " << child->gamma << "\n";
    }
  }
  
  //std::cout<<"reach 3."<<std::endl;

//  std::cout << "orig db size_horiz: " << tot_cells_num + MBCStatic::num_transactions << "\n";
//  std::cout << "orig db size_min: " << MBCStatic::num_transactions + (MBCStatic::max_item - MBCStatic::min_item + 1) << "\n";

  BitDb *covered = new BitDb(MBCStatic::num_transactions, MBCStatic::min_item, MBCStatic::max_item);

  typedef PTNode::GONodeSet GONodeSet;

  GONodeSet* gons = root->gamma_ordered_nodeset();

  CartesianProductDb bicliques;
 
  //std::cout<<"reach 3.5."<<std::endl;
 //below are code specifically for MBC
 
  
  for(GONodeSet::iterator ists=gons->begin(); ists!=gons->end(); ists++)
  {
	Transactionset* trans_chosen = new Transactionset;
	(*ists)->recalc_gamma(*covered, trans_chosen, root);
	if(((*ists)->itemset).size()==1){
		 Itemset::item_t tmp_single_item=*(((*ists)->itemset).begin());
		 if (added_single_items.find(tmp_single_item)!=added_single_items.end())
			continue;
	}
	CartesianProduct cp((*ists)->itemset, *trans_chosen);
    bicliques.push_back(cp);
	delete trans_chosen;
    trans_chosen = NULL;
  }
  
  //std::cout<<"reach 4."<<std::endl;
  
  //output bicliques
  std::vector<int> number_per_size;
  for (CartesianProductDb::iterator itbc=bicliques.begin(); itbc!=bicliques.end(); itbc++){
		unsigned int MBCwidth=std::min((itbc->itemset).size(), (itbc->transactionset).size());
  
		if(MBCwidth<minMBCwidth)
			continue;
			
	if(force_symmetric==0){
	  //output transet
			for (Transactionset::iterator itt=(itbc->transactionset).begin(); itt!=(itbc->transactionset).end(); itt++)
				out_biclique<<*itt<<" ";
			out_biclique<<";";
	 //output transet done
				
	  //output itemset
			for (Itemset::iterator iti=(itbc->itemset).begin(); iti!=(itbc->itemset).end(); iti++)
				out_biclique<<*iti<<" ";
			out_biclique<<std::endl;
	  //output itemset done
	  
		int elements_to_append= MBCwidth-number_per_size.size()+1;
		for(int i=0; i<elements_to_append; i++)
			number_per_size.push_back(0);
		number_per_size[MBCwidth]++;		
		
	}else{//force_symmetric==1
		std::sort((itbc->transactionset).begin(), (itbc->transactionset).end());
		std::sort((itbc->itemset).begin(), (itbc->itemset).end());
		Transactionset::iterator itt=(itbc->transactionset).begin();
		Itemset::iterator iti=(itbc->itemset).begin();
		std::vector<int> common_vertices;
		while(itt<(itbc->transactionset).end() && iti<(itbc->itemset).end()){
			if(*itt<*iti)
				itt++;
			else if(*itt>*iti)
				iti++;
			else{//*itt==*iti
				common_vertices.push_back(*itt);
				itt++;
				iti++;
			}
		}
		if(common_vertices.size()>=NON_TRIVIAL_CLIQUE_SIZE){
			for(std::vector<int>::const_iterator cit=common_vertices.begin(); cit<common_vertices.end(); cit++)
				out_biclique<<*cit<<" ";
			out_biclique<<";";
			
			for(std::vector<int>::const_iterator cit=common_vertices.begin(); cit<common_vertices.end(); cit++)
				out_biclique<<*cit<<" ";
			out_biclique<<std::endl;
			
			int elements_to_append= common_vertices.size()-number_per_size.size()+1;
			for(int i=0; i<elements_to_append; i++)
				number_per_size.push_back(0);
			number_per_size[common_vertices.size()]++;
		}
	}
  

 }
	if(force_symmetric==0){
		for(int i=1; i<int(number_per_size.size()); i++)
			std::cout<<"biclique width: "<<i<<"; Number: "<<number_per_size[i]<<std::endl;
	}else{
		for(int i=NON_TRIVIAL_CLIQUE_SIZE; i<int(number_per_size.size()); i++)
			std::cout<<"biclique width: "<<i<<"; Number: "<<number_per_size[i]<<std::endl;
	}
 
  //output bicliques done
  
  	 
  
  bicliques.clear();
  

  delete covered;

  delete gons;
 
  delete root;

  return 0;
}
