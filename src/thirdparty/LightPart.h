#ifndef _LIGHT_PART_H_
#define _LIGHT_PART_H_

#include "EMFSD.h"
#include "param.h"

template <int init_mem_in_bytes>
class LightPart
{
    #if USE_CS == 3
    static constexpr int counter_num = init_mem_in_bytes / 3;
    BOBHash32 *bobhash = NULL;
    BOBHash32 *bobhash_ = NULL;
    BOBHash32 *bobhash__ = NULL;
    #else
    BOBHash32 *bobhash = NULL;
    static constexpr int counter_num = init_mem_in_bytes;
    #endif

public:
    #if USE_CS == 3
    uint8_t counters_1[counter_num];
    uint8_t counters_2[counter_num];
    uint8_t counters_3[counter_num];
    #else
    uint8_t counters[counter_num];
    #endif
    int mice_dist[256];
    EMFSD *em_fsd_algo = NULL;

    LightPart()
    {
        clear();
        std::random_device rd;
        bobhash = new BOBHash32(rd() % MAX_PRIME32);
        #if USE_CS == 3
        bobhash_ = new BOBHash32(rd() % MAX_PRIME32);
        bobhash__ = new BOBHash32(rd() % MAX_PRIME32);
        #endif
    }
    ~LightPart()
    {
        delete bobhash;
        #if USE_CS == 3
        delete bobhash_;
        delete bobhash__;
        #endif
    }

    void clear()
    {
        #if USE_CS == 0
        memset(counters, 0, counter_num);
        #elif USE_CS == 1
        memset(counters, 128, counter_num);
        #elif USE_CS == 3
        memset(counters_1, 128, counter_num);
        memset(counters_2, 128, counter_num);
        memset(counters_3, 128, counter_num);
        #endif
        memset(mice_dist, 0, sizeof(int) * 256);
    }

    /* insertion */
    void insert(uint8_t *key, int f = 1)
    {
        #if USE_CS == 0
        uint32_t hash_val = (uint32_t)bobhash->run((const char *)key, KEY_LENGTH_4);
        uint32_t pos = hash_val % (uint32_t)counter_num;

        int old_val = (int)counters[pos];
        int new_val = (int)counters[pos] + f;

        new_val = new_val < 255 ? new_val : 255;
        counters[pos] = (uint8_t)new_val;

        mice_dist[old_val]--;
        mice_dist[new_val]++;
        #elif USE_CS == 1
        uint32_t hash_val = (uint32_t)bobhash->run((const char *)key, KEY_LENGTH_4);
        uint32_t hash_val_ = (uint32_t)bobhash->run((const char *)key, 2);
        uint32_t pos = hash_val % (uint32_t)counter_num;
        uint32_t coe = (hash_val_ % 2) * 2 - 1;

        int old_val = (int)counters[pos];
        int new_val = old_val + f * coe;

        new_val = new_val < 255 ? new_val : 255;
        new_val = new_val > 0 ? new_val : 0;
        counters[pos] = (uint8_t)new_val;

        mice_dist[old_val]--;
        mice_dist[new_val]++;
        #elif USE_CS == 3
        uint32_t hash_val_1 = (uint32_t)bobhash->run((const char *)key, KEY_LENGTH_4);
        uint32_t hash_val_1_ = (uint32_t)bobhash->run((const char *)key, 2);
        uint32_t pos_1 = hash_val_1 % (uint32_t)counter_num;
        uint32_t coe_1 = (hash_val_1_ % 2) * 2 - 1;

        uint32_t hash_val_2 = (uint32_t)bobhash_->run((const char *)key, KEY_LENGTH_4);
        uint32_t hash_val_2_ = (uint32_t)bobhash_->run((const char *)key, 2);
        uint32_t pos_2 = hash_val_2 % (uint32_t)counter_num;
        uint32_t coe_2 = (hash_val_2_ % 2) * 2 - 1;

        uint32_t hash_val_3 = (uint32_t)bobhash__->run((const char *)key, KEY_LENGTH_4);
        uint32_t hash_val_3_ = (uint32_t)bobhash__->run((const char *)key, 2);
        uint32_t pos_3 = hash_val_3 % (uint32_t)counter_num;
        uint32_t coe_3 = (hash_val_3_ % 2) * 2 - 1;

        int old_val_1 = (int)counters_1[pos_1];
        int new_val_1 = old_val_1 + f * coe_1;
        new_val_1 = new_val_1 < 255 ? new_val_1 : 255;
        new_val_1 = new_val_1 > 0 ? new_val_1 : 0;
        counters_1[pos_1] = (uint8_t)new_val_1;

        int old_val_2 = (int)counters_2[pos_2];
        int new_val_2 = old_val_2 + f * coe_2;
        new_val_2 = new_val_2 < 255 ? new_val_2 : 255;
        new_val_2 = new_val_2 > 0 ? new_val_2 : 0;
        counters_2[pos_2] = (uint8_t)new_val_2;

        int old_val_3 = (int)counters_3[pos_3];
        int new_val_3 = old_val_3 + f * coe_3;
        new_val_3 = new_val_3 < 255 ? new_val_3 : 255;
        new_val_3 = new_val_3 > 0 ? new_val_3 : 0;
        counters_3[pos_3] = (uint8_t)new_val_3;
        #endif
    }

