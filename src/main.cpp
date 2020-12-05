#include <iostream>
#include "Hyperrectangle.hpp"
#include "XTree.hpp"

int main() {
  Hyperrectangle<3> h;
  h[0] = Interval(3, 4);
  h[1] = Interval(0, 5);
  h[2] = Interval(6, 8);

  for (const auto& interval : h)
    std::cout << "Interval begin: " << interval.begin() << ", interval end: " << interval.end() << "\n";

  std::cout << "Hyperrectangle area: " << h.getArea() << "\n";

  Hyperrectangle<3> h2;
  h2 = h;

  std::cout << "h2:\n";
  for (const auto& interval : h2)
    std::cout << "Interval begin: " << interval.begin() << ", interval end: " << interval.end() << "\n";

  std::cout << "Hyperrectangle2 area: " << h2.getArea() << "\n";

  XTree<3, int, 6> xt;
}
