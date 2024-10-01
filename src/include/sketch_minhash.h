#pragma once
#ifndef __SKC_MIHS_H__

#define __SKC_MIHS_H__
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <omp.h>
#include "defs.h"
#include "hash.h"
#include "../thirdparty/ElasticSketch.h"

class Counter
{
public:
    int counter1(data_t item)
    {
        q1.push(item);
        freq1[item]++;
        return freq1[item];
    }
    int counter2(data_t item)
    {
        q2.push(item);
        freq2[item]++;
        return freq2[item];
    }

private:
    std::queue<data_t> q1;
    std::unordered_map<data_t, int> freq1;
    std::queue<data_t> q2;
    std::unordered_map<data_t, int> freq2;
};

template <int memory>
class Counter_Sketch
{
public:
    ElasticSketch<memory / 256, memory> *sketch1;
    ElasticSketch<memory / 256, memory> *sketch2;

    Counter_Sketch()
    {
        sketch1 = new ElasticSketch<memory / 256, memory>();
        sketch2 = new ElasticSketch<memory / 256, memory>();
    }
    ~Counter_Sketch()
    {
        delete sketch1;
        delete sketch2;
    }
    int counter1(data_t item)
    {
        uint8_t key[4];
        key[0] = (item >> 24) & 0xFF;
        key[1] = (item >> 16) & 0xFF;
        key[2] = (item >> 8) & 0xFF;
        key[3] = item & 0xFF;
        sketch1->insert((uint8_t *)key, 1);
        return sketch1->query((uint8_t *)key);
    }
    int counter2(data_t item)
    {
        uint8_t key[4];
        key[0] = (item >> 24) & 0xFF;
        key[1] = (item >> 16) & 0xFF;
        key[2] = (item >> 8) & 0xFF;
        key[3] = item & 0xFF;
        sketch2->insert((uint8_t *)key, 1);
        return sketch2->query((uint8_t *)key);
    }
};

template <int memory>
class novel_minhash
{
public:
    int HASH_LEN, HASH_CNT;
    seed_t *hash_seed;
    uint64_t *min_hash_value_1;
    uint64_t *min_hash_value_2;
    seed_t index_s;
    struct u *hash;
    int left_pos;
    Counter_Sketch<memory> counter;

    novel_minhash(int hash_cnt) : HASH_CNT(hash_cnt)
    {
        srand(clock());
        index_s = rand();
        hash_seed = new seed_t[HASH_CNT];
        for (int i = 0; i < HASH_CNT; i++)
        {
            hash_seed[i] = rand();
        }
        min_hash_value_1 = new uint64_t[HASH_CNT];
        min_hash_value_2 = new uint64_t[HASH_CNT];
        for (int i = 0; i < HASH_CNT; i++)
        {
            min_hash_value_1[i] = 0xffffffffffffffff;
            min_hash_value_2[i] = 0xffffffffffffffff;
        }
    }

    ~novel_minhash()
    {
        delete[] hash_seed;
        delete[] min_hash_value_1;
        delete[] min_hash_value_2;
    }

    void insert1(data_t item)
    {
        int freq = counter.counter1(item);
        // #pragma omp parallel for
        // for (int i = 0; i < HASH_CNT; i++)
        // {
        //     min_hash_value_1[i] = std::min(min_hash_value_1[i], HASH::hash(HASH::hash(item, hash_seed[i]), freq));
        // }
        int min_idx =  static_cast<int>(HASH::hash(HASH::hash(item, index_s), freq) % static_cast<uint64_t>(HASH_CNT));
        min_hash_value_1[min_idx] = std::min(min_hash_value_1[min_idx], HASH::hash(HASH::hash(item, hash_seed[min_idx]), freq));
    }

    void insert2(data_t item)
    {
        int freq = counter.counter2(item);
        // #pragma omp parallel for
        // for (int i = 0; i < HASH_CNT; i++)
        // {
        //     min_hash_value_2[i] = std::min(min_hash_value_2[i], HASH::hash(HASH::hash(item, hash_seed[i]), freq));
        // }
        int min_idx =  static_cast<int>(HASH::hash(HASH::hash(item, index_s), freq) % static_cast<uint64_t>(HASH_CNT));
        min_hash_value_2[min_idx] = std::min(min_hash_value_2[min_idx], HASH::hash(HASH::hash(item, hash_seed[min_idx]), freq));
    }

    double similarity()
    {
        // LOG_DEBUG("into similarity_minhash()");
        double similarity = 0;
        int same_hash = 0, all_hash = HASH_CNT;
        for (int i = 0; i < all_hash; i++)
            if (min_hash_value_1[i] == min_hash_value_2[i])
                same_hash++;
        similarity = 1.0 * same_hash / all_hash;
        // LOG_DEBUG("same_hash: %d, all_hash: %d", same_hash, all_hash);
        // LOG_RESULT("similarity: %lf", similarity);
        // LOG_DEBUG("exit similarity_minhash()");
        return similarity;
    }
};

#endif