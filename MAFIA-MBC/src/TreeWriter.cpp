#include <deque>
#include <iterator>
#include <queue>
#include "TreeWriter.hh"

void TreeWriter::write_to(const PTNode* root, std::ostream& os)
{
  std::queue<const PTNode*> node_queue;

  node_queue.push(root);

  while (!node_queue.empty())
  {
    const PTNode* cur_node = node_queue.front();
    node_queue.pop();

    os << *cur_node << "\n";

    for (PTNode::PTChildMap::const_iterator cIt=cur_node->children.begin(); cIt != cur_node->children.end(); ++cIt)
    {
      node_queue.push(cIt->second);
    }
  }
}
