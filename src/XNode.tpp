#pragma once

template <size_t N, typename ElemType, size_t M, size_t m>
XTree<N, ElemType, M, m>::XNode::XNode() : size(0) {
  this->entries.resize(M);
}

template <size_t N, typename ElemType, size_t M, size_t m>
typename XTree<N, ElemType, M, m>::XNode::iterator
XTree<N, ElemType, M, m>::XNode::begin() {
  return entries.begin();
}

template <size_t N, typename ElemType, size_t M, size_t m>
typename XTree<N, ElemType, M, m>::XNode::iterator
XTree<N, ElemType, M, m>::XNode::end() {
  return entries.begin() + size;
}

template <size_t N, typename ElemType, size_t M, size_t m>
typename XTree<N, ElemType, M, m>::XNode::const_iterator
XTree<N, ElemType, M, m>::XNode::begin()
const {
  return entries.begin();
}

template <size_t N, typename ElemType, size_t M, size_t m>
typename XTree<N, ElemType, M, m>::XNode::const_iterator
XTree<N, ElemType, M, m>::XNode::end()
const {
  return entries.begin() + size;
}

template <size_t N, typename ElemType, size_t M, size_t m>
typename XTree<N, ElemType, M, m>::SpatialObject&
XTree<N, ElemType, M, m>::XNode::operator[](
  size_t index) {
  return entries.at(index);
}

template <size_t N, typename ElemType, size_t M, size_t m>
typename XTree<N, ElemType, M, m>::SpatialObject
XTree<N, ElemType, M, m>::XNode::operator[](
  size_t index) const {
  return entries.at(index);
}

template <size_t N, typename ElemType, size_t M, size_t m>
bool XTree<N, ElemType, M, m>::XNode::isLeaf() {
  if (!entries.empty() && entries.at(0).child_pointer != nullptr)
    return false;

  return true;
}

template <size_t N, typename ElemType, size_t M, size_t m>
std::shared_ptr<typename XTree<N, ElemType, M, m>::XNode>
XTree<N, ElemType, M, m>::XNode::insert(
  const SpatialObject& new_entry) {
  if (size < M) {
    entries[size++] = new_entry;
    return nullptr;
  }

  std::vector<SpatialObject> entries(M + 1);
  std::copy(begin(), end(), entries.begin());
  entries.at(M) = new_entry;
  this->size = 0;
  size_t first, second;
  pickSeeds(entries, &first, &second);
  this->insert(entries.at(first));
  auto new_node = std::make_shared<XNode>();
  new_node->insert(entries.at(second));
  auto mbb_group1 = entries.at(first).box;
  auto mbb_group2 = entries.at(second).box;
  entries.erase(entries.begin() + first);
  entries.erase(entries.begin() + second - 1);
  float area_increase_g1, area_increase_g2;
  Hyperrectangle<N> enlarged_mbb_g1, enlarged_mbb_g2;
  size_t idx;

  while (!entries.empty()) {
    if (size + entries.size() == m) {
      for (auto& e : entries) this->insert(e);

      entries.clear();
    } else if (new_node->entries.size() + entries.size() == m) {
      for (auto& e : entries) new_node->insert(e);

      entries.clear();
    } else {
      pickNext(entries, &idx, mbb_group1, mbb_group2);
      enlarged_mbb_g1 = mbb_group1;
      enlarged_mbb_g1.adjust(entries.at(idx).box);
      area_increase_g1 = enlarged_mbb_g1.getArea() - mbb_group1.getArea();
      enlarged_mbb_g2 = mbb_group2;
      enlarged_mbb_g2.adjust(entries.at(idx).box);
      area_increase_g2 = enlarged_mbb_g2.getArea() - mbb_group2.getArea();

      if (area_increase_g1 < area_increase_g2 ||
          (area_increase_g1 == area_increase_g2 &&
           mbb_group1.getArea() < mbb_group2.getArea()) ||
          (area_increase_g1 == area_increase_g2 &&
           mbb_group1.getArea() == mbb_group2.getArea() &&
           size <= new_node->entries.size())) {
        mbb_group1.adjust(entries.at(idx).box);
        this->insert(entries.at(idx));
      } else {
        mbb_group2.adjust(entries.at(idx).box);
        new_node->insert(entries.at(idx));
      }

      entries.erase(entries.begin() + idx);
    }
  }

  return new_node;
}

template <size_t N, typename ElemType, size_t M, size_t m>
void XTree<N, ElemType, M, m>::XNode::pickSeeds(
  std::vector<SpatialObject>& entries, size_t* first, size_t* second) {
  float max_waste = 0.f, area;
  Hyperrectangle<N> enlargement_box;

  for (size_t i = 0; i < M; ++i) {
    for (size_t j = i + 1; j < M + 1; ++j) {
      enlargement_box = entries.at(i).box;
      enlargement_box.adjust(entries.at(j).box);
      area = enlargement_box.getArea() - entries.at(i).box.getArea() -
             entries.at(j).box.getArea();

      if (area > max_waste) {
        max_waste = area;
        *first = i;
        *second = j;
      }
    }
  }
}

template <size_t N, typename ElemType, size_t M, size_t m>
void XTree<N, ElemType, M, m>::XNode::pickNext(
  std::vector<SpatialObject>& entries,
  size_t* idx,
  const Hyperrectangle<N>& mbb_group1,
  const Hyperrectangle<N>& mbb_group2) {
  float max_diff = -1.f;
  Hyperrectangle<N> enlarged_g1, enlarged_g2;
  float area_increase_g1, area_increase_g2, area_diff;

  for (size_t i = 0; i < entries.size(); ++i) {
    enlarged_g1 = mbb_group1;
    enlarged_g1.adjust(entries.at(i).box);
    area_increase_g1 = enlarged_g1.getArea() - mbb_group1.getArea();
    enlarged_g2 = mbb_group2;
    enlarged_g2.adjust(entries.at(i).box);
    area_increase_g2 = enlarged_g2.getArea() - mbb_group2.getArea();
    area_diff = std::abs(area_increase_g1 - area_increase_g2);

    if (area_diff > max_diff) {
      max_diff = area_diff;
      *idx = i;
    }
  }
}
