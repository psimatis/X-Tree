#pragma once

using namespace std;

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
void XTree<N, ElemType, M, m>::insert(const Hyperrectangle<N>& box, const ElemType& value) {
  auto split_node_and_axis = chooseLeaf(root, box, value);
  ++entry_count;

  if (!split_node_and_axis) return;

  auto new_root = std::make_shared<XNode>();
  new_root->entries[0].child_pointer = root;
  ++new_root->size;
  adjustTree(new_root, root, split_node_and_axis, &new_root->entries[0]);
  root = new_root;
}

template <size_t N, typename ElemType, size_t M, size_t m>
float getTotalOverlap(const Hyperrectangle<N>& box,
                      const size_t& idx,
                      const std::shared_ptr<typename XNODE>& node) {
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
size_t getMinOverlapHyperrectangle(std::shared_ptr<typename XNODE> node,
                                   const Hyperrectangle<N>& box) {
  auto& entries = node->entries;
  size_t idx;
  float min_overlap, min_area, min_box_area;
  min_overlap = min_area = min_box_area = FLT_MAX;

  for (size_t i = 0; i < node->size; ++i) {
    auto entry = entries.at(i);
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
std::shared_ptr<std::pair<std::shared_ptr<typename XNODE>, size_t>>XTree<N, ElemType, M, m>::chooseLeaf(
      const std::shared_ptr<XNode>& n,
      const Hyperrectangle<N>& box,
      const ElemType& value) {
  if (!n->isLeaf()) {
    SpatialObject* entry;
    auto next_node = chooseNode(n, box, entry);
    auto split_node_and_axis = chooseLeaf(next_node, box, value);
    return adjustTree(n, next_node, split_node_and_axis, entry);
  }

  SpatialObject new_entry;
  new_entry.box = box;
  new_entry.identifier = value;
  return n->insert(new_entry);
}

template <size_t N, typename ElemType, size_t M, size_t m>
std::shared_ptr<typename XNODE> XTree<N, ElemType, M, m>::chooseNode(
    const std::shared_ptr<XNode>& current_node,
    const Hyperrectangle<N>& box,
    SpatialObject*& entry) {
  std::shared_ptr<XNode> node;

  if ((*current_node)[0].child_pointer->isLeaf()) {
    auto idx_least_overlap = getMinOverlapHyperrectangle<N, ElemType, M, m>
                             (current_node, box);
    auto& chosen_entry = (*current_node)[idx_least_overlap];
    node = chosen_entry.child_pointer;
    entry = &chosen_entry;
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
std::shared_ptr<std::pair<std::shared_ptr<typename XNODE>, size_t>>
    XTree<N, ElemType, M, m>::adjustTree(
      const std::shared_ptr<XNode>& parent,
      const std::shared_ptr<XNode>& left,
      const std::shared_ptr<std::pair<std::shared_ptr<XNode>, size_t>>& right,
      SpatialObject* entry) {
  entry->box.reset();

  for (SpatialObject current_entry : *left)
    entry->box.adjust(current_entry.box);

  if (!right)
    return nullptr;

  SpatialObject new_entry;
  new_entry.box.reset();

  for (SpatialObject& current_entry : *(right->first))
    new_entry.box.adjust(current_entry.box);

  parent->split_history.insert(right->second, left, right->first);
  new_entry.child_pointer = right->first;
  return parent->insert(new_entry);
}

template <size_t N, typename ElemType, size_t M, size_t m>
vector<string> XTree<N, ElemType, M, m>::kNN(Hyperrectangle<DIM>& point, int k) {
    vector<string> res;
    for (size_t i = 0; i < k; ++i)
        kNN_result.push(make_pair(nullptr, FLT_MAX));

    kNNProcess(root, point, k);

    for (size_t i = 0; i < k; ++i) {
        auto entry = kNN_result.top().first;
        res.push_back(entry->identifier);
        kNN_result.pop();
    }
    return res;
}

template <size_t N>
float objectDist(const Hyperrectangle<N>& point, const Hyperrectangle<N>& obj);
template <size_t N>
float minDist(const Hyperrectangle<N>& lhs, const Hyperrectangle<N>& rhs);
template <size_t N>
float minMaxDist(const Hyperrectangle<N>& lhs, const Hyperrectangle<N>& rhs);

template <size_t N, typename ElemType, size_t M, size_t m>
class kNN_comparison {
 public:
  bool operator()
    (const std::pair<const typename XTree<N, ElemType, M, m>::SpatialObject*, float>& lhs,
     const std::pair<const typename XTree<N, ElemType, M, m>::SpatialObject*, float>& rhs) {
    return lhs.second < rhs.second;
  }
};

template <size_t N, typename ElemType, size_t M, size_t m>
void XTree<N, ElemType, M, m>::kNNProcess(const shared_ptr<XNode> n, const Hyperrectangle<N>& point, size_t k) {
    float dist, min_dist, min_max_dist;
    size_t last;

    if (n->isLeaf()) {
        for (const auto& e : n->entries) {
            dist = objectDist(point, e.box);

            if (kNN_result.top().second > dist) {
                kNN_result.pop();
                kNN_result.push(std::make_pair(&e, dist));
            }
        }
    }
    else {
        vector<tuple<shared_ptr<XNode>, float, float>> branchList;

        for (size_t i = 0; i < n->size; ++i) {
            auto entry = n->entries[i];
            min_dist = minDist(point, entry.box);
            min_max_dist = minMaxDist(point, entry.box);

            // Tuple <Node, minDist, minMaxDist>
            branchList.push_back(std::make_tuple(entry.child_pointer, min_dist,min_max_dist));
        }

        sort(branchList.begin(), branchList.end(),
                  [](const std::tuple<std::shared_ptr<XNode>, float, float>& lhs,
                     const std::tuple<std::shared_ptr<XNode>, float, float>& rhs) {
                      return std::get<1>(lhs) < std::get<1>(rhs);
                  });
        last = branchList.size();

        // Discard MBRs. Strategy 1
        for (size_t i = 0; i < branchList.size(); ++i) {
            for (size_t j = 0; j < branchList.size(); ++j) {
                if (i == j) continue;

                if (std::get<2>(branchList[i]) < std::get<1>(branchList[j])) {
                    branchList.erase(branchList.begin() + j);
                    --last;
                }
            }
        }

        for (size_t i = 0; i < branchList.size(); ++i) {
            auto next_node = std::get<0>(branchList[i]);
            kNNProcess(next_node, point, k);
            for (size_t j = 0; j < branchList.size(); ++j) {
                if (std::get<1>(branchList[j]) > kNN_result.top().second)
                    branchList.erase(branchList.begin() + j);
            }
        }
    }
}

bool contains(Hyperrectangle<DIM> r, Hyperrectangle<DIM> qr){
    for (int i = 0; i < DIM; i++){
        if(!(qr[i].begin() <= r[i].begin() && r[i].end() <= qr[i].end())) return false;
    }
    return true;
}

bool intersects(Hyperrectangle<DIM> r, Hyperrectangle<DIM> qr){
    bool intersect = true;
    for (int i = 0; i < DIM; i++){
        intersect = intersect && !(r[i].begin() > qr[i].end()) && !(qr[i].begin() > r[i].end());
        if (!intersect) return false;
    }
    return true;
}

template <size_t N, typename ElemType, size_t M, size_t m>
void XTree<N, ElemType, M, m>::rangeSearch(const shared_ptr<XNode> n, Hyperrectangle<DIM> qr, vector<string> &result) {
    if (n->isLeaf()) {
        for (size_t i = 0; i < n->size; ++i) {
            if (contains(n->entries[i].box, qr))
                result.push_back(n->entries[i].identifier);
        }
    }
    else{
        for (size_t i = 0; i < n->size; ++i){
            if (intersects(n->entries[i].box, qr))
                rangeSearch(n->entries[i].child_pointer, qr, result);
        }
    }
}

template <size_t N, typename ElemType, size_t M, size_t m>
vector<string> XTree<N, ElemType, M, m>::rangeQuery(Hyperrectangle<DIM> qr) {
    vector<string> result;
    rangeSearch(root, qr, result);
    return result;
}

template <size_t N>
float objectDist(const Hyperrectangle<N>& point, const Hyperrectangle<N>& obj) {
  float obj_dist = 0;
  float dim_dist;

  for (size_t i = 0; i < N; ++i) {
    dim_dist = point[i].begin() - obj[i].begin();
    obj_dist += dim_dist*dim_dist;
  }

  return obj_dist;
}

template <size_t N>
float minDist(const Hyperrectangle<N>& point, const Hyperrectangle<N>& hr) {
  float dist = 0, r_i, dim_dist;

  for (size_t i = 0; i < N; ++i) {
    if (point[i].begin() < hr[i].begin())
      r_i = hr[i].begin();
    else if (point[i].begin() > hr[i].end())
      r_i = hr[i].end();
    else
      r_i = point[i].begin();

    dim_dist = point[i].begin() - r_i;
    dist += dim_dist*dim_dist;
  }

  return dist;
}

template <size_t N>
float minMaxDist(const Hyperrectangle<N>& point, const Hyperrectangle<N>& hr) {
  float min_max_dist = FLT_MAX;
  float dist = 0;
  float rm_k, rM_i;
  float tmp_dist;

  for (size_t i = 0; i < N; ++i) {
    if (point[i].begin() <= (hr[i].begin() + hr[i].end())/2)
      rm_k = hr[i].begin();
    else
      rm_k = hr[i].end();

    tmp_dist = point[i].begin() - rm_k;
    dist = tmp_dist*tmp_dist;

    for (size_t j = 0; j < N; ++j) {
      if (i == j) continue;

      if (point[i].begin() >= (hr[i].begin() + hr[i].end())/2)
        rM_i = hr[i].begin();
      else
        rM_i = hr[i].end();

      tmp_dist = point[i].begin() - rM_i;
      dist += tmp_dist*tmp_dist;
    }

    if (dist < min_max_dist)
      min_max_dist = dist;
  }

  return min_max_dist;
}

template <size_t N, typename ElemType, size_t M, size_t m>
void XTree<N, ElemType, M, m>::getNodeCount(const shared_ptr<XNode> n, int &lCount, int &dCount) {
    if (n->isLeaf()) {
        lCount++;
    }
    else{
        dCount++;
        for (size_t i = 0; i < n->size; ++i)
            getNodeCount(n->entries[i].child_pointer, lCount, dCount);
    }
}

template <size_t N, typename ElemType, size_t M, size_t m>
void XTree<N, ElemType, M, m>::getHeight(const shared_ptr<XNode> n, int &h){
    h++;
    if (n->isLeaf()) return;
    getHeight(n->entries[0].child_pointer, h);
}

template <size_t N, typename ElemType, size_t M, size_t m>
void XTree<N, ElemType, M, m>::getStats() {
    int l = 0, d = 0, h = 0;
    getNodeCount(root, l, d);
    getHeight(root, h);
    cout << "Height: " << h << endl;
    cout << "Leaves: " << l << endl;
    cout << "Directories: " << d << endl;
    cout << "Point count: " << entry_count << endl;
}

template <size_t N, typename ElemType, size_t M, size_t m>
void XTree<N, ElemType, M, m>::snapshot(const shared_ptr<XNode> n) {
    if (n->isLeaf()) cout << "leaf " << n->size <<  endl;
    else{
        cout << "internal " << n->size << endl;
        for (size_t i = 0; i < n->size; ++i)
            snapshot(n->entries[i].child_pointer);
    }
}