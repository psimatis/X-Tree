#pragma once
#include "def.h"

template <size_t N, typename ElemType, size_t M, size_t m> XNODE::XNode() :
  size(0) {
  this->entries.resize(M);
}

template <size_t N, typename ElemType, size_t M, size_t m>
typename XNODE::iterator XNODE::begin() {
  return entries.begin();
}

template <size_t N, typename ElemType, size_t M, size_t m>
typename XNODE::iterator XNODE::end() {
  return entries.begin() + size;
}

template <size_t N, typename ElemType, size_t M, size_t m>
typename XNODE::const_iterator XNODE::begin() const {
  return entries.begin();
}

template <size_t N, typename ElemType, size_t M, size_t m>
typename XNODE::const_iterator XNODE::end() const {
  return entries.begin() + size;
}

template <size_t N, typename ElemType, size_t M, size_t m>
typename XTree<N, ElemType, M, m>::SpatialObject& XNODE::operator[](
  size_t index) {
  return entries.at(index);
}

template <size_t N, typename ElemType, size_t M, size_t m>
typename XTree<N, ElemType, M, m>::SpatialObject XNODE::operator[](size_t index)
const {
  return entries.at(index);
}

template <size_t N, typename ElemType, size_t M, size_t m>
bool XNODE::isLeaf() {
  if (!entries.empty() && entries.at(0).child_pointer != nullptr)
    return false;

  return true;
}

template <size_t N, typename ElemType, size_t M, size_t m>
size_t XNODE::chooseSplitAxis(const SpatialObject& new_entry) {
  std::vector<const SpatialObject*> entries_ptr(this->size + 1);

  for (size_t i = 0; i < this->size; ++i)
    entries_ptr[i] = &(*this)[i];

  entries_ptr[this->size] = &new_entry;
  size_t k = M - 2*m + 2;

  Hyperrectangle<N> first_group_hr, second_group_hr;
  first_group_hr.reset();
  second_group_hr.reset();

  size_t best_axis_idx;
  float margin, min_margin = FLT_MAX;

  for (size_t i = 0; i < N; ++i) {
    std::sort(entries_ptr.begin(), entries_ptr.end(),
    [&i](const SpatialObject*& lhs, const SpatialObject*& rhs) {
      return lhs->box[i].begin() < rhs->box[i].begin();
    });
    margin = 0.f;

    for (size_t j = 1; j <= k; ++j) {
      for (size_t f1 = 0; f1 < m - 1 + j; ++f1)
        first_group_hr.adjust(entries_ptr[f1]->box);

      for (size_t f2 = m - 1 + j; f2 < M + 1; ++f2)
        second_group_hr.adjust(entries_ptr[f2]->box);

      margin += first_group_hr.getMargin();
      margin += second_group_hr.getMargin();
      first_group_hr.reset();
      second_group_hr.reset();
    }

    if (margin < min_margin) {
      min_margin = margin;
      best_axis_idx = i;
    }
  }

  return best_axis_idx;
}

