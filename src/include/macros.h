#ifndef MACROS_H
#define MACROS_H

#ifndef COSTUM


#define USE_TOWER       0
#define USE_CS          1
#define METRICS         1
#define MEMORY_1_24     (int)(512*1024)
#define K               10000
#define DATASET         "caida"
#define ITEM_SIZE       21
#define REPEAT          5
#define S_FACTOR        0.5



#endif
#endif


/*
TODO:   1. 数据集切分->OK
        2. 使用Count Sketch作为小流计数->OK
        3. 调参脚本
        4. 小流数据回读
        5. CAIDA的数据格式？
        6. 改进测试函数的数据集入口，节约数据集初始化时间
*/