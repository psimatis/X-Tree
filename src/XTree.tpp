#pragma once
#include <float.h>

#define XNODE typename XTree<N, ElemType, M, m>::XNode

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

template <size_t N>
float overlap(const Hyperrectangle<N>& hr1, const Hyperrectangle<N>& hr2) {
  float overlap = 1.f;

  for (size_t i = 0; i < N; ++i) {
    float length = 0.f;

    if (hr2.begin() < hr1.end() && hr1.begin() < hr2.end()
        || hr1.begin() < hr2.end() && hr2.begin() < hr1.end())
      length = std::min(hr1.end(), hr2.end())
               - std::max(hr1.begin(), hr2.begin());

    overlap *= length;
  }

  return overlap;
}

template <size_t N, typename ElemType, size_t M, size_t m>
float getTotalOverlap(const Hyperrectangle<N>& box,
                      const size_t& idx,
                      const std::shared_ptr<XNODE>& node) {
  float total_overlap = 0.f;

  size_t i;

  for (i = 0; i < idx; ++i)
    total_overlap += overlap((*node)[i].box, box);

  for (++i; i < node->size; ++i)
    total_overlap += overlap((*node)[i].box, box);

  return total_overlap;
}

template <size_t N>
float getAreaEnlargement(const Hyperrectangle<N>& container,
                         const Hyperrectangle<N>& item) {
  Hyperrectangle<N> enlarged_container = container;
  enlarged_container.adjust(item);
  return enlarged_container.getArea() - container.getArea();
}

template <size_t N, typename ElemType, size_t M, size_t m>
size_t getMinOverlapHyperrectangle(std::shared_ptr<XNODE> node,
                                   const Hyperrectangle<N>& box) {
  auto entries = &node->entries;
  size_t idx;
  float min_overlap, min_area, min_box_area;
  min_overlap = min_area = min_box_area = FLT_MAX;

  for (size_t i = 0; i < entries->size(); ++i) {
    auto entry = entries->at(i);
    auto overlap = getTotalOverlap<N, ElemType, M, m>(entry.box, i, node);
    auto areaEnlargement = getAreaEnlargement(entry.box, box);
    auto boxArea = entry.box.getArea();

    if (overlap < min_overlap ||
        (overlap == min_overlap && areaEnlargement < min_area) ||
        (overlap == min_overlap && areaEnlargement < min_area
         && boxArea < min_box_area)) {
      idx = i;
      min_overlap = overlap;
      min_area = areaEnlargement;
      min_box_area = boxArea;
    }
  }

  return idx;
}

template <size_t N, typename ElemType, size_t M, size_t m>
std::shared_ptr<XNODE> XTree<N, ElemType, M, m>::chooseLeaf(
  const std::shared_ptr<XNode>& current_node,
  const Hyperrectangle<N>& box,
  const ElemType& value) {
  if (!current_node->isLeaf()) {
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
std::shared_ptr<XNODE> XTree<N, ElemType, M, m>::chooseNode(
  const std::shared_ptr<XNode>& current_node,
  const Hyperrectangle<N>& box,
  SpatialObject*& entry) {

  std::shared_ptr<XNode> node;

  if ((*current_node)[0].child_pointer->isLeaf()) {
    auto idx_least_overlap = getMinOverlapHyperrectangle<N, ElemType, M, m>
                             (current_node, box);

    auto chosen_entry = (*current_node)[idx_least_overlap];
    node = chosen_entry.child_pointer;
  } else {
    float min_area, min_enlargement;
    min_area = min_enlargement = FLT_MAX;

    for (SpatialObject& current_entry : *current_node) {
      auto area_enlargement = getAreaEnlargement(current_entry.box, box);
      auto area = current_entry.box.getArea();

      if (area_enlargement < min_enlargement ||
          (area_enlargement == min_enlargement && area < min_area)) {
        min_enlargement = area_enlargement;
        min_area = area;
        node = current_entry.child_pointer;
        entry = &current_entry;
      }
    }
  }

  return node;
}

template <size_t N, typename ElemType, size_t M, size_t m>
std::shared_ptr<XNODE> XTree<N, ElemType, M, m>::adjustTree(
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
