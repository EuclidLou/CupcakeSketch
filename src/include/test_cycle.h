#pragma once
#include "cycle.h"
#include "dataset.h"
#include <fstream>
#include <omp.h>

double distribution_cycle(int loop_time, int len, Dataset& dataset, std::ofstream *fout_time = NULL)
{
    LOG_DEBUG("enter distribution_cycle()");
    int total_packets = std::max(dataset.stream1.TOTAL_PACKETS, dataset.stream2.TOTAL_PACKETS);
    double similarity_avg = 0;
    auto start = std::chrono::high_resolution_clock::now(), end = std::chrono::high_resolution_clock::now();
    // #pragma omp parallel for
    for (int i = 0; i < loop_time; i++)
    {
        cycle_ours cs(len);

        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < dataset.stream1.TOTAL_PACKETS; i++)
            cs.insert1(dataset.stream1.raw_data[i]);
        for (int i = 0; i < dataset.stream2.TOTAL_PACKETS; i++)
            cs.insert2(dataset.stream2.raw_data[i]);
        end = std::chrono::high_resolution_clock::now();

        double similarity = cs.similarity();
        similarity_avg += similarity;
    }
    similarity_avg /= loop_time;
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    int MIPS = (int)(1. * total_packets / elapsed_time.count() * 1e6);

    LOG_RESULT("cycle similarity_avg: %lf", similarity_avg);
    LOG_RESULT("cycle MIPS: %d", MIPS);
    if (fout_time != NULL)
        *fout_time << MIPS << ",";
    LOG_DEBUG("exit distribution_cycle()\n");
    return similarity_avg;
}
