#ifndef _HEAVYPART_H_
#define _HEAVYPART_H_

#include "param.h"

#if DHASH == 0
template <int bucket_num>
class HeavyPart
{
public:
    alignas(64) Bucket buckets[bucket_num];

    HeavyPart()
    {
        clear();
    }
    ~HeavyPart() {}

    void clear()
    {
        memset(buckets, 0, sizeof(Bucket) * bucket_num);
    }

    /* insertion */
    int insert(uint8_t *key, uint8_t *swap_key, uint32_t &swap_val, uint32_t f = 1)
    {
        uint32_t fp;
        int pos = CalculateFP(key, fp);

        /* find if there has matched bucket */
        const __m256i item = _mm256_set1_epi32((int)fp);
        __m256i *keys_p = (__m256i *)(buckets[pos].key);
        int matched = 0;

        __m256i a_comp = _mm256_cmpeq_epi32(item, keys_p[0]);
        matched = _mm256_movemask_ps((__m256)a_comp);

        /* if matched */
        if (matched != 0)
        {
            // return 32 if input is zero;
            int matched_index = _tzcnt_u32((uint32_t)matched);
            buckets[pos].val[matched_index] += f;
            return 0;
        }

        /* find the minimal bucket */
        const uint32_t mask_base = 0x7FFFFFFF;
        const __m256i *counters = (__m256i *)(buckets[pos].val);
        __m256 masks = (__m256)_mm256_set1_epi32(mask_base);
        __m256 results = (_mm256_and_ps(*(__m256 *)counters, masks));
        __m256 mask2 = (__m256)_mm256_set_epi32(mask_base, 0, 0, 0, 0, 0, 0, 0);
        results = _mm256_or_ps(results, mask2);

        __m128i low_part = _mm_castps_si128(_mm256_extractf128_ps(results, 0));
        __m128i high_part = _mm_castps_si128(_mm256_extractf128_ps(results, 1));

        __m128i x = _mm_min_epi32(low_part, high_part);
        __m128i min1 = _mm_shuffle_epi32(x, _MM_SHUFFLE(0, 0, 3, 2));
        __m128i min2 = _mm_min_epi32(x, min1);
        __m128i min3 = _mm_shuffle_epi32(min2, _MM_SHUFFLE(0, 0, 0, 1));
        __m128i min4 = _mm_min_epi32(min2, min3);
        int min_counter_val = _mm_cvtsi128_si32(min4);

        const __m256i ct_item = _mm256_set1_epi32(min_counter_val);
        int ct_matched = 0;

        __m256i ct_a_comp = _mm256_cmpeq_epi32(ct_item, (__m256i)results);
        matched = _mm256_movemask_ps((__m256)ct_a_comp);
        int min_counter = _tzcnt_u32((uint32_t)matched);

        /* if there has empty bucket */
        if (min_counter_val == 0) // empty counter
        {
            buckets[pos].key[min_counter] = fp;
            buckets[pos].val[min_counter] = f;
            return 0;
        }

        /* update guard val and comparison */
        uint32_t guard_val = buckets[pos].val[MAX_VALID_COUNTER];
        guard_val = UPDATE_GUARD_VAL(guard_val);

        if (!JUDGE_IF_SWAP(GetCounterVal(min_counter_val), guard_val))
        {
            buckets[pos].val[MAX_VALID_COUNTER] = guard_val;
            return 2;
        }

        *((uint32_t *)swap_key) = buckets[pos].key[min_counter];
        swap_val = buckets[pos].val[min_counter];

        buckets[pos].val[MAX_VALID_COUNTER] = 0;

        buckets[pos].key[min_counter] = fp;
        buckets[pos].val[min_counter] = 0x80000001;

        return 1;
    }

