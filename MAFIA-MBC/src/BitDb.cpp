#include <iostream>
#include <math.h>
#include <stdlib.h>
#include "BitDb.hh"

BitDb::BitDb(uint64_t num_trans, uint64_t min_item, uint64_t max_item)
    : db(NULL), num_trans(num_trans), min_item(min_item), max_item(max_item), _item_width(max_item - min_item + 1)
{
  const uint64_t num_bits = num_trans * _item_width;
  const uint64_t num_bytes = num_bits / 8 + 4;

  if ((size_t)num_bytes < num_bytes)
  {
    std::cout << "Error: won't be able to calloc " << num_bytes << " bytes on architecture with sizeof(size_t) " << sizeof(size_t) << "\n";
  }

  //std::cout << "num_trans " << num_trans << " min_item " << min_item << " max_item " << max_item << " db_size " << db_size << "\n";
  //std::cout << "BitDb allocating " << num_bits << " bits, " << num_bytes << "\n";
  if ((db = calloc(num_bytes, 1)) == NULL)
  {
    perror("calloc BitDb()");
    std::cout << "num_bytes " << num_bytes << "\n";
    exit(1);
  }
}

BitDb::~BitDb()
{
  free(db);
}

std::ostream& operator<<(std::ostream& os, const BitDb& tip_db)
{
  for (std::size_t i=1; i<=tip_db.num_trans; ++i)
  {
    for (std::size_t j=tip_db.min_item; j<=tip_db.max_item; ++j)
    {
      std::cout << tip_db.exists(i, j);
    }
    std::cout << "\n";
  }
  return os;
}
