#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <map>
#include <numeric>
#include "CartesianProductDb.hh"
#include "MBCStatic.hh"


CartesianProductDb::CartesianProductDb()
{
}

CartesianProductDb::~CartesianProductDb()
{
}

CartesianProductDb::iterator CartesianProductDb::begin()
{
  return db.begin();
}

CartesianProductDb::const_iterator CartesianProductDb::begin() const
{
  return db.begin();
}

CartesianProductDb::iterator CartesianProductDb::end()
{
  return db.end();
}
  
CartesianProductDb::const_iterator CartesianProductDb::end() const
{
  return db.end();
}

void CartesianProductDb::push_back(const CartesianProduct& cp)
{
  db.push_back(cp);
}

void CartesianProductDb::push_back_num_cov(uint64_t cells_cov)
{
  tot_cells_cov.push_back(cells_cov);
}

void CartesianProductDb::clear()
{
  db.clear();
}

uint64_t CartesianProductDb::size_mem() const
{
  uint64_t s = 0;
  for (backing_t::const_iterator cIt = db.begin(); cIt != db.end(); ++cIt)
  {
    s += cIt->size();
  }
  return s;
}

uint64_t CartesianProductDb::size_k() const
{
  return db.size();
}

std::vector<int> *CartesianProductDb::item_frequency() const
{
  std::vector<int> *i_ct = new std::vector<int>;
  for (backing_t::const_iterator cpIt=db.begin(); cpIt!=db.end(); ++cpIt)
  {
    for (Itemset::const_iterator iIt=cpIt->itemset.begin(); iIt!=cpIt->itemset.end(); ++iIt)
    {
      const int item = *iIt;
      if ((int)i_ct->size() <= item)
      {
        i_ct->resize(item + 1, 0);
      }
      ++i_ct->operator[](item);
    }
  }
  return i_ct;
}

std::vector<int> *CartesianProductDb::transaction_frequency() const
{
  std::vector<int>* t_ct = new std::vector<int>;
  for (backing_t::const_iterator cpIt=db.begin(); cpIt!=db.end(); ++cpIt)
  {
    for (Transactionset::const_iterator tIt=cpIt->transactionset.begin(); tIt!=cpIt->transactionset.end(); ++tIt)
    {
      const int transaction = *tIt;
      if ((int)t_ct->size() <= transaction)
      {
        t_ct->resize(transaction + 1, 0);
      }
      ++t_ct->operator[](transaction);
    }
  }
  return t_ct;
}

void CartesianProductDb::write_map_to_freq_file(const std::vector<int>& cv, const char *f_name) const
{
  std::vector<std::pair<int,int> > vp_map(cv.size());
  for (std::vector<int>::size_type i=0; i<cv.size(); ++i)
  {
    vp_map[i] = std::make_pair(cv[i], i);
  }
  std::sort(vp_map.begin(), vp_map.end());

  std::fstream is_out(f_name, std::ios::out | std::ios::binary);
  
  for (std::vector<std::pair<int,int> >::reverse_iterator vpIt=vp_map.rbegin(); vpIt!=vp_map.rend(); ++vpIt)
  {
    is_out << vpIt->second << "\t" << vpIt->first << "\n";
  }

  is_out.close();
}

void CartesianProductDb::write_frequency_files() const
{
  std::vector<int> *i_ct = item_frequency();
  std::vector<int> *t_ct = transaction_frequency();

  write_map_to_freq_file(*i_ct, "frequency_items");
  write_map_to_freq_file(*t_ct, "frequency_transactions");

  delete i_ct;
  delete t_ct;
}

double CartesianProductDb::set_mdl(const std::vector<int>* freq_list) const
{
  /*
  std::cout << " computing mdl of ";
  std::copy(freq_list->begin(), freq_list->end(), std::ostream_iterator<int>(std::cout,", "));
  std::cout << "\n";
  */

  double total_freq = std::accumulate(freq_list->begin(), freq_list->end(), 0);

  double mdl_sum = 0;
  for (std::vector<int>::const_iterator fIt=freq_list->begin(); fIt != freq_list->end(); ++fIt)
  {
    double cur_freq = *fIt;
    if (cur_freq != 0)
    {
      mdl_sum += cur_freq * log2(cur_freq / total_freq);
    }
  }
  mdl_sum *= -1;

  return mdl_sum;
}

CartesianProductDb::size_type CartesianProductDb::size() const
{
  return db.size();
}

double CartesianProductDb::item_mdl() const
{
  std::vector<int> *i_ct = item_frequency();
  double i_mdl = set_mdl(i_ct);
  delete i_ct;
  return i_mdl;
}

double CartesianProductDb::transaction_mdl() const
{
  std::vector<int> *t_ct = transaction_frequency();
  double t_mdl = set_mdl(t_ct);
  delete t_ct;
  return t_mdl;
}

std::ostream& operator<<(std::ostream& os, const CartesianProductDb& cp_db)
{
  for (CartesianProductDb::backing_t::size_type i=0; i<cp_db.db.size(); ++i)
  {
    //os << i << ": " << cp_db.db[i] << "\n";
    os << cp_db.db[i] << "\n";
  }
  return os;
}
