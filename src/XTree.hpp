#pragma once
#include <memory>
#include <vector>
#include <float.h>
#include <algorithm>
#include <stack>

#include "Hyperrectangle.hpp"

#define MAX_OVERLAP 0.2f

template <size_t N, typename ElemType, size_t M, size_t m = size_t(M*0.4)>
struct XTree {
  struct XNode;

  struct SpatialObject {
    Hyperrectangle<N> box;
    ElemType identifier;
    std::shared_ptr<XNode> child_pointer;
  };

  struct SplitHistory {
    struct SHNode {
      SHNode(size_t dim);

      size_t dim;
      std::shared_ptr<XNode> related_xnode;
      std::shared_ptr<SHNode> nodes[2];
    };

    SplitHistory();
    bool find(std::shared_ptr<SHNode> current_node,
              std::shared_ptr<XNode> target_node, std::stack<SHNode*>& path);
    void insert(size_t dim, std::shared_ptr<XNode> left,
                std::shared_ptr<XNode> right);
    bool getCommonSplitAxis();

    std::shared_ptr<SHNode> root;
  };

  struct XNode {
    typedef typename std::vector<SpatialObject>::iterator iterator;
    typedef typename std::vector<SpatialObject>::const_iterator const_iterator;

    XNode();

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    SpatialObject& operator[](size_t index);
    SpatialObject operator[](size_t index) const;

    bool isLeaf();
    void pickSeeds(std::vector<SpatialObject>& entries, size_t* first,
                   size_t* second);
    void pickNext(std::vector<SpatialObject>& entries, size_t* idx,
                  const Hyperrectangle<N>& mbb_group1,
                  const Hyperrectangle<N>& mbb_group2);

    std::shared_ptr<std::pair<std::shared_ptr<XNode>, size_t>> insert(
          const SpatialObject& new_entry);
    std::shared_ptr<std::pair<std::shared_ptr<XNode>, size_t>> topological_split(
          const SpatialObject& new_entry);
    size_t chooseSplitAxis(const SpatialObject& new_entry);
    std::shared_ptr<XNode> chooseSplitIndex(size_t axis,
                                            const SpatialObject& new_entry);
    std::shared_ptr<std::pair<std::shared_ptr<XNode>, size_t>> overlap_minimal_split(
          const SpatialObject& new_entry);

    std::vector<SpatialObject> entries;
    size_t size;
    SplitHistory split_history;
  };

  XTree();
  virtual ~XTree();

  size_t dimension() const;
  size_t size() const;
  bool empty() const;

  // split();
  // topological_split();
  // overlap_minimal_split();
  // create_supernode();

  void insert(const Hyperrectangle<N>& box, const ElemType& value);
  std::shared_ptr<std::pair<std::shared_ptr<XNode>, size_t>> chooseLeaf(
        const std::shared_ptr<XNode>& current_node,
        const Hyperrectangle<N>& box,
        const ElemType& value);

  std::shared_ptr<XNode> chooseNode(const std::shared_ptr<XNode>& current_node,
                                    const Hyperrectangle<N>& box,
                                    SpatialObject*& entry);

  std::shared_ptr<std::pair<std::shared_ptr<XNode>, size_t>> adjustTree(
        const std::shared_ptr<XNode>& current_node,
        const std::shared_ptr<XNode>& left,
        const std::shared_ptr<std::pair<std::shared_ptr<XNode>, size_t>>& right,
        SpatialObject* entry);

  // std::vector<ElemType>& kNN(const Point& point);

  std::shared_ptr<XNode> root;
  size_t entry_count;
  std::vector<ElemType> query_result;
};

#define XNODE XTree<N, ElemType, M, m>::XNode
#define SH XTree<N, ElemType, M, m>::SplitHistory

#include "XNode.tpp"
#include "SplitHistory.tpp"
#include "XTree.tpp"
