#!/bin/bash

# 定义需要调试的宏
USE_TOWER=0
USE_CS=1
METRICS=1
MEMORY_1_24=$((10*1024))
K=10000
DATASET="caida"
ITEM_SIZE=21
REPEAT=100
S_FACTOR=0.5
HEAVY_BIAS      1
SWAP_FACTOR     3

# 编译过程
compile_and_run() {
    cd ~/Sketch/
    echo "Cleaning project..."
    make clean > /dev/null

    if [ "$DATASET" = "caida" ]; then
        ITEM_SIZE=21
    else
        ITEM_SIZE=4
    fi
    
    echo "Building project with parameters:"
    echo "USE_TOWER = $USE_TOWER, USE_CS = $USE_CS, METRICS = $METRICS, MEMORY_1_24 = $MEMORY_1_24, SWAP_FACTOR = $SWAP_FACTOR"
    echo "K = $K, DATASET = $DATASET, ITEM_SIZE = $ITEM_SIZE, REPEAT = $REPEAT, S_FACTOR = $S_FACTOR, HEAVY_BIAS = $HEAVY_BIAS"
    
    # 根据宏定义编译项目
    make all MYFLAGS="-DCOSTUM -DUSE_TOWER=$USE_TOWER -DUSE_CS=$USE_CS -DMETRICS=$METRICS -DMEMORY_1_24=$MEMORY_1_24 -DK=$K -DDATASET='\"$DATASET\"' -DITEM_SIZE=$ITEM_SIZE -DREPEAT=$REPEAT -DS_FACTOR=$S_FACTOR -DSWAP_FACTOR=$SWAP_FACTOR -DHEAVY_BIAS=$HEAVY_BIAS" > /dev/null

    # 如果编译成功，运行程序
    if [ $? -eq 0 ]; then
        echo "Running program..."
        ./exec/exp > /dev/null # 替换为实际的可执行文件名称
    else
        echo "Compilation failed!"
    fi
}

# 调试组合
for HEAVY_BIAS in 0.125 0.25 0.5 1 2 3; do
for SWAP_FACTOR in 1 2 3 4; do
for MEMORY_1_24 in $((1024)) $((2*1024)) $((4*1024)) $((8*1024)) $((16*1024)) $((32*1024)) $((64*1024)); do
for K in 10000 20000 40000 80000 160000 320000; do
for USE_CS in 0 1 3; do
    compile_and_run
done
done
done
done
done
