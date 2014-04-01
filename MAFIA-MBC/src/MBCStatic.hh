#ifndef __MBC_STATIC_HH
#define __MBC_STATIC_HH

#include "Itemset.hh"
#include "Transactionset.hh"

class MBCStatic
{
public:
  static std::string ds_fpath;
  static Itemset::item_t max_item;
  static Itemset::item_t min_item;
  static Transactionset::transaction_t num_transactions;
  static double beta;
  static unsigned int merge_k;
  static int64_t start_usec;

  static double elapsed_time_sec();
  static void init_timer();
};


#endif
