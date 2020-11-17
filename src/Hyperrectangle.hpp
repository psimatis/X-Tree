#include "Interval.hpp"
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

  float get_area() const;

  void reset();

  void adjust(const Hyperrectangle& other);

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
Hyperrectangle<N>& Hyperrectangle<N>::operator=(const Hyperrectangle& other) {
  std::copy(other.begin(), other.end(), begin());
  return *this;
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
  float area = 1.f;

  for (const auto& interval : bounds)
    area *= (interval.end() - interval.begin());

  return area;
}

template <size_t N>
void Hyperrectangle<N>::reset() {
  for (auto& interval : bounds)
    interval.reset();
}

template <size_t N>
void Hyperrectangle<N>::adjust(const Hyperrectangle& other) {
  for (size_t i = 0; i < N; ++i) {
    (*this).at(i).begin() = std::min((*this)[i].begin(), other[i].begin());
    (*this).at(i).end() = std::min((*this)[i].end(), other[i].end());
  }
}
