#pragma once
#include "hll.h"
#include "dataset.h"
#include <cmath>
#include <cstdint>

double distribution_hll_cm(int loop_time, int len, string dataset_name = "caida", int nytimes_dataset_len = 800000, int second_dataset_start = 800003, std::ofstream *fout_time = NULL)
{
    LOG_DEBUG("into distribution_hll_cm()");
    bool is_bow = dataset_name == "docword.nytimes";
    Dataset dataset;
    Dataset_BoW dataset_bow1, dataset_bow2;
    int total_packets = 0;

    if (dataset_name == "caida")
    {
        dataset.init("./dataset/caida.dat", 21);
        total_packets = std::max(dataset.stream1.TOTAL_PACKETS, dataset.stream2.TOTAL_PACKETS);
    }
    else if (dataset_name.find("zipf") != string::npos)
    {
        dataset.init("./dataset/" + dataset_name + ".dat", 4);
        total_packets = std::max(dataset.stream1.TOTAL_PACKETS, dataset.stream2.TOTAL_PACKETS);
    }
    else if (dataset_name == "docword.nytimes")
    {
        dataset_bow1.init("./dataset/docword.nytimes.txt", nytimes_dataset_len, 3);
        dataset_bow2.init("./dataset/docword.nytimes.txt", nytimes_dataset_len, second_dataset_start);

        LOG_DEBUG("dataset_bow1.TOTAL_PACKETS: %d", dataset_bow1.TOTAL_PACKETS);
        LOG_DEBUG("dataset_bow2.TOTAL_PACKETS: %d", dataset_bow2.TOTAL_PACKETS);

        total_packets = std::max(dataset_bow1.TOTAL_PACKETS, dataset_bow2.TOTAL_PACKETS);
    }
    else
    {
        dataset.init("./dataset/caida.dat", 21);
        total_packets = std::max(dataset.stream1.TOTAL_PACKETS, dataset.stream2.TOTAL_PACKETS);
    }

    hyperloglog_cm hll(2048, len);

    auto start = std::chrono::high_resolution_clock::now(), end = std::chrono::high_resolution_clock::now();
    double similarity_avg = 0;
    for (int i = 0; i < loop_time; i++)
    {
        start = std::chrono::high_resolution_clock::now();
        if (!is_bow)
        {
            for (int i = 0; i < dataset.stream1.TOTAL_PACKETS; i++)
            {
                hll.insert1(dataset.stream1.raw_data[i]);
            }
            for (int i = 0; i < dataset.stream2.TOTAL_PACKETS; i++)
            {
                hll.insert2(dataset.stream2.raw_data[i]);
            }
        }
        else if (is_bow)
        {
            for (int i = 0; i < dataset_bow1.TOTAL_PACKETS; i++)
            {
                hll.insert1(dataset_bow1.raw_data[i]);
            }
            for (int i = 0; i < dataset_bow2.TOTAL_PACKETS; i++)
            {
                hll.insert2(dataset_bow2.raw_data[i]);
            }
        }
        uint64_t size_1 = hll.get_estimated_size1();
        uint64_t size_2 = hll.get_estimated_size2();
        uint64_t size_1_2 = hll.get_estimated_size_1and2();
        double similarity = hll.similarity();
        similarity_avg += similarity;
    }
    similarity_avg /= loop_time;
    end = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    int MIPS = (int)(1. * total_packets / elapsed_time.count() * 1e6);

    LOG_DEBUG("TEST SIMILARITY THROUGH HYPERLOGLOG WITH CM");
    // LOG_DEBUG("The size of 1:%llu,size of 2:%llu,size of 1 and 2:%llu", size_1, size_2, size_1_2);
    LOG_RESULT("hll cm similarity_avg: %lf", similarity_avg);
    LOG_RESULT("hll cm MIPS: %d", MIPS);
    if (fout_time != NULL)
        *fout_time << MIPS << ",";
    LOG_DEBUG("exit distribution_hll_cm()");
    return similarity_avg;
}

