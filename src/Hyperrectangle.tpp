#pragma once

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
float Hyperrectangle<N>::getArea() const {
  float area = 1.f;

  for (const auto& interval : bounds)
    area *= (interval.end() - interval.begin());

  return area;
}

template <size_t N>
float Hyperrectangle<N>::getMargin() const {
  float margin = 0.f;

  for (const auto& interval : bounds)
    margin += (interval.end() - interval.begin());
  margin *= (1 << (N - 1));

  return margin;
}

template <size_t N>
void Hyperrectangle<N>::reset() {
  for (auto& interval : bounds)
    interval.reset();
}

template <size_t N>
void Hyperrectangle<N>::adjust(const Hyperrectangle& other) {
  for (size_t i = 0; i < N; ++i) {
    (*this)[i].begin() = std::min((*this)[i].begin(), other[i].begin());
    (*this)[i].end() = std::max((*this)[i].end(), other[i].end());
  }
}

template <size_t N>
float overlap(const Hyperrectangle<N>& hr1, const Hyperrectangle<N>& hr2) {
  float overlap = 1.f;

  for (size_t i = 0; i < N; ++i) {
    float length = 0.f;

    if ((hr2.begin() < hr1.end() && hr1.begin() < hr2.end())
        || (hr1.begin() < hr2.end() && hr2.begin() < hr1.end()))
      length = std::min(hr1.end(), hr2.end())
               - std::max(hr1.begin(), hr2.begin());

    overlap *= length;
  }

  return overlap;
}
