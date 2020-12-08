#pragma once
#include <float.h>

template <size_t N, typename ElemType, size_t M, size_t m>
XTree<N, ElemType, M, m>::XTree()
  : root(std::make_shared<XNode>()), entry_count(0) {}

template <size_t N, typename ElemType, size_t M, size_t m>
XTree<N, ElemType, M, m>::~XTree() {}

template <size_t N, typename ElemType, size_t M, size_t m>
size_t XTree<N, ElemType, M, m>::dimension() const {
  return N;
}

template <size_t N, typename ElemType, size_t M, size_t m>
size_t XTree<N, ElemType, M, m>::size() const {
  return entry_count;
}

template <size_t N, typename ElemType, size_t M, size_t m>
bool XTree<N, ElemType, M, m>::empty() const {
  return size() == 0;
}

template <size_t N, typename ElemType, size_t M, size_t m>
void XTree<N, ElemType, M, m>::insert(const Hyperrectangle<N>& box,
                                      const ElemType& value) {
  auto split_node = chooseLeaf(root, box, value);
  ++entry_count;

  if (!split_node)
    return;

  auto new_root = std::make_shared<XNode>();
  new_root->entries[0].child_pointer = root;
  ++new_root->size;
  adjustTree(new_root, root, split_node, &new_root->entries[0]);
  root = new_root;
}

template <size_t N, typename ElemType, size_t M, size_t m>
int getMinOverlapHyperrectangle(
  std::shared_ptr<typename XTree<N, ElemType, M, m>::XNode> node,
  const Hyperrectangle<N>& box) {
  auto entries = &node->entries;
  int idx;
  float minOverlap, minArea, minBoxArea;
  minOverlap = minArea = minBoxArea = FLT_MAX;

  for (int i = 0; i < entries->size(); ++i) {
    auto hr = entries.at(i);
    auto overlap = getTotalOverlap(hr);
    auto areaEnlargement = getAreaEnlargement(hr, box);
    auto boxArea = hr.box.getArea();

    if (overlap < minOverlap ||
        (overlap == minOverlap && areaEnlargement < minArea) ||
        (overlap == minOverlap && areaEnlargement < minArea && boxArea < minBoxArea)) {
      idx = i;
      minOverlap = overlap;
      minArea = areaEnlargement;
      minBoxArea = boxArea;
    }
  }

  return idx;
}

template <size_t N, typename ElemType, size_t M, size_t m>
std::shared_ptr<typename XTree<N, ElemType, M, m>::XNode>
XTree<N, ElemType, M, m>::chooseLeaf(
  const std::shared_ptr<XNode>& current_node,
  const Hyperrectangle<N>& box,
  const ElemType& value) {
  if (!current_node->isLeaf()) {
    // if ((*current_node)[0].child_pointer->isLeaf()) {
    // auto least_overlap_enlargement_hyperrectangle = getMinOverlapHyperrectangle(current_node);
    // }
    SpatialObject* entry;
    auto next_node = chooseNode(current_node, box, entry);
    auto split_node = chooseLeaf(next_node, box, value);
    return adjustTree(current_node, next_node, split_node, entry);
  }

  SpatialObject new_entry;
  new_entry.box = box;
  new_entry.identifier = value;
  return current_node->insert(new_entry);
}

template <size_t N, typename ElemType, size_t M, size_t m>
std::shared_ptr<typename XTree<N, ElemType, M, m>::XNode>
XTree<N, ElemType, M, m>::chooseNode(
  const std::shared_ptr<XNode>& current_node,
  const Hyperrectangle<N>& box,
  SpatialObject*& entry) {
  float minimum_area = (*current_node)[0].box.getArea();

  Hyperrectangle<N> enlarged_box = (*current_node)[0].box;
  enlarged_box.adjust(box);
  float minimum_enlargement = enlarged_box.getArea() - minimum_area;

  float enlargement, area;
  std::shared_ptr<XNode> node = (*current_node)[0].child_pointer;

  entry = &(*current_node)[0];

  for (SpatialObject& current_entry : *current_node) {
    area = current_entry.box.getArea();

    enlarged_box = current_entry.box;
    enlarged_box.adjust(box);
    enlargement = enlarged_box.getArea() - area;

    if (enlargement < minimum_enlargement ||
        (enlargement == minimum_enlargement && area < minimum_area)) {
      minimum_enlargement = enlargement;
      minimum_area = area;
      node = current_entry.child_pointer;
      entry = &current_entry;
    }
  }

  return node;
}

template <size_t N, typename ElemType, size_t M, size_t m>
std::shared_ptr<typename XTree<N, ElemType, M, m>::XNode>
XTree<N, ElemType, M, m>::adjustTree(
  const std::shared_ptr<XNode>& parent,
  const std::shared_ptr<XNode>& left,
  const std::shared_ptr<XNode>& right,
  SpatialObject* entry) {
  entry->box.reset();

  for (SpatialObject current_entry : *left)
    entry->box.adjust(current_entry.box);

  if (!right)
    return nullptr;

  SpatialObject new_entry;
  new_entry.box.reset();

  for (SpatialObject& current_entry : *right)
    new_entry.box.adjust(current_entry.box);

  new_entry.child_pointer = right;
  return parent->insert(new_entry);
}

// template <size_t N, typename ElemType, size_t M, size_t m>
// std::vector<ElemType>& XTree<N, ElemType, M, m>::operator[](
// const Hyperrectangle<N>& box) {
// query_result.clear();
// query(box);
// return query_result;
// }
//
// template <size_t N, typename ElemType, size_t M, size_t m>
// std::vector<ElemType>& XTree<N, ElemType, M, m>::at(const Hyperrectangle<N>& box) {
// query_result.clear();
// query(box);
// return query_result;
// }
//
// template <size_t N, typename ElemType, size_t M, size_t m>
// const std::vector<ElemType>& XTree<N, ElemType, M, m>::at(
// const Hyperrectangle<N>& box) const {
// query_result.clear();
// query(box);
// return query_result;
// }
//
// template <size_t N, typename ElemType, size_t M, size_t m>
// void XTree<N, ElemType, M, m>::query(const Hyperrectangle<N>& box,
// std::shared_ptr<XNode> current) {
// if (!current) current = root;
//
// for (const auto& so : *current) {
// if (overlaps(so.box, box)) {
// if (current->isLeaf())
// query_result.push_back(so.identifier);
// else
// query(box, so.child_pointer);
// }
// }
// }
