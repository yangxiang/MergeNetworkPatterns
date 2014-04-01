#ifndef __ITEMSET_HH
#define __ITEMSET_HH

#include <iostream>
#include <vector>

class Itemset
{
public:
  typedef uint16_t item_t;
  typedef std::vector<item_t> backing_set_t;
  typedef backing_set_t::iterator iterator;
  typedef backing_set_t::const_iterator const_iterator;
  typedef backing_set_t::size_type size_type;
  typedef backing_set_t::value_type value_type;
  typedef backing_set_t::const_reference const_reference;

  Itemset();
  Itemset(const Itemset&);
  ~Itemset();

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  /*
  template <class InputIterator>
  void insert(InputIterator, InputIterator);
  */

  void push_back(const value_type&);

  size_type size() const;

  void resize(size_type);

protected:
  backing_set_t is;
};

std::ostream& operator<<(std::ostream&, const Itemset*);
std::ostream& operator<<(std::ostream&, const Itemset&);

/*
template <class InputIterator>
void Itemset::insert(InputIterator i, InputIterator j)
{
  return is.insert(i, j);
}
*/

#endif
