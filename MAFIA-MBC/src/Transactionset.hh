#ifndef __TRANSACTION_SET_HH
#define __TRANSACTION_SET_HH

#include <iostream>
#include <vector>

class Transactionset
{
public:
  typedef uint32_t transaction_t;
  typedef std::vector<transaction_t> backing_set_t;
  typedef backing_set_t::iterator iterator;
  typedef backing_set_t::reverse_iterator reverse_iterator;
  typedef backing_set_t::const_iterator const_iterator;
  typedef backing_set_t::const_reverse_iterator const_reverse_iterator;
  typedef backing_set_t::size_type size_type;
  typedef backing_set_t::value_type value_type;
  typedef backing_set_t::const_reference const_reference;

  Transactionset();
  Transactionset(const Transactionset&);
  ~Transactionset();

  iterator begin();
  iterator end();

  const_iterator begin() const;
  const_iterator end() const;

  reverse_iterator rbegin();
  const_reverse_iterator rbegin() const;

  reverse_iterator rend();
  const_reverse_iterator rend() const;

  size_type size() const;

  /*
  template <class InputIterator>
  void insert(InputIterator, InputIterator);

  iterator insert(iterator pos, const value_type& x);
  */

  void push_back(const value_type&);

  std::pair<iterator, bool> insert(const value_type&);

  iterator erase(iterator);
  /*
  size_type erase(const value_type& k);
  void erase(iterator, iterator);
  */
 
  void clear();
  
  void resize(size_type);

  void reserve(size_type);

  void swap(Transactionset&);

  size_type capacity() const;

protected:
  backing_set_t ts;
};

std::ostream& operator<<(std::ostream&, const Transactionset*);
std::ostream& operator<<(std::ostream&, const Transactionset&);

/*
template <class InputIterator>
void Transactionset::insert(InputIterator i, InputIterator j)
{
  ts.insert(i, j);
}
*/

#endif
