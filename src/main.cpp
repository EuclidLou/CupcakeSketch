#include "include/dataset.h"
#include "include/test_cm.h"
#include "include/test_cu.h"
#include "include/test_count_sketch.h"
#include "include/test_elastic.h"
#include "include/test_cycle.h"
#include "include/test_minhash.h"
#include "include/test_hll.h"
#include "include/test_maxloghash.h"
#include "include/novel_minhash.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#define MEMORY_1_12 (int)(400*1024)

void log_to_file(const std::string& message) {
    std::ofstream log_file("execution_log.txt", std::ios_base::app);
    if (log_file.is_open()) {
        std::time_t now = std::time(nullptr);
        log_file << std::ctime(&now) << message << std::endl;
        log_file.close();
    }
}

int main(){
  string dataset_name = "caida";
  int time_repeat = 1;
  int num_minhash_to_sample = 1000;
  Dataset dataset;
  dataset.init("./dataset/"+dataset_name+".dat", 4, true, true, 1.0, true);
  double gt = dataset.similarity();
  double est_simi = distribution_cycle(time_repeat,MEMORY_1_12,dataset_name);   //mem = 24*len
  double est_mihs = enroll_novel_minhash<12*MEMORY_1_12>(time_repeat, num_minhash_to_sample, dataset_name); //mem = 2*<>
  double RE_simi = (est_simi - gt) / gt * 100.0;
  double RE_mihs = (est_mihs - gt) / gt * 100.0;
  LOG_RESULT("Relative Error of SimiSketch: %lf%c", RE_simi, '%');
  LOG_RESULT("Relative Error of Novel MinHash: %lf%c", RE_mihs, '%');
  log_to_file("Dataset: "+dataset_name+"\t Sample time: "+std::to_string(num_minhash_to_sample)+"\t Memory: "+std::to_string(MEMORY_1_12*12.0/1024.0)+"KB"+"\t GT: "+std::to_string(gt)+"\t Simisketch: "+std::to_string(est_simi)+"@"+std::to_string(RE_simi/100)+"\t Ours: "+std::to_string(est_mihs)+"@"+std::to_string(RE_mihs/100));
}