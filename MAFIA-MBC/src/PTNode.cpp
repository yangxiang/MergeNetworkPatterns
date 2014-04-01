#include <algorithm>
#include <assert.h>
#include <ext/functional>
#include <ext/slist>
#include <iostream>
#include <limits>
#include <queue>
#include <vector>
#include "PTNode.hh"
#include "MBCStatic.hh"

PTNode::PTNode()
    : gamma(std::numeric_limits<float>::max()), coverable_itemset(false)
{
}

PTNode::~PTNode()
{
  for (PTChildMap::iterator cIt=children.begin(); cIt != children.end(); ++cIt)
  {
    delete cIt->second;
  }
}

PTNode* PTNode::find(const std::vector<item_t>& child_list) const
{
  PTNode* node = const_cast<PTNode*>(this);
  for (std::vector<item_t>::const_iterator cIt=child_list.begin(); cIt != child_list.end(); ++cIt)
  {
    PTChildMap::iterator nIt = node->children.find(*cIt);
    assert(nIt != node->children.end());
    node = nIt->second;
  }
  return node;
}

PTNode* PTNode::build_node(const std::set<item_t>& path)
{
  /*
  std::cout << "build_node path: ";
  for (std::set<item_t>::const_iterator pit = path.begin(); pit != path.end(); ++pit)
  {
    std::cout << *pit << " ";
  }
  std::cout << "\n";
  */

  PTNode* node = const_cast<PTNode*>(this);
  for (std::set<item_t>::const_iterator iIt=path.begin(); iIt!=path.end(); ++iIt)
  {
    int cur_item = *iIt;
    //std::cout << "  cur_item " << cur_item << "\n";
    PTChildMap::iterator nIt = node->children.find(cur_item);
    if (nIt != node->children.end())
    {
      node = nIt->second;
      //std::cout << "  existing node with itemset " << node->itemset << "\n";
    } else {
      PTNode *new_child = new PTNode();
      node->children[cur_item] = new_child;
      assert(new_child->itemset.size() == 0);

      int dist_along_path = std::distance(path.begin(), iIt) + 1;
      new_child->itemset.resize(dist_along_path);
      std::set<item_t>::const_iterator cp_end = path.begin();
      std::advance(cp_end, dist_along_path);
      std::copy(path.begin(), cp_end, new_child->itemset.begin());

      node = new_child;
      //std::cout << "  new node with itemset " << node->itemset << "\n";
    }
  }
  return node;
}

PTNode* PTNode::build_node_emptyitemset(const std::set<item_t>& path)
{
  PTNode* node = const_cast<PTNode*>(this);
  for (std::set<item_t>::const_iterator iIt=path.begin(); iIt!=path.end(); ++iIt)
  {
    int cur_item = *iIt;
    //std::cout << "  cur_item " << cur_item << "\n";
    PTChildMap::iterator nIt = node->children.find(cur_item);
    if (nIt != node->children.end())
    {
      node = nIt->second;
      //std::cout << "  existing node with itemset " << node->itemset << "\n";
    } else {
      PTNode *new_child = new PTNode();
      node->children[cur_item] = new_child;
      assert(new_child->itemset.size() == 0);

      node = new_child;
      //std::cout << "  new node with itemset " << node->itemset << "\n";
    }
  }
  return node;
}

PTNode::GONodeSet* PTNode::gamma_ordered_nodeset() const
{
  GONodeSet* list = new GONodeSet; 
  std::queue<PTNode*> q;
  for (PTChildMap::const_iterator cIt=children.begin(); cIt != children.end(); ++cIt)
  {
    q.push(cIt->second);
  }
  while (!q.empty())
  {
    PTNode* node = q.front();
    q.pop();
    // account for maximal or closed frequent itemset, by not including their PTNodes in the result
    //if (node->itemset.size() > 0)
    if (node->coverable_itemset)
    {
      //assert(node->transactionset.size() > 0);
      list->insert(node);
    }

    for (PTChildMap::iterator cIt=node->children.begin(); cIt != node->children.end(); ++cIt)
    {
      q.push(cIt->second);
    }
  }
  return list;
}

CartesianProductDb* PTNode::to_nodelist() const
{
  CartesianProductDb* nl = new CartesianProductDb;

  std::queue<PTNode*> q;
  q.push(const_cast<PTNode*>(this));
  while(!q.empty())
  {
    PTNode *node = q.front();
    q.pop();
    if (node->itemset.size() > 0 && node->coverable_itemset)
    {
      nl->push_back(CartesianProduct(node->itemset, node->transactionset));
    }
    for (PTChildMap::const_iterator cIt=node->children.begin(); cIt!=node->children.end(); ++cIt)
    {
      q.push(cIt->second);
    }
  }

  return nl;
}

float PTNode::get_gamma() const
{
  return gamma;
}

// TODO: for false positives: write alternate version of propagate_transactions_to, which scans the dataset

