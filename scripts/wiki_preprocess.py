import pandas as pd
import re
import hashlib
import struct
from tqdm import tqdm

class preprocess:
    def __init__(self):    
        self.words_num = 0
        
    def hash_word(self, word):
        """
        将单词映射为32位的hash值
        """
        # 使用 hashlib 计算 md5 并取前4字节作为32位hash值
        md5_hash = hashlib.md5(word.encode('utf-8')).digest()
        return struct.unpack("I", md5_hash[:4])[0]

    def process_text(self, text):
        """
        提取段落中的单词并生成32位hash值
        """
        # 保留单词中的英文字母，忽略标点符号、数字和其他字符
        words = re.findall(r'[a-zA-Z]+', text)
        self.words_num += len(words)
        # words = text.split()
        return [self.hash_word(word.lower()) for word in words]

    def save_hashes_to_binary(self, hashes, output_file):
        """
        将hash值以二进制格式保存到文件中
        """
        with open(output_file, 'wb') as f:
            for h in hashes:
                f.write(struct.pack("I", h))  # 以4字节写入每个32位hash值

    def main(self, parquet_file, output_file):
        # 读取Parquet文件
        df = pd.read_parquet(parquet_file)
        
        if 'text' not in df.columns:
            print("Parquet 文件中不包含 'text' 列！")
            return

        # 存储所有hash值
        all_hashes = []

        # 对每个段落处理
        for text in tqdm(df['text'], desc="Processing texts"):
            if isinstance(text, str):  # 确保是字符串
                hashes = self.process_text(text)
                all_hashes.extend(hashes)
        
        # 保存hash值到二进制文件
        self.save_hashes_to_binary(all_hashes, output_file)
        print(f"所有hash值已保存到 {output_file} 中。")

if __name__ == "__main__":
    # 替换为实际的Parquet文件路径和输出文件路径
    parquet_file = "/home/louyaopeng/Sketch/dataset/train-00000-of-00041.parquet"
    output_file = "/home/louyaopeng/Sketch/dataset/wikipedia_00.dat"
    process = preprocess()
    process.main(parquet_file, output_file)
    print(process.words_num)
