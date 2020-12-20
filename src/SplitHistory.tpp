#pragma once

template <size_t N, typename ElemType, size_t M, size_t m> SH::SHNode::SHNode(
  size_t dim) : dim(dim) {
  nodes[0] = nodes[1] = nullptr;
  related_xnode = nullptr;
}

template <size_t N, typename ElemType, size_t M, size_t m> SH::SplitHistory() :
  root(nullptr) {}

template <size_t N, typename ElemType, size_t M, size_t m>
bool SH::find(std::shared_ptr<SHNode> current_node,
              std::shared_ptr<XNODE> target_node, std::stack<SHNode*>& path) {
  if (current_node == nullptr)
    return 0;

  path.push(current_node.get());

  if (current_node->related_xnode == target_node)
    return 1;

  for (int i = 0; i < 2; ++i) {
    if (find(current_node->nodes[i], target_node, path))
      return 1;
  }

  path.pop();
  return 0;
}

template <size_t N, typename ElemType, size_t M, size_t m>
void SH::insert(size_t dim, std::shared_ptr<XNODE> left,
                std::shared_ptr<XNODE> right) {
  std::stack<SHNode*> path;
  SHNode* new_dim_node;

  if (root) {
    find(root, left, path);

    if (path.empty())
      return;

    new_dim_node = path.top();
  } else {
    root = std::make_shared<SHNode>(dim);
    new_dim_node = root.get();
  }

  new_dim_node->dim = dim;
  new_dim_node->related_xnode = nullptr;
  new_dim_node->nodes[0] = std::make_shared<SHNode>(-1);
  new_dim_node->nodes[0]->related_xnode = left;
  new_dim_node->nodes[1] = std::make_shared<SHNode>(-1);
  new_dim_node->nodes[1]->related_xnode = right;
}

template <size_t N, typename ElemType, size_t M, size_t m>
bool SH::getCommonSplitAxis() {
  return root->dim;
}
