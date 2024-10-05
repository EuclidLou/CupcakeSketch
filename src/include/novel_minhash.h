#pragma once
#include "sketch_minhash.h"
#include "dataset.h"
#include "macros.h"
#include <fstream>
#include <omp.h>

template <int memory>
double enroll_novel_minhash(int loop_time, int hash_cnt, Dataset& dataset, double* metrics = NULL, std::ofstream *fout_time = NULL)
{
    LOG_DEBUG("enter enroll_novel_minhash()");
    int total_packets = std::max(dataset.stream1.TOTAL_PACKETS, dataset.stream2.TOTAL_PACKETS);

    double similarity_avg = 0;
    auto start = std::chrono::high_resolution_clock::now(), end = std::chrono::high_resolution_clock::now();
    // #pragma omp parallel for
    for (int i = 0; i < loop_time; i++)
    {
        novel_minhash<memory> nm(hash_cnt);

        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < dataset.stream1.TOTAL_PACKETS; i++)
            nm.insert1(dataset.stream1.raw_data[i]);
        for (int i = 0; i < dataset.stream2.TOTAL_PACKETS; i++)
            nm.insert2(dataset.stream2.raw_data[i]);
        end = std::chrono::high_resolution_clock::now();

        double similarity = nm.similarity();
        nm.sketch_aae(metrics);
        similarity_avg += similarity;
    }
    similarity_avg /= loop_time;
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    int MIPS = (int)(1. * total_packets / elapsed_time.count() * 1e6);
    #if METRICS == 1
    metrics[0] /= loop_time;
    metrics[1] /= loop_time;
    LOG_RESULT("AAE: %lf, ARE: %lf%c", metrics[0], metrics[1], '%');
    #endif
    LOG_RESULT("cycle similarity_avg: %lf", similarity_avg);
    LOG_RESULT("cycle MIPS: %d", MIPS);
    if (fout_time != NULL)
        *fout_time << MIPS << ",";
    LOG_DEBUG("exit enroll_novel_minhash()\n");
    return similarity_avg;
}
