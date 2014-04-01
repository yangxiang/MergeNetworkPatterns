#ifndef __CARTESIAN_PRODUCT_HH
#define __CARTESIAN_PRODUCT_HH

#include "Itemset.hh"
#include "Transactionset.hh"

class CartesianProduct
{
friend std::ostream& operator<<(std::ostream&, const CartesianProduct*);
friend std::ostream& operator<<(std::ostream&, const CartesianProduct&);

public:
  CartesianProduct();
  CartesianProduct(const Itemset&, const Transactionset&);
  ~CartesianProduct();

  typedef enum {
    ITEMSET = 0,
    TRANSACTIONSET = 1
  } Component;

  Itemset itemset;
  Transactionset transactionset;

  unsigned int size() const;

  CartesianProduct union_with(const CartesianProduct& cp) const;
  CartesianProduct intersect_with(const CartesianProduct& cp) const;
};


#endif
