import numpy as np
import struct
import random
import argparse

def generate_skewed_data(output_file, skewness, tot_pkgs):
    """
    生成具有特定数据倾斜度的数据集并保存到二进制文件中。

    参数:
        output_file (str): 输出文件路径。
        skewness (float): 数据倾斜度，值越大越倾斜。
        total_size (int): 数据总大小（字节）。
    """
    
    num_ids = tot_pkgs

    ranks = np.arange(1, num_ids + 1)
    probabilities = 1 / np.power(ranks, skewness)
    probabilities /= probabilities.sum()

    frequencies = np.random.multinomial(num_ids, probabilities)
    # all = frequencies.sum()
    ids = np.arange(num_ids, dtype=np.uint32)

    dataset = []
    for id_value, freq in zip(ids, frequencies):
        dataset.extend([id_value] * freq)

    random.shuffle(dataset)

    with open(output_file, 'wb') as f:
        for id_value in dataset:
            f.write(struct.pack('I', id_value))
    
    print(f"数据已写入 {output_file}，倾斜度为 {skewness}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='zipf')
    parser.add_argument('--output_file', '-o', type=str, default='/home/louyaopeng/Sketch/dataset/_zipf_1.0.dat')
    parser.add_argument('--total_size', '-t', type=int, default=256*1024*1024)
    parser.add_argument('--skewness', '-s', type=float, default=1.0)
    args = parser.parse_args()

    generate_skewed_data(args.output_file, args.skewness, args.total_size / 4)
