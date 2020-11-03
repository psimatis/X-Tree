#pragma once
#include <array>
#include <float.h>

struct Interval {
  Interval(float min, float max);
  Interval(const Interval& other);
  virtual ~Interval();

  float& begin();
  float begin() const;

  float& end();
  float end() const;

  float get_range() const;

  void reset();

  private:
    std::array<float, 2> limits;
};

Interval::Interval(float min, float max) {
  if (min > max)
    throw std::invalid_argument("Lower limit of an interval cannot be greater than the upper limit.\n");

  this->limits[0] = min;
  this->limits[1] = max;
}

Interval::Interval(const Interval& other) {
  this->limits[0] = other.limits[0];
  this->limits[1] = other.limits[0];
}

Interval::~Interval() {}

void Interval::reset() {
  limits[0] = FLT_MAX;
  limits[1] = FLT_MIN;
}
