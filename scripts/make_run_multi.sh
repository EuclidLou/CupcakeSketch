#!/bin/bash

# 定义需要调试的宏
USE_TOWER=0
# USE_CS_LIST=(0 1 3)
USE_CS_LIST=(1)
METRICS=0
MEMORY_LIST=($(seq 8 8 1024 | awk '{print $1*1024}'))
K=80000
DATASET="zipf_1.0"
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

    cd ~/Sketch/
    make clean > /dev/null

    if [ "$DATASET" = "caida" ]; then
        ITEM_SIZE=15
    elif [ "$DATASET" = "webdocs_form01" ]; then
        ITEM_SIZE=8
    elif [ "$DATASET" = "CAIDA_large" ]; then
        ITEM_SIZE=15
    fi

    echo "Building project with USE_CS=$use_cs and MEMORY_1_24=$memory"

    # 指定独特的可执行文件名
    executable_file="exec_use_cs_${use_cs}_memory_${memory}"
    
    # 编译生成独特的可执行文件
    make all MYFLAGS="-DCOSTUM -DUSE_TOWER=$USE_TOWER -DUSE_CS=$use_cs -DMETRICS=$METRICS -DMEMORY_1_24=$memory -DK=$K -DDATASET='\"$DATASET\"' -DITEM_SIZE=$ITEM_SIZE -DREPEAT=$REPEAT -DS_FACTOR=$S_FACTOR -DSWAP_FACTOR=$SWAP_FACTOR -DHEAVY_BIAS=$HEAVY_BIAS -DTIMES_RESAMPLE=$TIMES_RESAMPLE -DDHASH=$DHASH" DIRFLAG="${executable_file}" > /dev/null

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
    compile_and_run "$1" "$2" &
}

# 循环组合并并行执行
for memory in "${MEMORY_LIST[@]}"; do
    for use_cs in "${USE_CS_LIST[@]}"; do
        run_with_limit "$use_cs" "$memory"
    done
done

# 等待所有任务完成
wait
echo "All tasks finished."
