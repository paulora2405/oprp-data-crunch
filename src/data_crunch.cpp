#include "data_crunch.hpp"

#include <omp.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>

#include "sort.hpp"
#include "timer.hpp"

namespace dc {

void entryPoint() {
  const std::vector<uint32_t> entradas{
      10000001, 10000003, 1000003, 2000003, 3000003, 4000001, 4000003,
  };
  std::vector<std::vector<double>> dataVectors;

#pragma omp parallel
  {
#pragma omp for
    for(auto entrada : entradas) {
      dataVectors.push_back(ingestData(entrada));
    }
  }

  for(auto data : dataVectors) {
    dataManipulation(data);
  }
}

std::pair<double, double> avg_and_geo(std::vector<double>& data) {
  int N = data.size();
  double avg = 0.0, prod = 1.0;
  for(double value : data) {
    avg += (value / N);
    prod *= std::pow(value, (1.0 / static_cast<double>(N)));
  }

  return std::make_pair(avg, prod);
}

void dataManipulation(std::vector<double>& data) {
  {
    TIMER::precise_stopwatch timer("DataMani" + std::to_string(data.size()));
    sort::quicksort(data, 0, data.size() - 1);
  }
  bool ordered = true;
  for(size_t i = 0; i < 1000; i++) {
    if(data[i] > data[i + 1])
      ordered = false;
  }

  if(!ordered) {
    std::cerr << data.size() << " nao ordenado\n";
  }

  std::pair<double, double> p = avg_and_geo(data);
  double avg = p.first, geo = p.second;

  double ma = avg;
  double std = 0.0;
  double mg = geo;
  double md = data[data.size() / 2 + 1];
  double p95 = data[(95 / 100) * data.size()];
  double min = data.front();
  double max = data.back();

  for(size_t i = 0; i < data.size(); ++i)
    std += std::pow(data[i] - ma, 2);

  std = sqrt(std / data.size());

  std::cout << data.size() << std::setprecision(8) << ".txt " << ma << ' ' << std << ' ' << mg
            << ' ' << md << ' ' << p95 << ' ' << min << ' ' << max << '\n';

  /* N0.txt MA DP MG MD 95p min max */
}

std::vector<double> ingestData(int N) {
  FILE* is;
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

}  // namespace dc