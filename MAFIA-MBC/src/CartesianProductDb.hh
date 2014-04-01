#ifndef __CARTESIAN_PRODUCT_DB_HH
#define __CARTESIAN_PRODUCT_DB_HH

#include <map>
#include <vector>
#include "CartesianProduct.hh"
#include "BitDb.hh"



class CartesianProductDb
{
public:
  friend std::ostream& operator<<(std::ostream& os, const CartesianProductDb& cp_db);

  typedef std::vector<CartesianProduct> backing_t;
  typedef backing_t::iterator iterator;
  typedef backing_t::const_iterator const_iterator;
  typedef backing_t::size_type size_type;

 
  CartesianProductDb();
  ~CartesianProductDb();

  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;

  void push_back(const CartesianProduct&);
  void push_back_num_cov(uint64_t num_cov);
  void clear();

  uint64_t size_mem() const;
  uint64_t size_k() const;

  std::vector<int> *transaction_frequency() const;
  std::vector<int> *item_frequency() const;

  double set_mdl(const std::vector<int>* freq_list) const;
  double transaction_mdl() const;
  double item_mdl() const;

  void write_map_to_freq_file(const std::vector<int>& cm, const char *f_name) const;
  void write_frequency_files() const;

  size_type size() const;

  std::vector<uint64_t> tot_cells_cov;
  
protected:
  backing_t db;
};

#endif
