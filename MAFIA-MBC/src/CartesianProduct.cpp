#include <algorithm>
#include "CartesianProduct.hh"

CartesianProduct::CartesianProduct()
{
}

CartesianProduct::CartesianProduct(const Itemset& in_itemset, const Transactionset& in_transactionset)
    // : itemset(in_itemset), transactionset(in_transactionset)
{
  itemset = in_itemset;
  transactionset = in_transactionset;
}

CartesianProduct::~CartesianProduct()
{
}

unsigned int CartesianProduct::size() const
{
  return itemset.size() + transactionset.size();
}

CartesianProduct CartesianProduct::union_with(const CartesianProduct& cp) const
{
  CartesianProduct unioned_cp;

  std::set_union(this->itemset.begin(), this->itemset.end(), cp.itemset.begin(), cp.itemset.end(), std::back_insert_iterator<Itemset>(unioned_cp.itemset));
  
  std::set_union(this->transactionset.begin(), this->transactionset.end(), cp.transactionset.begin(), cp.transactionset.end(), std::back_insert_iterator<Transactionset>(unioned_cp.transactionset));

  return unioned_cp;
}

CartesianProduct CartesianProduct::intersect_with(const CartesianProduct& cp) const
{
  CartesianProduct inter_cp;

  std::set_intersection(this->itemset.begin(), this->itemset.end(), cp.itemset.begin(), cp.itemset.end(), std::back_insert_iterator<Itemset>(inter_cp.itemset));

  std::set_intersection(this->transactionset.begin(), this->transactionset.end(), cp.transactionset.begin(), cp.transactionset.end(), std::back_insert_iterator<Transactionset>(inter_cp.transactionset));

  return inter_cp;

}

std::ostream& operator<<(std::ostream& os, const CartesianProduct* cp)
{
  if (NULL == cp)
  {
    os << "NULL";
  } else {
    os << *cp;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const CartesianProduct& cp)
{
  os << cp.itemset << "; " << cp.transactionset;
  os << " (" << cp.itemset.size() << " x " << cp.transactionset.size() << " == " << (cp.itemset.size() * cp.transactionset.size()) << ")";
  return os;
}

