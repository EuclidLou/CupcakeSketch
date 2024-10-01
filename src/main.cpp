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


int main(){
  Dataset dataset;
  dataset.init("./dataset/zipf_0.5.dat",4);
  double gt = dataset.similarity();
  // double est_simi = distribution_cycle(1,1000000,"zipf_0.5");
  double est_mihs = enroll_novel_minhash(1, 1000000, 1000000, "zipf_0.5");
  // double RE_simi = (est_simi - gt) / gt * 100.0;
  double RE_mihs = (est_mihs - gt) / gt * 100.0;
  // LOG_RESULT("Relative Error of SimiSketch: %lf%c", RE_simi, '%');
  LOG_RESULT("Relative Error of Novel MinHash: %lf%c", RE_mihs, '%');
}