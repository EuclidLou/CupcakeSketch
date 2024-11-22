#!/bin/bash

# 定义需要调试的宏
USE_TOWER=0
# USE_CS_LIST=(0 1 3)
USE_CS_LIST=(1)
METRICS=0
# MEMORY_LIST=($(seq 1 1 128 | awk '{print $1*256}'))
MEMORY_LIST=($((16*1024*1024)))
# K_LIST=($(seq 2 2 256 | awk '{print $1*1}'))
K_LIST=(2 4 8 16 32 64 128 256)
# K_LIST=(1024)
DATASET_LIST=("zipf_0.5" "caida" "webdocs_form01" "wikipedia_30")
# DATASET_LIST=("_zipf_0.4" "_zipf_0.5" "_zipf_0.6" "_zipf_0.7" "_zipf_0.8" "_zipf_0.9" "_zipf_0.10" "_zipf_0.11" "_zipf_0.12" "_zipf_0.13" "_zipf_0.14" "_zipf_0.15" "_zipf_0.16" "_zipf_0.17" "_zipf_0.18" "_zipf_0.19" "_zipf_0.20")
ITEM_SIZE=4
REPEAT=10
S_FACTOR=0.5
HEAVY_BIAS=1
SWAP_FACTOR=3
TIMES_RESAMPLE=1
DHASH=1

# 最大并发数
MAX_CONCURRENT_JOBS=128

# 编译和运行的函数
compile_and_run() {
    local use_cs=$1
    local memory=$2
    local dataset=$3
    local k=$4

    cd ~/Sketch/
    make clean > /dev/null

    if [ "$DATASET" = "caida" ]; then
        ITEM_SIZE=15
    elif [ "$DATASET" = "webdocs_form01" ]; then
        ITEM_SIZE=8
    elif [ "$DATASET" = "CAIDA_large" ]; then
        ITEM_SIZE=15
    elif [ "$DATASET" = "zipf_0.5" ]; then
        ITEM_SIZE=4
    elif [ "$DATASET" = "wikipedia_30" ]; then
        ITEM_SIZE=4
    fi

    echo "Building project with USE_CS=$use_cs, MEMORY_1_24=$memory and K=$k on $dataset"

    # 指定独特的可执行文件名
    executable_file="exec_use_cs_${use_cs}_memory_${memory}_K_${k}_${dataset}"
    
    # 编译生成独特的可执行文件
    make all MYFLAGS="-DCOSTUM -DUSE_TOWER=$USE_TOWER -DUSE_CS=$use_cs -DMETRICS=$METRICS -DMEMORY_1_24=$memory -DK=$k -DDATASET='\"$dataset\"' -DITEM_SIZE=$ITEM_SIZE -DREPEAT=$REPEAT -DS_FACTOR=$S_FACTOR -DSWAP_FACTOR=$SWAP_FACTOR -DHEAVY_BIAS=$HEAVY_BIAS -DTIMES_RESAMPLE=$TIMES_RESAMPLE -DDHASH=$DHASH" DIRFLAG="${executable_file}" > /dev/null

    if [ $? -eq 0 ]; then
        # 运行独特的可执行文件
        "./exec/${executable_file}" > /dev/null
    else
        echo "Compilation failed!"
    fi
}

# 用于控制并发数的函数
run_with_limit() {
    while (( $(jobs -r | wc -l) >= MAX_CONCURRENT_JOBS )); do
        sleep 1
    done
    compile_and_run "$1" "$2" "$3" "$4"&
}

# 循环组合并并行执行
for dataset in "${DATASET_LIST[@]}"; do
    for memory in "${MEMORY_LIST[@]}"; do
        for use_cs in "${USE_CS_LIST[@]}"; do
            for k in "${K_LIST[@]}"; do
                run_with_limit "$use_cs" "$memory" "$dataset" "$k"
            done
        done
    done
done

# 等待所有任务完成
wait
echo "All tasks finished."