    void swap_insert(uint8_t *key, int f)
    {
        #if USE_CS == 0
        uint32_t hash_val = (uint32_t)bobhash->run((const char *)key, KEY_LENGTH_4);
        uint32_t pos = hash_val % (uint32_t)counter_num;

        f = f < 255 ? f : 255;
        if (counters[pos] < f)
        {
            int old_val = (int)counters[pos];
            counters[pos] = (uint8_t)f;
            int new_val = (int)counters[pos];

            mice_dist[old_val]--;
            mice_dist[new_val]++;
        }
        #elif USE_CS == 1
        uint32_t hash_val = (uint32_t)bobhash->run((const char *)key, KEY_LENGTH_4);
        uint32_t hash_val_ = (uint32_t)bobhash->run((const char *)key, 2);
        uint32_t pos = hash_val % (uint32_t)counter_num;
        uint32_t coe = (hash_val_ % 2) * 2 - 1;

        f = f * coe + 128;
        f = f < 255 ? f : 255;
        f = f > 0 ? f : 0;
        if ((counters[pos]>=128&&counters[pos]<f)||(counters[pos]<128&&counters[pos]>f))
        {
            int old_val = (int)counters[pos];
            counters[pos] = (uint8_t)f;
            int new_val = (int)counters[pos];

            mice_dist[old_val]--;
            mice_dist[new_val]++;
        }
        #elif USE_CS == 3
        uint32_t hash_val_1 = (uint32_t)bobhash->run((const char *)key, KEY_LENGTH_4);
        uint32_t hash_val_1_ = (uint32_t)bobhash->run((const char *)key, 2);
        uint32_t pos_1 = hash_val_1 % (uint32_t)counter_num;
        uint32_t coe_1 = (hash_val_1_ % 2) * 2 - 1;

        uint32_t hash_val_2 = (uint32_t)bobhash_->run((const char *)key, KEY_LENGTH_4);
        uint32_t hash_val_2_ = (uint32_t)bobhash_->run((const char *)key, 2);
        uint32_t pos_2 = hash_val_2 % (uint32_t)counter_num;
        uint32_t coe_2 = (hash_val_2_ % 2) * 2 - 1;

        uint32_t hash_val_3 = (uint32_t)bobhash__->run((const char *)key, KEY_LENGTH_4);
        uint32_t hash_val_3_ = (uint32_t)bobhash__->run((const char *)key, 2);
        uint32_t pos_3 = hash_val_3 % (uint32_t)counter_num;
        uint32_t coe_3 = (hash_val_3_ % 2) * 2 - 1;

        int f_1 = f * coe_1 + 128;
        f_1 = f_1 < 255 ? f_1 : 255;
        f_1 = f_1 > 0 ? f_1 : 0;
        if ((counters_1[pos_1]>=128&&counters_1[pos_1]<f_1)||(counters_1[pos_1]<128&&counters_1[pos_1]>f_1)) counters_1[pos_1] = (uint8_t)f_1;

        int f_2 = f * coe_2 + 128;
        f_2 = f_2 < 255 ? f_2 : 255;
        f_2 = f_2 > 0 ? f_2 : 0;
        if ((counters_2[pos_2]>=128&&counters_2[pos_2]<f_2)||(counters_2[pos_2]<128&&counters_2[pos_2]>f_2)) counters_2[pos_2] = (uint8_t)f_2;

        int f_3 = f * coe_3 + 128;
        f_3 = f_3 < 255 ? f_3 : 255;
        f_3 = f_3 > 0 ? f_3 : 0;
        if ((counters_3[pos_3]>=128&&counters_3[pos_3]<f_3)||(counters_3[pos_3]<128&&counters_3[pos_3]>f_3)) counters_3[pos_3] = (uint8_t)f_3;
        #endif
    }

