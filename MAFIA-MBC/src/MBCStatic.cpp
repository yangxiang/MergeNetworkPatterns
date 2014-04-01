#include <assert.h>
#include <limits>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include "MBCStatic.hh"

std::string MBCStatic::ds_fpath;
Itemset::item_t MBCStatic::max_item = 0;
Itemset::item_t MBCStatic::min_item = 0;
Transactionset::transaction_t MBCStatic::num_transactions = 0;
double MBCStatic::beta = -1.0;
unsigned int MBCStatic::merge_k = std::numeric_limits<unsigned int>::max();
int64_t MBCStatic::start_usec = 0;

int64_t t_usec()
{
  struct timeval tv;
  gettimeofday(&tv, 0);
  return (int64_t)tv.tv_sec * pow(10, 6) + tv.tv_usec;
}

double MBCStatic::elapsed_time_sec()
{
  return (double)(t_usec() - MBCStatic::start_usec) / pow(10, 6);
}

void MBCStatic::init_timer()
{
  MBCStatic::start_usec = t_usec();
}
