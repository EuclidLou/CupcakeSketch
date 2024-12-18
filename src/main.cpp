#include "include/main.h"

int main(){
    double metrics[]={0.0, 0.0};
    Dataset dataset;
    dataset.init("./dataset/"+std::string(DATASET)+".dat", ITEM_SIZE, S_FACTOR);

    double gt = dataset.similarity();
    vector<double> est_minhash = distribution_minhash_cm(REPEAT, 3*MEMORY_1_24, K, dataset); // mem = 8*len, signature_size = hash_cnt*2*8
    vector<double> est_sampled = enroll_sample_based(REPEAT, 24*MEMORY_1_24, K, dataset); // mem = mem, signature_size = len*2*8
    vector<double> est_cupcake = enroll_cupcake<12*MEMORY_1_24>(REPEAT, K, dataset, metrics); //mem = 2*<>, signature_size = hash_cnt*2*8
    vector<double> est_hyperll = test_hyperloglog(REPEAT, 24*MEMORY_1_24, K, dataset); // mem = mem, signature_size = sigature_size*16
    vector<double> RE_minhash = postprocess(est_minhash, gt);
    vector<double> RE_sampled = postprocess(est_sampled, gt);
    vector<double> RE_cupcake = postprocess(est_cupcake, gt);
    vector<double> RE_hyperll = postprocess(est_hyperll, gt);
    LOG_RESULT("Relative Error of MinHash: %lf%c", RE_minhash[0]*100.0, '%');
    LOG_RESULT("Relative Error of Sampled: %lf%c", RE_sampled[0]*100.0, '%');
    LOG_RESULT("Relative Error of Cupcake: %lf%c", RE_cupcake[0]*100.0, '%');
    LOG_RESULT("Relative Error of Hyperll: %lf%c", RE_hyperll[0]*100.0, '%');
    std::ofstream log_file("log/execution_log.txt", std::ios_base::app);
    log_file    << "Dataset: "      << std::left << std::setw(10) << std::setfill(' ') << DATASET << "\t"
                << "Separate: "     << S_FACTOR << "\t"
                << "Heavy-bias: "   << std::left << std::setw(5) << std::setfill(' ') << HEAVY_BIAS << "\t"
                << "Swap-factor: "  << std::left << std::setw(2) << std::setfill(' ') << SWAP_FACTOR << "\t"
                << "K: "            << std::left << std::setw(10) << std::setfill(' ') << K << "\t"
                << "Mem in KB: "    << std::left << std::setw(6) << std::setfill(' ') << std::setprecision(3) << MEMORY_1_24*24.0/1024.0 << "\t"
                << "GT: "           << std::fixed << std::setprecision(6) << gt << "\t"
                << "MinHash: "         << std::fixed << std::setprecision(6) << RE_minhash[0] << "\t" << std::fixed << std::setprecision(6) << RE_minhash[1] << "\t"
                << "Sampled: "         << std::fixed << std::setprecision(6) << RE_sampled[0] << "\t" << std::fixed << std::setprecision(6) << RE_sampled[1] << "\t"
                << "Hyperll: "         << std::fixed << std::setprecision(6) << RE_hyperll[0] << "\t" << std::fixed << std::setprecision(6) << RE_hyperll[1] << "\t"
                << "Cupcake: "         << std::fixed << std::setprecision(6) << RE_cupcake[0] << "\t" << std::fixed << std::setprecision(6) << RE_cupcake[1] << "\t"
                << (USE_TOWER == 1 ? "With Tower \t" : (USE_CS == 0 ? "Level-1 CM \t" : (USE_CS == 1 ? "Level-1 CS \t" : "Level-3 CS \t")))
                << (DHASH == 0 ? "S-Hash     \t" : "D-Hash     \t")
                #if METRICS == 1
                << "AAE: "          << std::left << std::setfill(' ') << std::setw(10) << std::fixed << std::setprecision(3) << metrics[0] << "\t" 
                << "ARE: "          << std::fixed << std::setprecision(4) << metrics[1] << "%\t"
                #endif
                <<std::endl;
    log_file.close();
}