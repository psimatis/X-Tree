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

int rootAdjust = 0;

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

template <size_t N>
float getAreaEnlargement(const Hyperrectangle<N>& container, const Hyperrectangle<N>& item) {
  Hyperrectangle<N> enlarged_container = container;
  enlarged_container.adjust(item);
  return enlarged_container.getArea() - container.getArea();
}

template <size_t N>
float getAreaEnlargement(const Hyperrectangle<N>& container, float containerArea, const Hyperrectangle<N>& item) {
    Hyperrectangle<N> enlarged_container = container;
    enlarged_container.adjust(item);
    return enlarged_container.getArea() - containerArea;
}

template <size_t N, typename ElemType, size_t M, size_t m>
size_t getMinOverlapHyperrectangle(shared_ptr<typename XNODE> node, const Hyperrectangle<N>& box) {

    size_t idx;
    float min_overlap, min_area, min_box_area, areaEnlargement, boxArea;
    min_overlap = min_area = min_box_area = FLT_MAX;
    Hyperrectangle<N> b;
    vector<vector<float>> overlaps;
    vector<float> tot_overlaps(node->size, 0);

    for (size_t i = 0; i < node->size; ++i) {
        vector<float> tmp(node->size, 0);
        overlaps.push_back(tmp);
        for (size_t j = 0; j < i; ++j)
            tot_overlaps[i] += overlaps[i][j];
        for (size_t j = i + 1; j < node->size; ++j) {
            overlaps[i][j] = overlap((*node)[i].box, (*node)[j].box);
            tot_overlaps[i] += overlaps[i][j];
        }

        b = node->entries[i].box;
        boxArea = b.getArea();
        areaEnlargement = getAreaEnlargement(b, boxArea, box);

        if (tot_overlaps[i] < min_overlap ||
            (tot_overlaps[i] == min_overlap && areaEnlargement < min_area) ||
            (tot_overlaps[i] == min_overlap && areaEnlargement < min_area && boxArea < min_box_area)) {
            idx = i;
            min_overlap = tot_overlaps[i];
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
shared_ptr<typename XNODE> XTree<N, ElemType, M, m>::chooseNode(const shared_ptr<XNode>& current_node, const Hyperrectangle<N>& box, SpatialObject*& entry) {
    shared_ptr<XNode> node;

    if ((*current_node)[0].child_pointer->isLeaf()) {
        auto idx_least_overlap = getMinOverlapHyperrectangle<N, ElemType, M, m>(current_node, box);
        auto& chosen_entry = (*current_node)[idx_least_overlap];
        node = chosen_entry.child_pointer;
        entry = &chosen_entry;

    } else {
        float min_area, min_enlargement;
        min_area = min_enlargement = FLT_MAX;

        for (SpatialObject& current_entry : *current_node) {
            auto area_enlargement = getAreaEnlargement(current_entry.box, box);
            auto area = current_entry.box.getArea();

            if (area_enlargement < min_enlargement || (area_enlargement == min_enlargement && area < min_area)) {
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

template <size_t N>
float objectDist(const Hyperrectangle<N>& point, const Hyperrectangle<N>& obj);

template <size_t N>
float minDist(const Hyperrectangle<N>& lhs, const Hyperrectangle<N>& rhs);

template <size_t N>
float minMaxDist(const Hyperrectangle<N>& lhs, const Hyperrectangle<N>& rhs);

template <size_t N, typename ElemType, size_t M, size_t m>
vector<string> XTree<N, ElemType, M, m>::kNNQuery(Hyperrectangle<DIM>& q, int k) {
    class Compare{
    public:
        bool operator()(pair<string, float> &l, pair<string, float> &r) const
        { return l.second < r.second; }
    };
    class Compare2{
    public:
        bool operator()(pair<shared_ptr<XNode>, float> &l, pair<shared_ptr<XNode>, float> &r) const
        { return l.second > r.second; }
    };
    priority_queue<pair<string, float>, vector<pair<string, float>>, Compare> knnPts;
    priority_queue<pair<shared_ptr<XNode>, float>, vector<pair<shared_ptr<XNode>, float>>, Compare2>  toVisit;
    toVisit.push(make_pair(root, 0));
    for (int i = 0; i < k; i++) knnPts.push(make_pair("", FLT_MAX));
    float dist, minD;
    while (!toVisit.empty()) {
        shared_ptr<XNode> n = toVisit.top().first;
        toVisit.pop();
        for (int i = 0; i < n->size; i++) {
            minD = knnPts.top().second;
            if (n->isLeaf()) dist = objectDist(n->entries[i].box, q);
            else dist = minDist(q, n->entries[i].box);
            if (dist < minD) {
                if (n->isLeaf()) {
                    knnPts.pop();
                    knnPts.push(make_pair(n->entries[i].identifier, dist));
                    queryLeafCount++;
                } else toVisit.push(make_pair(n->entries[i].child_pointer, dist));
            }
        }
    }
    vector<string> res;
    for (size_t i = 0; i < k; ++i) {
        res.push_back(knnPts.top().first);
        knnPts.pop();
    }
    return res;
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
        queryLeafCount++;
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

template <size_t N, typename ElemType, size_t M, size_t m>
void XTree<N, ElemType, M, m>::getNodeCount(const shared_ptr<XNode> n, int &lCount, int &dCount) {
    if (n->isLeaf()) lCount++;
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
    cout << "Entries: " << entry_count << endl;
    cout << "Capacity: " << CAPACITY << endl;
    cout << "Dimensions: " << DIM << endl;
    cout << "Max overlap: " << MAX_OVERLAP << endl;
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