    int quick_insert(uint8_t *key, uint32_t f = 1)
    {
        uint32_t fp;
        int pos = CalculateFP(key, fp);

        const __m256i item = _mm256_set1_epi32((int)fp);
        __m256i *keys_p = (__m256i *)(buckets[pos].key);
        int matched = 0;

        __m256i a_comp = _mm256_cmpeq_epi32(item, keys_p[0]);
        matched = _mm256_movemask_ps((__m256)a_comp);

        if (matched != 0)
        {
            int matched_index = _tzcnt_u32((uint32_t)matched);
            buckets[pos].val[matched_index] += f;
            return 0;
        }

        const uint32_t mask_base = 0x7FFFFFFF;
        const __m256i *counters = (__m256i *)(buckets[pos].val);
        __m256 masks = (__m256)_mm256_set1_epi32(mask_base);
        __m256 results = (_mm256_and_ps(*(__m256 *)counters, masks));
        __m256 mask2 = (__m256)_mm256_set_epi32(mask_base, 0, 0, 0, 0, 0, 0, 0);
        results = _mm256_or_ps(results, mask2);

        __m128i low_part = _mm_castps_si128(_mm256_extractf128_ps(results, 0));
        __m128i high_part = _mm_castps_si128(_mm256_extractf128_ps(results, 1));

        __m128i x = _mm_min_epi32(low_part, high_part);
        __m128i min1 = _mm_shuffle_epi32(x, _MM_SHUFFLE(0, 0, 3, 2));
        __m128i min2 = _mm_min_epi32(x, min1);
        __m128i min3 = _mm_shuffle_epi32(min2, _MM_SHUFFLE(0, 0, 0, 1));
        __m128i min4 = _mm_min_epi32(min2, min3);
        int min_counter_val = _mm_cvtsi128_si32(min4);

        const __m256i ct_item = _mm256_set1_epi32(min_counter_val);
        int ct_matched = 0;

        __m256i ct_a_comp = _mm256_cmpeq_epi32(ct_item, (__m256i)results);
        matched = _mm256_movemask_ps((__m256)ct_a_comp);
        int min_counter = _tzcnt_u32((uint32_t)matched);

        if (min_counter_val == 0)
        {
            buckets[pos].key[min_counter] = fp;
            buckets[pos].val[min_counter] = f;
            return 0;
        }

        uint32_t guard_val = buckets[pos].val[MAX_VALID_COUNTER];
        guard_val = UPDATE_GUARD_VAL(guard_val);

        if (!JUDGE_IF_SWAP(min_counter_val, guard_val))
        {
            buckets[pos].val[MAX_VALID_COUNTER] = guard_val;
            return 2;
        }

        buckets[pos].val[MAX_VALID_COUNTER] = 0;

        buckets[pos].key[min_counter] = fp;
        return 1;
    }

    /* query */
    uint32_t query(uint8_t *key)
    {
        uint32_t fp;
        int pos = CalculateFP(key, fp);

        for (int i = 0; i < MAX_VALID_COUNTER; ++i)
            if (buckets[pos].key[i] == fp)
                return buckets[pos].val[i];

        return 0;
    }

    /* interface */
    int get_memory_usage()
    {
        return bucket_num * sizeof(Bucket);
    }
    int get_bucket_num()
    {
        return bucket_num;
    }

private:
    int CalculateFP(uint8_t *key, uint32_t &fp)
    {
        fp = *((uint32_t *)key);
        return CalculateBucketPos(fp) % bucket_num;
    }
};
#else
template <int bucket_num>
class HeavyPart
{
public:
    static constexpr int per_bucket_num = bucket_num / 2;
    alignas(64) Bucket buckets_1[per_bucket_num];
    alignas(64) Bucket buckets_2[per_bucket_num];

    HeavyPart()
    {
        clear();
    }
    ~HeavyPart() {}

    void clear()
    {
        memset(buckets_1, 0, sizeof(Bucket) * per_bucket_num);
        memset(buckets_2, 0, sizeof(Bucket) * per_bucket_num);
    }

