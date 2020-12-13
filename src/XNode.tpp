#pragma once

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
  std::vector<const SpatialObject*> entries(this->size + 1);

  for (size_t i = 0; i < M; ++i)
    entries[i] = &(*this)[i];

  entries[M] = &new_entry;

  size_t k = M - 2*m + 2;
  Hyperrectangle<N> first_group_hr, second_group_hr;
  size_t best_axis_idx;
  float margin, min_margin = FLT_MAX;

  for (size_t i = 0; i < N; ++i) {
    std::sort(entries.begin(), entries.end(),
    [&](const SpatialObject*& lhs, const SpatialObject*& rhs) {
      return lhs->box[i].begin() < rhs->box[i].begin();
    });

    margin = 0.f;

    for (size_t j = 1; j <= k; ++j) {
      for (size_t f1 = 0; f1 < m - 1 + j; ++f1)
        first_group_hr.adjust(entries[f1]->box);

      for (size_t f2 = m - 1 + j; f2 < M + 1; ++f2)
        second_group_hr.adjust(entries[f2]->box);

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
  std::vector<const SpatialObject*> entries(this->size + 1);
  std::vector<const SpatialObject*> best_distribution;
  auto new_node = std::make_shared<XNode>();

  for (size_t i = 0; i < M; ++i)
    entries[i] = &(*this)[i];

  entries[M] = &new_entry;

  size_t k = M - 2*m + 2;
  Hyperrectangle<N> first_group_hr, second_group_hr;

  float first_group_area, second_group_area;
  float group_overlap, total_area;
  float min_overlap, min_total_area;
  min_overlap = min_total_area = FLT_MAX;
  size_t second_group_idx;

  std::sort(entries.begin(), entries.end(),
  [&](const SpatialObject*& lhs, const SpatialObject*& rhs) {
    return lhs->box[axis].begin() < rhs->box[axis].begin();
  });

  for (size_t j = 1; j <= k; ++j) {
    for (size_t f1 = 0; f1 < m - 1 + j; ++f1)
      first_group_hr.adjust(entries[f1]->box);

    for (size_t f2 = m - 1 + j; f2 < M + 1; ++f2)
      second_group_hr.adjust(entries[f2]->box);

    first_group_area = first_group_hr.getArea();
    second_group_area = second_group_hr.getArea();
    total_area = first_group_area + second_group_area;
    group_overlap = overlap(first_group_hr, second_group_hr);

    if (group_overlap < min_overlap ||
        (group_overlap == min_overlap && total_area < min_total_area)) {
      best_distribution = entries;
      second_group_idx = m - 1 + j;
    }

    first_group_hr.reset();
    second_group_hr.reset();
  }

  std::vector<SpatialObject> new_entry_order;

  for (size_t i = 0; i < M + 1; ++i)
    new_entry_order.push_back(*(best_distribution[i]));

  this->size = second_group_idx;

  for (size_t i = 0; i < second_group_idx; ++i)
    this->entries[i] = new_entry_order[i];

  for (size_t i = second_group_idx; i < M + 1; ++i)
    new_node->insert(new_entry_order[i]);

  return new_node;
}

template <size_t N, typename ElemType, size_t M, size_t m>
std::shared_ptr<typename XNODE> XNODE::insert(const SpatialObject& new_entry) {
  if (size < M) {
    entries[size++] = new_entry;
    return nullptr;
  }

  auto split_axis = chooseSplitAxis(new_entry);
  return chooseSplitIndex(split_axis, new_entry);
}

template <size_t N, typename ElemType, size_t M, size_t m>
void XNODE::pickSeeds(std::vector<SpatialObject>& entries, size_t* first,
                      size_t* second) {
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
void XNODE::pickNext(std::vector<SpatialObject>& entries, size_t* idx,
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
