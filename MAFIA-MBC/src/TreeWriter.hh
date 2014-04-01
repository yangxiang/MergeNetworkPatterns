#ifndef __TREEWRITER_HH
#define __TREEWRITER_HH

#include <iostream>
#include <vector>
#include "PTNode.hh"

class TreeWriter
{
public:
  static void write_to(const PTNode*, std::ostream&);
};

#endif
