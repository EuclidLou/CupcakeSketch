#include "dataset.h"
#include "test_minhash.h"
#include "test_hll.h"
#include "test_sample.h"
#include "test_cupcake.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include "logger.h"
#include "macros.h"

vector<double> postprocess(const std::vector<double>& similarity, double gt){
    vector<double> RE(similarity.size());
    for (size_t i = 0; i < similarity.size(); ++i){
        RE[i] = std::abs((similarity[i] - gt) / gt);
    }
    vector<double> ret(2, 0.0);
    ret[0] = std::accumulate(RE.begin(), RE.end(), 0.0) / RE.size();
    for (double re : RE) {
        ret[1] += std::pow(re - ret[0], 2);
    }
    ret[1] = std::sqrt(ret[1] / RE.size());
    return ret;
}