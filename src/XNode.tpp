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
  float min_goodness_value = FLT_MAX;

  float first_group_area, second_group_area, group_overlap;
  float goodness_value;

  for (size_t i = 0; i < N; ++i) {
    std::sort(entries.begin(), entries.end(),
    [&](const SpatialObject*& lhs, const SpatialObject*& rhs) {
      return lhs->box[i].begin() < rhs->box[i].begin();
    });

    for (size_t j = 1; j <= k; ++j) {
      for (size_t f1 = 0; f1 < m - 1 + j; ++f1)
        first_group_hr.adjust(entries[f1]->box);

      for (size_t f2 = m - 1 + j; f2 < M + 1; ++f2)
        second_group_hr.adjust(entries[f2]->box);

      first_group_area = first_group_hr.getArea();
      second_group_area = second_group_hr.getArea();
      group_overlap = overlap(first_group_hr, second_group_hr);
      goodness_value = first_group_area + second_group_area + group_overlap;

      if (goodness_value < min_goodness_value) {
        min_goodness_value = goodness_value;
        best_axis_idx = i;
      }
    }

    first_group_hr.reset();
    second_group_hr.reset();

    std::sort(entries.begin(), entries.end(),
    [&](const SpatialObject*& lhs, const SpatialObject*& rhs) {
      return lhs->box[i].end() < rhs->box[i].end();
    });

    for (size_t j = 1; j <= k; ++j) {
      for (size_t f1 = 0; f1 < m - 1 + j; ++f1)
        first_group_hr.adjust(entries[f1]->box);

      for (size_t f2 = m - 1 + j; f2 < M + 1; ++f2)
        second_group_hr.adjust(entries[f2]->box);

      first_group_area = first_group_hr.getArea();
      second_group_area = second_group_hr.getArea();
      group_overlap = overlap(first_group_hr, second_group_hr);
      goodness_value = first_group_area + second_group_area + group_overlap;

      if (goodness_value < min_goodness_value) {
        min_goodness_value = goodness_value;
        best_axis_idx = i;
      }
    }

    first_group_hr.reset();
    second_group_hr.reset();
  }

  return best_axis_idx;
}

template <size_t N, typename ElemType, size_t M, size_t m>
std::shared_ptr<typename XNODE> XNODE::insert(const SpatialObject& new_entry) {
  if (size < M) {
    entries[size++] = new_entry;
    return nullptr;
  }

  auto split_axis = chooseSplitAxis(new_entry);

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
