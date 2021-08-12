#ifndef SORT_HPP
#define SORT_HPP

#include <string>
#include <vector>

namespace sort {

int partition(std::vector<double> &v, int start, int end);

void quicksort(std::vector<double> &v, int start, int end);

}  // namespace sort

#endif