void PTNode::propagate_transactions_to(const Itemset& prop_is)
{
  // only valid for root
  assert(itemset.size() == 0);
  //std::cout << "propagating transactions from root " << this << " to " << prop_is << "\n";
  PTNode *parent = const_cast<PTNode*>(this);
  const Itemset::const_iterator prop_is_end = prop_is.end();
  for (Itemset::const_iterator pIt=prop_is.begin(); pIt!=prop_is_end; ++pIt)
  {
    PTNode *child = parent->children.find(*pIt)->second;

    //std::cout << "child before propagation: " << child << "\n";

    if (child->transactionset.size() == 0 && parent != this)
    {
      if (child->itemset.size() == 0)
      {
        std::cout << "Error propagating transactions.  Child:\n" << *child << "\nof parent:\n" << parent << "\nhas no items.\n";
      }
      assert(child->itemset.size() > 0);
      //if (child->itemset.size() > 0)
      {
        const Itemset::item_t child_last_item = *(--child->itemset.end());
        PTNode *li_firstchild = this->children.find(child_last_item)->second;
        //std::cout << "intersecting parent " << parent << "\n";
        //std::cout << "with last_item " << child_last_item << " firstchild " << li_firstchild << "\n";
        child->transactionset.reserve(std::min(parent->transactionset.size(), li_firstchild->transactionset.size()));
        std::back_insert_iterator<Transactionset > ct_ii(child->transactionset);
        std::set_intersection(parent->transactionset.begin(), parent->transactionset.end(), li_firstchild->transactionset.begin(), li_firstchild->transactionset.end(), ct_ii);
  
        Transactionset(child->transactionset).swap(child->transactionset); // shrink capacity to size
  
        assert(child->transactionset.size() == child->transactionset.capacity());
  
        //std::cout << "child transactionset was propagated: " << child->transactionset << "\n";
      }
    }

    //std::cout << "child after propagation: " << child << "\n";

    parent = child;
  }
}

void PTNode::recalc_gamma(const BitDb& covered, Transactionset* trans_chosen, PTNode *root)
{
  typedef std::vector<std::pair<int,Transactionset::transaction_t> > uncover_trans_t;

  //std::cout << "recalcuating gamma for node " << this << "\n";
  
  if (trans_chosen != NULL && transactionset.size() == 0)
  {
    assert(NULL != root);
    root->propagate_transactions_to(this->itemset);

    //std::cout << "propagated transactions to this node " << this << "\n";
  } 

  //uncover_trans_t uncover_trans;
  //uncover_trans.reserve(transactionset.size());
  
  const int itemset_size = itemset.size();

  typedef std::vector<__gnu_cxx::slist<Transactionset::transaction_t> > uncov_trans_t;
  uncov_trans_t uncov_trans(itemset_size + 1);

  int num_uncovered;
  const Transactionset::const_iterator ts_end = transactionset.end();
  const Itemset::const_iterator is_begin = itemset.begin();
  const Itemset::const_iterator is_end = itemset.end();
  for (Transactionset::iterator transaction_it=transactionset.begin(); transaction_it!=ts_end; ++transaction_it)
  {
    num_uncovered = itemset_size;
    const Transactionset::transaction_t cur_trans = *transaction_it;
    for (Itemset::const_iterator iIt = is_begin; iIt != is_end; ++iIt)
    {
      if (covered.exists(cur_trans, *iIt))
      {
        --num_uncovered;
      }
    }
    uncov_trans[num_uncovered].push_front(cur_trans);
    //uncover_trans.push_back(std::make_pair(num_uncovered, cur_trans));
  }
  //std::sort(uncover_trans.begin(), uncover_trans.end());

  gamma = std::numeric_limits<float>::max();

  //std::cout << "node " << *this << " recalc_gamma() (num_uncovered " << num_uncovered << "):\n";

  if (NULL != trans_chosen)
  {
    trans_chosen->clear();
  }

  int num_transactions = 0, num_uncov_cells = 0;
  //while (lg_uncov_it != uncov_rend)
  for (uncov_trans_t::size_type cur_num_uncov=uncov_trans.size() - 1; cur_num_uncov > 0; --cur_num_uncov)
  {
    uncov_trans_t::value_type& t_list = uncov_trans[cur_num_uncov];
    while (!t_list.empty())
    {
      ++num_transactions;
      num_uncov_cells += cur_num_uncov;
  
      const int cov_cost = num_transactions + itemset_size;
      const float proposed_gamma = (float)cov_cost / (float)num_uncov_cells;
  
      if (proposed_gamma > gamma)
      {
        //std::cout << "  proposed_gamma " << proposed_gamma << " > node->gamma " << gamma << "\n";
        break;
      }
  
      gamma = proposed_gamma;
  
      if (NULL != trans_chosen)
      {
        trans_chosen->push_back(t_list.front());
      }
  
      //std::cout << "  updating node->gamma " << gamma << " to " << proposed_gamma << " == " << cov_cost << " / " << num_cov_cells << " using trans " << trans_chosen << "\n";

      t_list.pop_front();
    }
    //++lg_uncov_it;
  }
  if (NULL != trans_chosen)
  {
    std::sort(trans_chosen->begin(), trans_chosen->end());
  }
}

std::ostream& operator<<(std::ostream& os, const PTNode* pt_node)
{
  if (NULL == pt_node)
  {
    os << "NULL";
  } else {
    os << *pt_node;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const PTNode& pt_node)
{
  os << pt_node.itemset;
  os << ": ";
  os << pt_node.transactionset;
  os << " γ: " << pt_node.gamma;
  //os << " init_support: " << pt_node.init_support;

  return os;
}

bool PTNode::GammaOrderer::operator()(const PTNode* n1, const PTNode* n2) const
{
  return (n1->gamma < n2->gamma);
}

