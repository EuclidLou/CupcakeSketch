#include "include/dataset.h"
// #include "include/test_cm.h"
// #include "include/test_cu.h"
// #include "include/test_count_sketch.h"
// #include "include/test_elastic.h"
#include "include/test_cycle.h"
// #include "include/test_minhash.h"
// #include "include/test_hll.h"
// #include "include/test_maxloghash.h"
#include "include/novel_minhash.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include "macros.h"


int main(){
    double metrics[]={0.0, 0.0};
    Dataset dataset;
    dataset.init("./dataset/"+std::string(DATASET)+".dat", ITEM_SIZE, S_FACTOR);

    double gt = dataset.similarity();
    double est_simi = distribution_cycle(REPEAT, MEMORY_1_24, dataset);   //mem = 24*len
    double est_mihs = enroll_novel_minhash<12*MEMORY_1_24>(REPEAT, K, dataset, metrics); //mem = 2*<>
    double RE_simi = (est_simi - gt) / gt * 100.0;
    double RE_mihs = (est_mihs - gt) / gt * 100.0;
    LOG_RESULT("Relative Error of SimiSketch: %lf%c", RE_simi, '%');
    LOG_RESULT("Relative Error of Novel MinHash: %lf%c", RE_mihs, '%');
    std::ofstream log_file("log/execution_log.txt", std::ios_base::app);
    log_file    << "Dataset: "      << std::left << std::setw(10) << std::setfill(' ') << DATASET << "\t"
                << "Separate: "     << S_FACTOR << "\t"
                << "K: "            << std::left << std::setw(10) << std::setfill(' ') << K << "\t"
                << "Heavy-bias: "   << std::left << std::setw(5) << std::setfill(' ') << HEAVY_BIAS << "\t"
                << "Swap-factor: "  << std::left << std::setw(2) << std::setfill(' ') << SWAP_FACTOR << "\t"
                << "Mem in KB: "    << std::left << std::setw(6) << std::setfill(' ') << std::setprecision(3) << MEMORY_1_24*24.0/1024.0 << "\t"
                << "GT: "           << std::fixed << std::setprecision(6) << gt << "\t"
                << "Simi: "         << std::fixed << std::setprecision(6) << est_simi << "@" << std::fixed << std::setprecision(3) << RE_simi << "%\t"
                << "Ours: "         << std::fixed << std::setprecision(6) << est_mihs << "@" << std::fixed << std::setprecision(3) << RE_mihs << "%\t\t"
                #if USE_TOWER == 1
                << "With Tower \t"
                #else
                #if USE_CS == 1
                << "Level-1 CS \t"
                #elif USE_CS == 3
                << "Level-3 CS \t"
                #else
                << "Elastic    \t"
                #endif
                #endif
                #if METRICS == 1
                << "AAE: "          << std::left << std::setfill(' ') << std::setw(10) << std::fixed << std::setprecision(3) << metrics[0] << "\t" 
                << "ARE: "          << std::fixed << std::setprecision(4) << metrics[1] << "%\t"
                #endif
                <<std::endl;
    log_file.close();
}