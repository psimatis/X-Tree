#pragma once
#include "interval/Interval.hpp"
#include <array>

template <size_t N>
struct Hyperrectangle {
  typedef Interval* iterator;
  typedef const Interval* const_iterator;

  Interval& operator[](size_t index);
  Interval operator[](size_t index) const;

  Hyperrectangle& operator=(const Hyperrectangle& other);

  iterator begin();
  iterator end();

  const_iterator begin() const;
  const_iterator end() const;

  float getArea() const;

  void reset();

  void adjust(const Hyperrectangle& other);

 private:
  std::array<Interval, N> bounds;
};

#include "Hyperrectangle.tpp"
