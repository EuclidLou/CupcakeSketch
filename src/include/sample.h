#pragma once

#include <algorithm>
#include <vector>
#include "defs.h"
#include "hash.h"
#include <fstream>
#include <vector>

class SAMPLE
{
public:
    int LEN;
    seed_t seed;
    data_t *ids_1, *ids_2;
    uint32_t *freq_1, *freq_2;

    SAMPLE(int len) : LEN(len)
    {
        srand(clock());
        seed = rand();
        ids_1 = new data_t[LEN];
        ids_2 = new data_t[LEN];
        freq_1 = new uint32_t[LEN];
        freq_2 = new uint32_t[LEN];

        memset(ids_1, 0, sizeof(data_t) * LEN);
        memset(ids_2, 0, sizeof(data_t) * LEN);
        memset(freq_1, 0, sizeof(uint32_t) * LEN);
        memset(freq_2, 0, sizeof(uint32_t) * LEN);
    }

    ~SAMPLE()
    {
        delete[] ids_1;
        delete[] ids_2;
        delete[] freq_1;
        delete[] freq_2;
    }

    void insert_1(data_t item)
    {
        int pos = HASH::hash(item, seed) % LEN;
        if(ids_1[pos] == 0){
            ids_1[pos] = item;
            freq_1[pos] = 1;
        }
        else freq_1[pos] += 1;
    }

    void insert_2(data_t item)
    {
        int pos = HASH::hash(item, seed) % LEN;
        if(ids_2[pos] == 0){
            ids_2[pos] = item;
            freq_2[pos] = 1;
        }
        else freq_2[pos] += 1;
    }

    double calculate_similarity(){
        int I=0, U=0;
        double sim = 0;
        for(int i=0; i<LEN; i++){
            if(ids_1[i] == ids_2[i]){
                I += (freq_1[i] < freq_2[i] ? freq_1[i] : freq_2[i]);
                U += (freq_1[i] > freq_2[i] ? freq_1[i] : freq_2[i]);
            }
            else {
                U += (freq_1[i] + freq_2[i]);
            }
        }
        sim = (double) I / U;
        return sim;
    }
};