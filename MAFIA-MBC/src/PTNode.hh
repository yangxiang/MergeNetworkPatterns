#ifndef __PT_NODE_HH
#define __PT_NODE_HH

#include <map>
#include <set>
#include <vector>
#include "PTNode.hh"
#include "Itemset.hh"
#include "Transactionset.hh"
#include "BitDb.hh"
#include "CartesianProductDb.hh"

class PTNode
{
public:
  typedef Itemset::item_t item_t;
  typedef std::map<item_t,PTNode*> PTChildMap;

  struct GammaOrderer
  {
    bool operator()(const PTNode*, const PTNode*) const;
  };

  typedef std::multiset<PTNode*,PTNode::GammaOrderer> GONodeSet;

  PTNode();
  ~PTNode();

  PTChildMap children;
  Itemset itemset;
  Transactionset transactionset;
  //float init_support;
  float gamma;
  bool coverable_itemset;

  void recalc_gamma(const BitDb& covered, Transactionset* trans_chosen, PTNode *root = NULL);

  PTNode* find(const std::vector<item_t>&) const;

  PTNode* build_node(const std::set<item_t>&);
  PTNode* build_node_emptyitemset(const std::set<item_t>&);

  void propagate_transactions_to(const Itemset& prop_is);

  float get_gamma() const;

  GONodeSet* gamma_ordered_nodeset() const;
  CartesianProductDb* to_nodelist() const;
};

std::ostream& operator<<(std::ostream&, const PTNode*);
std::ostream& operator<<(std::ostream&, const PTNode&);


#endif
