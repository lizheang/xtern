// generate algorithm example
#include <iostream>     // std::cout
#include <algorithm>    // std::generate
#include <vector>       // std::vector
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand
#include <parallel/algorithm>
#include "microbench.h"

// function generator:
//int RandomNumber () { return (std::rand()%100); }

//class generator:
struct c_unique {
  int current;
  c_unique() {current=1;}
  //int operator()() {return ++current;}
  int operator()() {return current;}
} UniqueNumber;

unsigned int data_size = 0;

//std::vector<int> myvector(1000*1000*100);
std::vector<int> myvector(data_size);

int main (int argc, char * argv[]) {
    SET_INPUT_SIZE(argc, argv[1])
    myvector.resize(data_size);

    struct timeval start, end;
    fprintf(stderr, "omp num threads %d\n", omp_get_max_threads());
//    std::srand(SEED);

    gettimeofday(&start, NULL);
//    __gnu_parallel::generate_n (myvector.begin(), myvector.size(), RandomNumber);
    __gnu_parallel::generate_n (myvector.begin(), myvector.size(), UniqueNumber);
    gettimeofday(&end, NULL);
    fprintf(stderr, "real %.3f\n", ((end.tv_sec * 1000000 + end.tv_usec)
          - (start.tv_sec * 1000000 + start.tv_usec)) / 1000000.0);


//  std::cout << "myvector contains:";
//  for (std::vector<int>::iterator it=myvector.begin(); it!=myvector.end(); ++it)
//    std::cout << ' ' << *it;
//  std::cout << '\n';

//  __gnu_parallel::generate_n (myvector.begin(), N_SIZE, UniqueNumber);

//  std::cout << "myvector contains:";
//  for (std::vector<int>::iterator it=myvector.begin(); it!=myvector.end(); ++it)
//    std::cout << ' ' << *it;
//  std::cout << '\n';
 
  return 0;
}