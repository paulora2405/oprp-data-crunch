#ifndef DATA_CRUNCH_HPP
#define DATA_CRUNCH_HPP

#include <string>
#include <vector>

namespace dc {

void dataManipulation(std::vector<double>& data);

std::pair<double, double> avg_and_geo(std::vector<double>& data);

std::vector<double> ingestData(int N);

void entryPoint();

}  // namespace dc

#endif