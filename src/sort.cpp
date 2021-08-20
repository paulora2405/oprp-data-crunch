#include "sort.hpp"

#include <omp.h>

namespace sort {

int partition(std::vector<double> &v, int start, int end) {
  int pivot = end;
  int j = start;
  for(int i = start; i < end; ++i) {
    if(v[i] < v[pivot]) {
      std::swap(v[i], v[j]);
      ++j;
    }
  }
  std::swap(v[j], v[pivot]);
  return j;
}

void quicksort(std::vector<double> &v, int start, int end) {
  if(start < end) {
    int p = partition(v, start, end);
#pragma omp task final((p - start) < omp_get_num_threads()) mergeable default(none) shared(v) \
    firstprivate(start, p)
    quicksort(v, start, p - 1);
#pragma omp task final((end - p) < omp_get_num_threads()) mergeable default(none) shared(v) \
    firstprivate(end, p)
    quicksort(v, p + 1, end);
  }
}

}  // namespace sort