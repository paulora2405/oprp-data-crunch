#include <mpi.h>
#include <omp.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace TIMER {

template <typename Clock = std::chrono::high_resolution_clock>
class stopwatch {
  const typename Clock::time_point start_point;
  bool printName;
  std::string name;

public:
  stopwatch(std::string name, bool printName = true)
      : start_point(Clock::now()),
        printName{printName},
        name{name} {}
  ~stopwatch() {
    if(printName)
      std::cout << name << " - Time elapsed: "
                << this->elapsed_time<unsigned int, std::chrono::microseconds>() * 10e-7
                << " seconds.\n";
    else
      std::cout << "t=" << this->elapsed_time<unsigned int, std::chrono::microseconds>() * 10e-7
                << "s\n";
  }

  template <typename Rep = typename Clock::duration::rep, typename Units = typename Clock::duration>
  Rep elapsed_time() const {
    std::atomic_thread_fence(std::memory_order_relaxed);
    auto counted_time = std::chrono::duration_cast<Units>(Clock::now() - start_point).count();
    std::atomic_thread_fence(std::memory_order_relaxed);
    return static_cast<Rep>(counted_time);
  }
};

using precise_stopwatch = stopwatch<>;
using system_stopwatch = stopwatch<std::chrono::system_clock>;
using monotonic_stopwatch = stopwatch<std::chrono::steady_clock>;

}  // namespace TIMER

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

namespace dc {

std::pair<double, double> avg_and_geo(std::vector<double> &data) {
  int N = data.size();
  double avg = 0.0, prod = 1.0;
  for(double value : data) {
    avg += (value / N);
    prod *= std::pow(value, (1.0 / static_cast<double>(N)));
  }

  return std::make_pair(avg, prod);
}

void dataManipulation(std::vector<double> &data) {
  sort::quicksort(data, 0, data.size() - 1);

  bool ordered = true;
  for(size_t i = 0; i < 1000; i++) {
    if(data[i] > data[i + 1])
      ordered = false;
  }

  if(!ordered) {
    std::cerr << data.size() << " nao ordenado\n";
  }

  std::pair<double, double> p = avg_and_geo(data);

  const double ma = p.first;
  const double std = [&data, &ma]() {
    double std = 0.0;
    for(size_t i = 0; i < data.size(); ++i)
      std += std::pow(data[i] - ma, 2);
    return sqrt(std / data.size());
  }();
  const double mg = p.second;
  const double md = data[data.size() / 2 + 1];
  const double p95 = data[(95 / 100) * data.size()];
  const double min = data.front();
  const double max = data.back();

  std::cout << 'N' << data.size() << std::setprecision(12) << ".txt " << ma << ' ' << std << ' '
            << mg << ' ' << md << ' ' << p95 << ' ' << min << ' ' << max << ' ';  //<< '\n';

  /* N0.txt MA DP MG MD 95p min max */
}

std::vector<double> ingestData(int N) {
  FILE *is;
  std::string filename = "entrada/" + std::to_string(N) + ".txt";
  is = fopen(filename.c_str(), "r");
  if(!is) {
    std::cerr << "Erro ao abrir o arquivo";
    exit(EXIT_FAILURE);
  }

  std::vector<double> data(static_cast<std::size_t>(N));
  std::size_t i = 0;

  {
    TIMER::precise_stopwatch timer("Ingest" + std::to_string(N));

    while((fscanf(is, "%lf", &data[i++])) != EOF) {
    }
  }

  return data;
}

void entryPoint(int rank, int mpiSize) {
  int tag = 0;
  if(rank == 0) {
    const std::vector<uint32_t> entradas{
        1000003,  2000003,  3000003,  4000003,                       //
        10000001, 10000003, 10000005, 10000011, 10000021, 10000055,  //
        12000123, 12000155, 13000155, 13010155, 15000121,            //
    };
    std::vector<std::vector<double>> dataVectors;
    {
      TIMER::precise_stopwatch timer("Total Ingestion");
#pragma omp parallel
      {
#pragma omp for
        for(auto entrada : entradas) {
          dataVectors.push_back(ingestData(entrada));
        }
      }
    }

    {
      TIMER::precise_stopwatch timer("Total Manipulation");

      for(int i = 1; i < mpiSize; ++i) {
        {
          // TIMER::precise_stopwatch timer("DataMani" + std::to_string(data.size()), false);
          // dataManipulation(data);
          int vecSize = dataVectors[i].size();
          MPI_Send(&vecSize, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
          MPI_Send(&dataVectors[i - 1][0], vecSize, MPI_DOUBLE, i, tag, MPI_COMM_WORLD);
        }
      }
    }
  }  // fi(rank == 0)

  if(rank != 0) {
    int vecSize = 0;
    MPI_Recv(&vecSize, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    double tmp[vecSize];
    std::vector<double> data;
    data.resize(vecSize);
    MPI_Recv(&tmp[0], vecSize, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    for(size_t i = 0; i < vecSize; i++) {
      data[i] = tmp[i];
    }

    {
      TIMER::precise_stopwatch timer("DataMani" + std::to_string(data.size()), false);
      dataManipulation(data);
    }
  }
}

}  // namespace dc

int main(int argc, char **argv) {
  int rank, size, tag;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  {
    TIMER::precise_stopwatch timer("Total Total");
    dc::entryPoint(rank, size);
  }
  MPI_Finalize();
}
