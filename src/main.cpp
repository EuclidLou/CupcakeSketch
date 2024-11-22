#include "include/dataset.h"
// #include "include/test_cm.h"
// #include "include/test_cu.h"
// #include "include/test_count_sketch.h"
// #include "include/test_elastic.h"
#include "include/test_cycle.h"
#include "include/test_minhash.h"
#include "include/test_hll.h"
// #include "include/test_maxloghash.h"
#include "include/test_sample.h"
#include "include/test_cupcake.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include "logger.h"
#include "macros.h"

vector<double> postprocess(const std::vector<double>& similarity, double gt){
    vector<double> RE(similarity.size());
    for (size_t i = 0; i < similarity.size(); ++i){
        RE[i] = abs((similarity[i] - gt) / gt);
    }
    vector<double> ret(2, 0.0);
    ret[0] = std::accumulate(RE.begin(), RE.end(), 0.0) / RE.size();
    for (double re : RE) {
        ret[1] += std::pow(re - ret[0], 2);
    }
    ret[1] = std::sqrt(ret[1] / RE.size());
    return ret;
}

int main(){
    double metrics[]={0.0, 0.0};
    Dataset dataset;
    dataset.init("./dataset/"+std::string(DATASET)+".dat", ITEM_SIZE, S_FACTOR);

    double gt = dataset.similarity();
    vector<double> est_minhash = distribution_minhash_cm(REPEAT, 3*MEMORY_1_24, K, dataset); // mem = 8*len
    vector<double> est_sampled = enroll_sample_based(REPEAT, 24*MEMORY_1_24, K, dataset); // mem = mem
    vector<double> est_cupcake = enroll_cupcake<12*MEMORY_1_24>(REPEAT, K, dataset, metrics); //mem = 2*<>
    vector<double> RE_minhash = postprocess(est_minhash, gt);
    vector<double> RE_sampled = postprocess(est_sampled, gt);
    vector<double> RE_cupcake = postprocess(est_cupcake, gt);
    LOG_RESULT("Relative Error of MinHash: %lf%c", RE_minhash[0]*100.0, '%');
    LOG_RESULT("Relative Error of Sampled: %lf%c", RE_sampled[0]*100.0, '%');
    LOG_RESULT("Relative Error of Cupcake: %lf%c", RE_cupcake[0]*100.0, '%');
    std::ofstream log_file("log/execution_log.txt", std::ios_base::app);
    log_file    << "Dataset: "      << std::left << std::setw(10) << std::setfill(' ') << DATASET << "\t"
                // << "Separate: "     << S_FACTOR << "\t"
                // << "Heavy-bias: "   << std::left << std::setw(5) << std::setfill(' ') << HEAVY_BIAS << "\t"
                // << "Swap-factor: "  << std::left << std::setw(2) << std::setfill(' ') << SWAP_FACTOR << "\t"
                << "K: "            << std::left << std::setw(10) << std::setfill(' ') << K << "\t"
                << "Mem in KB: "    << std::left << std::setw(6) << std::setfill(' ') << std::setprecision(3) << MEMORY_1_24*24.0/1024.0 << "\t"
                << "GT: "           << std::fixed << std::setprecision(6) << gt << "\t"
                << "MinHash: "         << std::fixed << std::setprecision(6) << RE_minhash[0] << "@" << std::fixed << std::setprecision(6) << RE_minhash[1] << "\t"
                << "Sampled: "         << std::fixed << std::setprecision(6) << RE_sampled[0] << "@" << std::fixed << std::setprecision(6) << RE_sampled[1] << "\t"
                << "Cupcake: "         << std::fixed << std::setprecision(6) << RE_cupcake[0] << "@" << std::fixed << std::setprecision(6) << RE_cupcake[1] << "\t"
                << (USE_TOWER == 1 ? "With Tower \t" : (USE_CS == 0 ? "Level-1 CM \t" : (USE_CS == 1 ? "Level-1 CS \t" : "Level-3 CS \t")))
                << (DHASH == 0 ? "S-Hash     \t" : "D-Hash     \t")
                #if METRICS == 1
                << "AAE: "          << std::left << std::setfill(' ') << std::setw(10) << std::fixed << std::setprecision(3) << metrics[0] << "\t" 
                << "ARE: "          << std::fixed << std::setprecision(4) << metrics[1] << "%\t"
                #endif
                <<std::endl;
    log_file.close();
}