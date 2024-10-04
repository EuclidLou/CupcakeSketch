#ifndef MACROS_H
#define MACROS_H


// #define USE_TOWER
#define METRICS
#define MEMORY_1_24     (int)(10*1024)
#define K               10000
#define DATASET         "caida"
#define REPEAT          5


#endif


/*
TODO:   1. 数据集切分
        2. 使用Count Sketch作为小流计数
        3. 调参脚本
        4. 小流数据回读
        5. CAIDA的数据格式？
        6. 改进测试函数的数据集入口，节约数据集初始化时间
*/