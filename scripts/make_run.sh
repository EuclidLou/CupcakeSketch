#!/bin/bash

# 定义需要调试的宏
USE_TOWER=0
USE_CS=1
METRICS=1
MEMORY_1_24=$((10*1024))
K=10000
DATASET="caida"
ITEM_SIZE=21
REPEAT=10
S_FACTOR=0.5

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
    echo "USE_TOWER = $USE_TOWER, USE_CS = $USE_CS, METRICS = $METRICS, MEMORY_1_24 = $MEMORY_1_24"
    echo "K = $K, DATASET = $DATASET, ITEM_SIZE = $ITEM_SIZE, REPEAT = $REPEAT, S_FACTOR = $S_FACTOR"
    
    # 根据宏定义编译项目
    make all MYFLAGS="-DCOSTUM -DUSE_TOWER=$USE_TOWER -DUSE_CS=$USE_CS -DMETRICS=$METRICS -DMEMORY_1_24=$MEMORY_1_24 -DK=$K -DDATASET='\"$DATASET\"' -DITEM_SIZE=$ITEM_SIZE -DREPEAT=$REPEAT -DS_FACTOR=$S_FACTOR" > /dev/null

    # 如果编译成功，运行程序
    if [ $? -eq 0 ]; then
        echo "Running program..."
        ./exec/exp > /dev/null # 替换为实际的可执行文件名称
    else
        echo "Compilation failed!"
    fi
}

# 调试组合
for DATASET in "caida" "zipf_0.5" "zipf_1.0" "zipf_1.5" "zipf_2.0" "zipf_2.5" "zipf_3.0"; do
for S_FACTOR in 0.1 0.2 0.3 0.4 0.5; do
for MEMORY_1_24 in $((1024)) $((10*1024)) $((100*1024)) $((1000*1024)) $((10000*1024)); do
for K in 1000 10000 100000 1000000; do
for USE_CS in 0 1; do
    compile_and_run
done
done
done
done
done
