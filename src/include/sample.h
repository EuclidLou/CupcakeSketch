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
    int LEN, MEM;
    seed_t seed;
    data_t *ids_1, *ids_2;
    uint32_t *freq_1, *freq_2;
    int *merge;

    SAMPLE(int mem, int len) : MEM(mem/16), LEN(len/2)
    {
        srand(clock());
        seed = rand();
        ids_1 = new data_t[MEM];
        ids_2 = new data_t[MEM];
        freq_1 = new uint32_t[MEM];
        freq_2 = new uint32_t[MEM];
        merge = new int[LEN];

        memset(ids_1, 0, sizeof(data_t) * MEM);
        memset(ids_2, 0, sizeof(data_t) * MEM);
        memset(freq_1, 0, sizeof(uint32_t) * MEM);
        memset(freq_2, 0, sizeof(uint32_t) * MEM);
        memset(merge, 0, sizeof(int) * LEN);
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
        int pos = HASH::hash(item, seed) % MEM;
        if(ids_1[pos] == 0){
            ids_1[pos] = item;
            freq_1[pos] = 1;
        }
        else freq_1[pos] += 1;
    }

    void insert_2(data_t item)
    {
        int pos = HASH::hash(item, seed) % MEM;
        if(ids_2[pos] == 0){
            ids_2[pos] = item;
            freq_2[pos] = 1;
        }
        else freq_2[pos] += 1;
    }

    double calculate_similarity(){
        int I=0, U=0;
        double sim = 0;
        if(MEM < LEN){
            for(int i=0; i<MEM; i++){
                if(ids_1[i] == ids_2[i]){
                    I += (freq_1[i] < freq_2[i] ? freq_1[i] : freq_2[i]);
                    U += (freq_1[i] > freq_2[i] ? freq_1[i] : freq_2[i]);
                }
                else {
                    U += (freq_1[i] + freq_2[i]);
                }
            }
        }
        else {
            int C = MEM / LEN;
            int R = MEM % LEN;
            int idx_1, idx_2;
            for(int i=0; i<R; i++){
                idx_1 = findMaxIndexInRange(freq_1, i*(C+1), (i+1)*(C+1));
                idx_2 = findMaxIndexInRange(freq_2, i*(C+1), (i+1)*(C+1));
                if(ids_1[idx_1] == ids_2[idx_2]){
                    I += (freq_1[idx_1] < freq_2[idx_2] ? freq_1[idx_1] : freq_2[idx_2]);
                    U += (freq_1[idx_1] > freq_2[idx_2] ? freq_1[idx_1] : freq_2[idx_2]);
                }
                else {
                    U += (freq_1[idx_1] + freq_2[idx_2]);
                }
            }
            for(int i=R; i<LEN; i++){
                idx_1 = findMaxIndexInRange(freq_1, i*C+R, (i+1)*C+R);
                idx_2 = findMaxIndexInRange(freq_1, i*C+R, (i+1)*C+R);
                if(ids_1[idx_1] == ids_2[idx_2]){
                    I += (freq_1[idx_1] < freq_2[idx_2] ? freq_1[idx_1] : freq_2[idx_2]);
                    U += (freq_1[idx_1] > freq_2[idx_2] ? freq_1[idx_1] : freq_2[idx_2]);
                }
                else {
                    U += (freq_1[idx_1] + freq_2[idx_2]);
                }
            }
        }
        sim = (double) I / U;
        return sim;
    }

    int findMaxIndexInRange(uint32_t list[], int a, int b) {
        int maxIndex = a;
        // Loop through the range [a, b)
        for (int i = a + 1; i < b; ++i) {
            if (list[i] > list[maxIndex]) {
                maxIndex = i;
            }
        }
        return maxIndex;
    }
};