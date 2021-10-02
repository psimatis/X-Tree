#pragma once

#include "def.h"
#include "Interval.hpp"

template <size_t N>
struct Hyperrectangle {
  typedef Interval* iterator;
  typedef const Interval* const_iterator;

  Interval& operator[](size_t index);
  Interval operator[](size_t index) const;

  Hyperrectangle();
  Hyperrectangle(const Hyperrectangle& other);
  Hyperrectangle& operator=(const Hyperrectangle& other);
  ~Hyperrectangle();

  iterator begin();
  iterator end();

  const_iterator begin() const;
  const_iterator end() const;

  float getArea() const;

  float getMargin() const;

  void reset();

  void adjust(const Hyperrectangle& other);

 //private:
  std::array<Interval, N> bounds;

    void print();
};

#include "Hyperrectangle.tpp"