vector<double> test_hyperloglog(int loop_time, uint64_t memory_size, int sigature_size, Dataset &dataset)
{
    LOG_DEBUG("into test_hyperloglog()");
    int total_packets = std::max(dataset.stream1.TOTAL_PACKETS, dataset.stream2.TOTAL_PACKETS);
    vector<double>similarity(loop_time);
    auto start = std::chrono::high_resolution_clock::now(), end = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < loop_time; i++)
    {
        int bucketnum = 1 << static_cast<int>(log2(sigature_size * 8));
        // hyperloglog_hash hll(bucketnum, memory_size/16);
        // hyperloglog_excat hll(bucketnum);
        hyperloglog_cm   hll(bucketnum, memory_size/8);
        // hyperloglog_cs   hll(bucketnum, memory_size/8);
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < dataset.stream1.TOTAL_PACKETS; i++){
            hll.insert1(dataset.stream1.raw_data[i]);
        }
        for (int i = 0; i < dataset.stream2.TOTAL_PACKETS; i++){
            hll.insert2(dataset.stream2.raw_data[i]);
        }
        similarity[i] = hll.similarity();
        end = std::chrono::high_resolution_clock::now();
    }
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    int MIPS = (int)(1. * total_packets / elapsed_time.count() * 1e6);
    LOG_INFO("TEST SIMILARITY THROUGH HYPERLOGLOG WITH EXCAT HASH MAP");
    LOG_RESULT("hll cm similarity_avg: %lf", std::accumulate(similarity.begin(), similarity.end(), 0.0) / loop_time);
    LOG_RESULT("hll cm MIPS: %d", MIPS);
    LOG_DEBUG("exit distribution_hll_cm()\n");
    // LOG_RESULT("The size of 1:%lu,size of 2:%lu,size of 1 and 2:%lu,similarity:%lf", size_1, size_2, size_1_2, 
    //             std::accumulate(similarity.begin(), similarity.end(), 0.0) / loop_time);
    // printf("TEST SIMILARITY THROUGH HYPERLOGLOG WITH EXCAT HASH MAP");
    // printf("The size of 1:%lu,size of 2:%lu,size of 1 and 2:%lu,similarity:%lf\n", size_1, size_2, size_1_2, similarity);
    return similarity;
}

double distribution_hll_hash(int loop_time, int len, string dataset_name = "caida", int nytimes_dataset_len = 800000, int second_dataset_start = 800003, std::ofstream *fout_time = NULL)
{
    LOG_DEBUG("into distribution_hll_hash()");
    bool is_bow = dataset_name == "docword.nytimes";
    Dataset dataset;
    Dataset_BoW dataset_bow1, dataset_bow2;
    int total_packets = 0;

    if (dataset_name == "caida")
    {
        dataset.init("./dataset/caida.dat", 21);
        total_packets = std::max(dataset.stream1.TOTAL_PACKETS, dataset.stream2.TOTAL_PACKETS);
    }
    else if (dataset_name.find("zipf") != string::npos)
    {
        dataset.init("./dataset/" + dataset_name + ".dat", 4);
        total_packets = std::max(dataset.stream1.TOTAL_PACKETS, dataset.stream2.TOTAL_PACKETS);
    }
    else if (dataset_name == "docword.nytimes")
    {
        dataset_bow1.init("./dataset/docword.nytimes.txt", nytimes_dataset_len, 3);
        dataset_bow2.init("./dataset/docword.nytimes.txt", nytimes_dataset_len, second_dataset_start);

        LOG_DEBUG("dataset_bow1.TOTAL_PACKETS: %d", dataset_bow1.TOTAL_PACKETS);
        LOG_DEBUG("dataset_bow2.TOTAL_PACKETS: %d", dataset_bow2.TOTAL_PACKETS);

        total_packets = std::max(dataset_bow1.TOTAL_PACKETS, dataset_bow2.TOTAL_PACKETS);
    }
    else
    {
        dataset.init("./dataset/caida.dat", 21);
        total_packets = std::max(dataset.stream1.TOTAL_PACKETS, dataset.stream2.TOTAL_PACKETS);
    }
    hyperloglog_hash hll(2048, len);

    auto start = std::chrono::high_resolution_clock::now(), end = std::chrono::high_resolution_clock::now();
    double similarity_avg = 0;
    for (int i = 0; i < loop_time; i++)
    {
        start = std::chrono::high_resolution_clock::now();
        if (!is_bow)
        {
            for (int i = 0; i < dataset.stream1.TOTAL_PACKETS; i++)
            {
                hll.insert1(dataset.stream1.raw_data[i]);
            }
            for (int i = 0; i < dataset.stream2.TOTAL_PACKETS; i++)
            {
                hll.insert2(dataset.stream2.raw_data[i]);
            }
        }
        else if (is_bow)
        {
            for (int i = 0; i < dataset_bow1.TOTAL_PACKETS; i++)
            {
                hll.insert1(dataset_bow1.raw_data[i]);
            }
            for (int i = 0; i < dataset_bow2.TOTAL_PACKETS; i++)
            {
                hll.insert2(dataset_bow2.raw_data[i]);
            }
        }
        uint64_t size_1 = hll.get_estimated_size1();
        uint64_t size_2 = hll.get_estimated_size2();
        uint64_t size_1_2 = hll.get_estimated_size_1and2();
        double similarity = hll.similarity();
        similarity_avg += similarity;
    }
    similarity_avg /= loop_time;
    end = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    int MIPS = (int)(1. * total_packets / elapsed_time.count() * 1e6);

    LOG_DEBUG("TEST SIMILARITY THROUGH HYPERLOGLOG WITH HASH");
    // LOG_DEBUG("The size of 1:%llu,size of 2:%llu,size of 1 and 2:%llu", size_1, size_2, size_1_2);
    LOG_RESULT("hll hash similarity_avg: %lf", similarity_avg);
    LOG_RESULT("hll hash MIPS: %d", MIPS);
    if (fout_time != NULL)
        *fout_time << MIPS << ",";
    LOG_DEBUG("exit distribution_hll_hash()");
    return similarity_avg;
}