template <size_t N, typename ElemType, size_t M, size_t m>
std::shared_ptr<typename XNODE> XNODE::chooseSplitIndex(size_t axis,
    const SpatialObject& new_entry) {
  this->entries.push_back(new_entry);
  size_t k = M - 2*m + 2;

  Hyperrectangle<N> first_group_hr, second_group_hr;
  first_group_hr.reset();
  second_group_hr.reset();

  float group_overlap, total_area;
  float min_overlap, min_total_area;
  min_overlap = min_total_area = FLT_MAX;
  size_t second_group_idx;
  float ovlp, relative_ovlp;

  std::sort(this->entries.begin(), this->entries.end(),
  [&](const SpatialObject& lhs, const SpatialObject& rhs) {
    return lhs.box[axis].begin() < rhs.box[axis].begin();
  });

  for (size_t j = 1; j <= k; ++j) {
    for (size_t f1 = 0; f1 < m - 1 + j; ++f1)
      first_group_hr.adjust(this->entries[f1].box);

    for (size_t f2 = m - 1 + j; f2 < M + 1; ++f2) // check if i can merge these 2 for loops11
      second_group_hr.adjust(this->entries[f2].box);

    total_area = first_group_hr.getArea() + second_group_hr.getArea();
    group_overlap = overlap(first_group_hr, second_group_hr);

    if (group_overlap < min_overlap ||
        (group_overlap == min_overlap && total_area < min_total_area)) {
      min_overlap = group_overlap;
      min_total_area = total_area;
      second_group_idx = m - 1 + j;
      ovlp = overlap(first_group_hr, second_group_hr);
      relative_ovlp = ovlp / (min_total_area - ovlp);
    }

    first_group_hr.reset();
    second_group_hr.reset();
  }

  if (relative_ovlp > MAX_OVERLAP)
    return nullptr;

  auto new_node = std::make_shared<XNode>();
  for (size_t i = second_group_idx; i < M + 1; ++i)
    new_node->insert(this->entries[i]);

  this->size = second_group_idx;
  this->entries.resize(this->entries.size() - 1);

  return new_node;
}

template <size_t N, typename ElemType, size_t M, size_t m>
std::shared_ptr<std::pair<std::shared_ptr<typename XNODE>, size_t>>XNODE::topological_split(const SpatialObject& new_entry) {
    auto split_axis = chooseSplitAxis(new_entry);
    auto new_node = chooseSplitIndex(split_axis, new_entry);
    if (!new_node) return nullptr;
    return make_shared<std::pair<std::shared_ptr<XNode>, size_t>> (new_node, split_axis);
}

template <size_t N, typename ElemType, size_t M, size_t m>
std::shared_ptr<std::pair<std::shared_ptr<typename XNODE>, size_t>>XNODE::insert(const SpatialObject& new_entry) {

  if (size < entries.size()) {
    entries[size++] = new_entry;
    return nullptr;
  }

  auto new_node_and_axis = topological_split(new_entry);
  if (new_node_and_axis != nullptr)
    return new_node_and_axis;

  new_node_and_axis = overlap_minimal_split();
  if (new_node_and_axis != nullptr)
      return new_node_and_axis;

  // Create Supernode
  entries.resize(entries.size() + M);
  entries[size++] = new_entry;
  return nullptr;
}

template <size_t N, typename ElemType, size_t M, size_t m>
std::shared_ptr<std::pair<std::shared_ptr<typename XNODE>, size_t>>
XNODE::overlap_minimal_split() {
  size_t split_axis = split_history.getCommonSplitAxis();

  std::sort(this->entries.begin(), this->entries.end(),
  [&split_axis](const SpatialObject& lhs, const SpatialObject& rhs) {
    return lhs.box[split_axis].begin() < rhs.box[split_axis].begin();
  });

  size_t second_group_idx = 0;

  for (size_t i = 0; this->entries.size()/2 - i > m
       &&  this->entries.size()/2 + 1 + i < this->entries.size() - m; ++i) {
    if (this->entries[this->entries.size()/2 + i].box[split_axis].begin() <
        this->entries[this->entries.size()/2 + 1 + i].box[split_axis].begin()) {
      second_group_idx = this->entries.size()/2 + 1 + i + 1;
      break;
    }

    if (this->entries[this->entries.size()/2 - i].box[split_axis].begin() <
        this->entries[this->entries.size()/2 + 1 - i].box[split_axis].begin()) {
      second_group_idx = this->entries.size()/2 + 1 - i + 1;
      break;
    }
  }

  if (!second_group_idx)
    return nullptr;

  auto new_node = std::make_shared<XNode>();

  for (size_t i = second_group_idx; i < M + 1; ++i)
    new_node->insert(this->entries[i]);

  this->size = second_group_idx;
  this->entries.resize(this->entries.size() - 1);
  return std::make_shared<std::pair<std::shared_ptr<XNode>, size_t>>(new_node, split_axis);
}