    /* query */
    int query(uint8_t *key)
    {
        #if USE_CS == 0
        uint32_t hash_val = (uint32_t)bobhash->run((const char *)key, KEY_LENGTH_4);
        uint32_t pos = hash_val % (uint32_t)counter_num;
        return (int)counters[pos];
        #elif USE_CS == 1
        uint32_t hash_val = (uint32_t)bobhash->run((const char *)key, KEY_LENGTH_4);
        uint32_t hash_val_ = (uint32_t)bobhash->run((const char *)key, 2);
        uint32_t pos = hash_val % (uint32_t)counter_num;
        uint32_t coe = (hash_val_ % 2) * 2 - 1;
        return ((int)counters[pos]-128)*coe;
        #elif USE_CS == 3
        uint32_t hash_val_1 = (uint32_t)bobhash->run((const char *)key, KEY_LENGTH_4);
        uint32_t hash_val_1_ = (uint32_t)bobhash->run((const char *)key, 2);
        uint32_t pos_1 = hash_val_1 % (uint32_t)counter_num;
        uint32_t coe_1 = (hash_val_1_ % 2) * 2 - 1;

        uint32_t hash_val_2 = (uint32_t)bobhash_->run((const char *)key, KEY_LENGTH_4);
        uint32_t hash_val_2_ = (uint32_t)bobhash_->run((const char *)key, 2);
        uint32_t pos_2 = hash_val_2 % (uint32_t)counter_num;
        uint32_t coe_2 = (hash_val_2_ % 2) * 2 - 1;

        uint32_t hash_val_3 = (uint32_t)bobhash__->run((const char *)key, KEY_LENGTH_4);
        uint32_t hash_val_3_ = (uint32_t)bobhash__->run((const char *)key, 2);
        uint32_t pos_3 = hash_val_3 % (uint32_t)counter_num;
        uint32_t coe_3 = (hash_val_3_ % 2) * 2 - 1;

        int a = ((int)counters_1[pos_1]-128)*coe_1;
        int b = ((int)counters_2[pos_2]-128)*coe_2;
        int c = ((int)counters_3[pos_3]-128)*coe_3;
        return a > b ? (a < c ? a : std::max(b, c)) : (b < c ? b : std::max(a, c));
        #endif
    }

    #if USE_CS == 0
    /* compress */
    void compress(int ratio, uint8_t *dst)
    {
        int width = get_compress_width(ratio);

        for (int i = 0; i < width && i < counter_num; ++i)
        {
            uint8_t max_val = 0;
            for (int j = i; j < counter_num; j += width)
                max_val = counters[j] > max_val ? counters[j] : max_val;
            dst[i] = max_val;
        }
    }

    int query_compressed_part(uint8_t *key, uint8_t *compress_part, int compress_counter_num)
    {
        uint32_t hash_val = (uint32_t)bobhash->run((const char *)key, KEY_LENGTH_4);
        uint32_t pos = (hash_val % (uint32_t)counter_num) % compress_counter_num;

        return (int)compress_part[pos];
    }

    /* other measurement task */
    int get_compress_width(int ratio) { return (counter_num / ratio); }
    int get_compress_memory(int ratio) { return (uint32_t)(counter_num / ratio); }
    int get_memory_usage() { return counter_num; }

    int get_cardinality()
    {
        int mice_card = 0;
        for (int i = 1; i < 256; i++)
            mice_card += mice_dist[i];

        double rate = (counter_num - mice_card) / (double)counter_num;
        return counter_num * log(1 / rate);
    }

    void get_entropy(int &tot, double &entr)
    {
        for (int i = 1; i < 256; i++)
        {
            tot += mice_dist[i] * i;
            entr += mice_dist[i] * i * log2(i);
        }
    }

    void get_distribution(vector<double> &dist)
    {
        uint32_t tmp_counters[counter_num];
        for (int i = 0; i < counter_num; i++)
            tmp_counters[i] = counters[i];

        em_fsd_algo = new EMFSD();
        em_fsd_algo->set_counters(counter_num, tmp_counters, UINT32_MAX);

        for (int i = 0; i < 20; ++i)
            em_fsd_algo->next_epoch();

        dist = em_fsd_algo->ns;
    }
    #endif
};

#endif