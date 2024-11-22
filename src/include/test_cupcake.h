#pragma once
#include "cupcake.h"
#include "dataset.h"
#include "macros.h"
#include <fstream>
#include <omp.h>
#include <vector>

template <int memory>
vector<double> enroll_cupcake(int loop_time, int hash_cnt, Dataset& dataset, double* metrics = NULL, std::ofstream *fout_time = NULL)
{
    LOG_DEBUG("enter enroll_cupcake()");
    int total_packets = std::max(dataset.stream1.TOTAL_PACKETS, dataset.stream2.TOTAL_PACKETS);
    vector<double>similarity(loop_time);
    auto start = std::chrono::high_resolution_clock::now(), end = std::chrono::high_resolution_clock::now();
    // #pragma omp parallel for
    for (int i = 0; i < loop_time; i++)
    {
        cupcake_sketch<memory> cc(hash_cnt);

        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < dataset.stream1.TOTAL_PACKETS; i++)
            cc.insert1(dataset.stream1.raw_data[i]);
        for (int i = 0; i < dataset.stream2.TOTAL_PACKETS; i++)
            cc.insert2(dataset.stream2.raw_data[i]);
        end = std::chrono::high_resolution_clock::now();

        similarity[i] = cc.similarity();
        cc.sketch_aae(metrics);
    }
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    int MIPS = (int)(1. * total_packets / elapsed_time.count() * 1e6);
    #if METRICS == 1
    metrics[0] /= loop_time;
    metrics[1] /= loop_time;
    LOG_RESULT("AAE: %lf, ARE: %lf%c", metrics[0], metrics[1], '%');
    #endif
    LOG_RESULT("cupcake similarity_avg: %lf", std::accumulate(similarity.begin(), similarity.end(), 0.0) / loop_time);
    LOG_RESULT("cupcake MIPS: %d", MIPS);
    if (fout_time != NULL)
        *fout_time << MIPS << ",";
    LOG_DEBUG("exit enroll_cupcake()\n");
    return similarity;
}
