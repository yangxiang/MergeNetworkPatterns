#include "Itemset.hh"

Itemset::Itemset()
{
}

Itemset::Itemset(const Itemset& in_itemset)
    : is(in_itemset.is)
{
}

Itemset::~Itemset()
{
}


Itemset::iterator Itemset::begin()
{
  return is.begin();
}

Itemset::iterator Itemset::end()
{
  return is.end();
}

Itemset::const_iterator Itemset::begin() const
{
  return is.begin();
}

Itemset::const_iterator Itemset::end() const
{
  return is.end();
}

Itemset::size_type Itemset::size() const
{
  return is.size();
}

/*
std::pair<Itemset::iterator,bool> Itemset::insert(const Itemset::value_type& x)
{
  return is.insert(x);
}
*/

void Itemset::resize(Itemset::size_type n)
{
  is.resize(n);
}

void Itemset::push_back(const value_type& v)
{
  is.push_back(v);
}

std::ostream& operator<<(std::ostream& os, const Itemset* is)
{
  if (NULL == is)
  {
    os << "NULL";
  } else {
    os << *is;
  }
  return os;
}


std::ostream& operator<<(std::ostream& os, const Itemset& is)
{
  //os << "{ ";
  if (is.size() == 0)
  {
    os << "empty itemset";
  } else {
    for (Itemset::const_iterator iIt=is.begin(); iIt!=is.end(); ++iIt)
    {
      //if (iIt != is.begin()) os << ", ";
      if (iIt != is.begin()) os << " ";
      os << *iIt;
    }
  }
  //os << " }";
  return os;
}