    /* insertion */
    int insert(uint8_t *key, uint8_t *swap_key, uint32_t &swap_val, uint32_t f = 1)
    {
        uint32_t fp;
        int pos_1 = CalculateFP(key, fp);
        int pos_2 = (pos_1 + (fp >> 24)) % per_bucket_num;

        /* find if there has matched bucket */
        const __m256i item = _mm256_set1_epi32((int)fp);
        __m256i *keys_p_1 = (__m256i *)(buckets_1[pos_1].key);
        __m256i *keys_p_2 = (__m256i *)(buckets_2[pos_2].key);
        int matched_1 = 0, matched_2 = 0;

        __m256i a_comp_1 = _mm256_cmpeq_epi32(item, keys_p_1[0]);
        __m256i a_comp_2 = _mm256_cmpeq_epi32(item, keys_p_2[0]);
        matched_1 = _mm256_movemask_ps((__m256)a_comp_1);
        matched_2 = _mm256_movemask_ps((__m256)a_comp_2);

        /* if matched */
        if (matched_1 != 0)
        {
            // return 32 if input is zero;
            int matched_index = _tzcnt_u32((uint32_t)matched_1);
            buckets_1[pos_1].val[matched_index] += f;
            return 0;
        }
        if (matched_2 != 0)
        {
            // return 32 if input is zero;
            int matched_index = _tzcnt_u32((uint32_t)matched_2);
            buckets_2[pos_2].val[matched_index] += f;
            return 0;
        }

        /* find the minimal bucket */
        const uint32_t mask_base = 0x7FFFFFFF;
        const __m256i *counters_1 = (__m256i *)(buckets_1[pos_1].val);
        const __m256i *counters_2 = (__m256i *)(buckets_2[pos_2].val);
        __m256 masks = (__m256)_mm256_set1_epi32(mask_base);
        __m256 results_1 = (_mm256_and_ps(*(__m256 *)counters_1, masks));
        __m256 results_2 = (_mm256_and_ps(*(__m256 *)counters_2, masks));
        // __m256 mask2 = (__m256)_mm256_set_epi32(mask_base, 0, 0, 0, 0, 0, 0, 0);
        // results_1 = _mm256_or_ps(results_1, mask2);
        // results_2 = _mm256_or_ps(results_2, mask2);

        __m128i low_part_1 = _mm_castps_si128(_mm256_extractf128_ps(results_1, 0));
        __m128i high_part_1 = _mm_castps_si128(_mm256_extractf128_ps(results_1, 1));
        __m128i low_part_2 = _mm_castps_si128(_mm256_extractf128_ps(results_2, 0));
        __m128i high_part_2 = _mm_castps_si128(_mm256_extractf128_ps(results_2, 1));

        __m128i x_1 = _mm_min_epi32(low_part_1, high_part_1);
        __m128i min1_1 = _mm_shuffle_epi32(x_1, _MM_SHUFFLE(0, 0, 3, 2));
        __m128i min2_1 = _mm_min_epi32(x_1, min1_1);
        __m128i min3_1 = _mm_shuffle_epi32(min2_1, _MM_SHUFFLE(0, 0, 0, 1));
        __m128i min4_1 = _mm_min_epi32(min2_1, min3_1);
        int min_counter_val_1 = _mm_cvtsi128_si32(min4_1);
        __m128i x_2 = _mm_min_epi32(low_part_2, high_part_2);
        __m128i min1_2 = _mm_shuffle_epi32(x_2, _MM_SHUFFLE(0, 0, 3, 2));
        __m128i min2_2 = _mm_min_epi32(x_2, min1_2);
        __m128i min3_2 = _mm_shuffle_epi32(min2_2, _MM_SHUFFLE(0, 0, 0, 1));
        __m128i min4_2 = _mm_min_epi32(min2_2, min3_2);
        int min_counter_val_2 = _mm_cvtsi128_si32(min4_2);

        const __m256i ct_item_1 = _mm256_set1_epi32(min_counter_val_1);
        const __m256i ct_item_2 = _mm256_set1_epi32(min_counter_val_2);
        int ct_matched_1 = 0, ct_matched_2 = 0;

        __m256i ct_a_comp_1 = _mm256_cmpeq_epi32(ct_item_1, (__m256i)results_1);
        __m256i ct_a_comp_2 = _mm256_cmpeq_epi32(ct_item_2, (__m256i)results_2);
        matched_1 = _mm256_movemask_ps((__m256)ct_a_comp_1);
        matched_2 = _mm256_movemask_ps((__m256)ct_a_comp_2);
        int min_counter_1 = _tzcnt_u32((uint32_t)matched_1);
        int min_counter_2 = _tzcnt_u32((uint32_t)matched_2);

        /* if there has empty bucket */
        if (min_counter_val_1 == 0) // empty counter
        {
            buckets_1[pos_1].key[min_counter_1] = fp;
            buckets_1[pos_1].val[min_counter_1] = f;
            return 0;
        }
        if (min_counter_val_2 == 0) // empty counter
        {
            buckets_2[pos_2].key[min_counter_2] = fp;
            buckets_2[pos_2].val[min_counter_2] = f;
            return 0;
        }

        /* comparison */
        if (min_counter_val_1 < min_counter_val_2){
            if (min_counter_val_1 < swap_val+f){
                *((uint32_t *)swap_key) = buckets_1[pos_1].key[min_counter_1];
                buckets_1[pos_1].key[min_counter_1] = fp;
                buckets_1[pos_1].val[min_counter_1] = swap_val+f;
                swap_val = min_counter_val_1;
                return 1;
            }
        }
        else {
            if (min_counter_val_2 < swap_val+f){
                *((uint32_t *)swap_key) = buckets_2[pos_2].key[min_counter_2];
                buckets_2[pos_2].key[min_counter_2] = fp;
                buckets_2[pos_2].val[min_counter_2] = swap_val+f;
                swap_val = min_counter_val_2;
                return 1;
            }
        }

        return 2;
    }

    /* query */
    uint32_t query(uint8_t *key)
    {
        uint32_t fp;
        int pos_1 = CalculateFP(key, fp);
        int pos_2 = (pos_1 + (fp >> 24)) % per_bucket_num;

        for (int i = 0; i <= MAX_VALID_COUNTER; ++i){
            if (buckets_1[pos_1].key[i] == fp)
                return buckets_1[pos_1].val[i];
            if (buckets_2[pos_2].key[i] == fp)
                return buckets_2[pos_2].val[i];
        }

        return 0;
    }

    /* interface */
    int get_memory_usage()
    {
        return bucket_num * sizeof(Bucket);
    }
    int get_bucket_num()
    {
        return bucket_num;
    }

private:
    int CalculateFP(uint8_t *key, uint32_t &fp)
    {
        fp = *((uint32_t *)key);
        return CalculateBucketPos(fp) % per_bucket_num;
    }
};
#endif

#endif
