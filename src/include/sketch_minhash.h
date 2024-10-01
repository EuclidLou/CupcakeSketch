#pragma once
#ifndef __SKC_MIHS_H__

#define __SKC_MIHS_H__
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <omp.h>
#include "defs.h"
#include "hash.h"

class Counter {
public:
    int counter1(data_t item) {
        q1.push(item);
        freq1[item]++;
        return freq1[item];
    }
    int counter2(data_t item) {
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
    Counter counter;

    novel_minhash(int len, int hash_cnt) : HASH_LEN(len), HASH_CNT(hash_cnt)
    {
        srand(clock());
        index_s = rand();
        left_pos=HASH_LEN;
        hash = new struct u[HASH_LEN];
        hash_seed = new seed_t[HASH_CNT];
        memset(hash, 0, sizeof(struct u) * HASH_LEN);
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
        delete[] hash;
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
        LOG_DEBUG("into similarity_minhash()");
        double similarity = 0;
        int same_hash = 0, all_hash = HASH_CNT;
        for (int i = 0; i < all_hash; i++)
            if (min_hash_value_1[i] == min_hash_value_2[i])
                same_hash++;
        similarity = 1.0 * same_hash / all_hash;
        LOG_DEBUG("same_hash: %d, all_hash: %d", same_hash, all_hash);
        LOG_RESULT("similarity: %lf", similarity);
        LOG_DEBUG("exit similarity_minhash()");
        return similarity;
    }
};

#endif