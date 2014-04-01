#include "Transactionset.hh"

Transactionset::Transactionset()
{
}

Transactionset::Transactionset(const Transactionset& in_ts)
    : ts(in_ts.ts)
{
}

Transactionset::~Transactionset()
{
}

Transactionset::iterator Transactionset::begin()
{
  return ts.begin();
}

Transactionset::iterator Transactionset::end()
{
  return ts.end();
}

Transactionset::const_iterator Transactionset::begin() const
{
  return ts.begin();
}

Transactionset::const_iterator Transactionset::end() const
{
  return ts.end();
}

Transactionset::size_type Transactionset::size() const
{
  return ts.size();
}

Transactionset::iterator Transactionset::erase(iterator i)
{
  return ts.erase(i);
}

/*
Transactionset::size_type Transactionset::erase(const Transactionset::key_type& k)
{
  return ts.erase(k);
}

void Transactionset::erase(iterator i, iterator j)
{
  ts.erase(i, j);
}
*/

/*
std::pair<Transactionset::iterator, bool> Transactionset::insert(const Transactionset::value_type& x)
{
  return ts.insert(x);
}

Transactionset::iterator Transactionset::insert(Transactionset::iterator pos, const Transactionset::value_type& x)
{
  return ts.insert(pos, x);
}
*/

void Transactionset::push_back(const value_type& v)
{
  ts.push_back(v);
}

void Transactionset::clear()
{
  ts.clear();
}

void Transactionset::resize(size_type n)
{
  ts.resize(n);
}

Transactionset::reverse_iterator Transactionset::rbegin()
{
  return ts.rbegin();
}

Transactionset::const_reverse_iterator Transactionset::rbegin() const
{
  return ts.rbegin();
}

Transactionset::reverse_iterator Transactionset::rend()
{
  return ts.rend();
}

Transactionset::const_reverse_iterator Transactionset::rend() const
{
  return ts.rend();
}

void Transactionset::reserve(size_type s)
{
  ts.reserve(s);
}

void Transactionset::swap(Transactionset& in_ts)
{
  ts.swap(in_ts.ts);
}

Transactionset::size_type Transactionset::capacity() const
{
  return ts.capacity();
}

std::ostream& operator<<(std::ostream& os, const Transactionset* ts)
{
  if (NULL == ts)
  {
    os << "NULL";
  } else {
    os << *ts;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const Transactionset& ts)
{
  //os << "[ ";
  for (Transactionset::const_iterator tIt=ts.begin(); tIt!=ts.end(); ++tIt)
  {
    //if (tIt != ts.begin()) os << ", ";
    if (tIt != ts.begin()) os << " ";
    os << *tIt;
  }
  if (ts.size() == 0)
  {
    os << "[empty transactionset]";
  }
  //os << " ]";
  return os;
}
