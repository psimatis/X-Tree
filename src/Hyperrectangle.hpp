#include "Interval.hpp"
#include <array>

template <size_t N>
struct Hyperrectangle {
  typedef Interval* iterator;
  typedef const Interval* const_iterator;

  Hyperrectangle(std::array<Interval, N> bounds);
  Hyperrectangle(const Hyperrectangle& other);
  virtual ~Hyperrectangle();

  iterator begin();
  iterator end();

  const_iterator begin() const;
  const_iterator end() const;

  float get_area() const;

  void reset();

  private:
    std::array<Interval, N> bounds;
};

template <size_t N>
Hyperrectangle<N>::Hyperrectangle(std::array<Interval, N> bounds) {
}

template <size_t N>
Hyperrectangle<N>::Hyperrectangle(const Hyperrectangle& other) {
}

template <size_t N>
Hyperrectangle<N>::~Hyperrectangle() {}
