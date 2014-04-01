#ifndef __BIT_DB
#define __BIT_DB

class BitDb
{
friend std::ostream& operator<<(std::ostream&, const BitDb&);

public:
  BitDb(uint64_t num_trans, uint64_t min_item, uint64_t max_item);
  ~BitDb();

  void insert(int transaction, int item);

  void set_zero(int transaction, int item);
  
  bool exists(int transaction, int item) const;

protected:
  unsigned int get_bit_loc(int transaction, int item) const;

  void *db;
  uint64_t num_trans;
  uint64_t min_item;
  uint64_t max_item;
  uint64_t _item_width;
};

inline
unsigned int BitDb::get_bit_loc(int transaction, int item) const
{
  return (transaction - 1) * _item_width + (item - min_item);
}

inline
void BitDb::insert(int transaction, int item)
{
  //const uint64_t bit_loc = (uint64_t)(transaction - 1) * _item_width + (item - min_item);
  //static_cast<uint64_t*>(db)[bit_loc >> 6] |= ((uint64_t)1 << (bit_loc & (uint64_t)63));
  const unsigned int bit_loc = get_bit_loc(transaction, item);
  static_cast<unsigned char*>(db)[bit_loc >> 3] |= (1 << (bit_loc & 7));
}

inline
void BitDb::set_zero(int transaction, int item)
{
  const unsigned int bit_loc = get_bit_loc(transaction, item);
  static_cast<unsigned char*>(db)[bit_loc >> 3] &= ~(1 << (bit_loc & 7));
}
  
inline
bool BitDb::exists(int transaction, int item) const
{
  //const uint64_t bit_loc = (uint64_t)(transaction - 1) * _item_width + (item - min_item);
  //return (static_cast<uint64_t*>(db)[bit_loc >> 6] >> (bit_loc & (uint64_t)63)) & (uint64_t)1;
  const unsigned int bit_loc = get_bit_loc(transaction, item);
  return (static_cast<unsigned char*>(db)[bit_loc >> 3] >> (bit_loc & 7)) & 1;
}


#endif
