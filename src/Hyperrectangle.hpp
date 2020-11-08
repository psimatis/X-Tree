#include "Interval.hpp"
#include <array>

template <size_t N>
struct Hyperrectangle {
  typedef Interval* iterator;
  typedef const Interval* const_iterator;

  Interval& operator[](size_t index);
  Interval operator[](size_t index) const;

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
Interval& Hyperrectangle<N>::operator[](size_t index) {
  return bounds[index];
}

template <size_t N>
Interval Hyperrectangle<N>::operator[](size_t index) const {
  return bounds[index];
}

template <size_t N>
typename Hyperrectangle<N>::iterator Hyperrectangle<N>::begin() {
  return bounds.begin();
}

template <size_t N>
typename Hyperrectangle<N>::iterator Hyperrectangle<N>::end() {
  return bounds.end();
}

template <size_t N>
typename Hyperrectangle<N>::const_iterator Hyperrectangle<N>::begin() const {
  return bounds.begin();
}

template <size_t N>
typename Hyperrectangle<N>::const_iterator Hyperrectangle<N>::end() const {
  return bounds.end();
}

template <size_t N>
float Hyperrectangle<N>::get_area() const {
  return 0.f;
}

template <size_t N>
void Hyperrectangle<N>::reset() {}
