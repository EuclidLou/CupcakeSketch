#pragma once
#include "sample.h"
#include "dataset.h"
#include <fstream>

double enroll_sample_based(int loop_time, int len, Dataset &dataset, std::ofstream *fout_time = NULL)
{
    LOG_DEBUG("enter enroll_sample_based()");
    int total_packets = std::max(dataset.stream1.TOTAL_PACKETS, dataset.stream2.TOTAL_PACKETS);
    int line_cnt = 1;
    
    auto start = std::chrono::high_resolution_clock::now(), end = std::chrono::high_resolution_clock::now();
    double similarity_avg = 0;
    for (int i = 0; i < loop_time; i++)
    {
        SAMPLE sample(len);
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < dataset.stream1.TOTAL_PACKETS; i++)
            sample.insert_1(dataset.stream1.raw_data[i]);
        for (int i = 0; i < dataset.stream2.TOTAL_PACKETS; i++)
            sample.insert_2(dataset.stream2.raw_data[i]);
        end = std::chrono::high_resolution_clock::now();

        double similarity = sample.calculate_similarity();
        similarity_avg += similarity;
    }
    similarity_avg /= loop_time;
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    int MIPS = (int)(1. * total_packets / elapsed_time.count() * 1e6);

    LOG_RESULT("sample based similarity_avg: %lf", similarity_avg);
    LOG_RESULT("sample based MIPS: %d", MIPS);
    if (fout_time != NULL)
        *fout_time << MIPS << ",";
    LOG_DEBUG("exit enroll_sample_based()\n");
    return similarity_avg;
}