#include <chrono>
#include <iostream>
#include <iomanip>
#include <random>
#include <thread>
#include <vector>

#include <pthread.h>
#include <stdio.h>

//------------------------------------------------------------------------

constexpr int FLOAT_MIN = 1;
constexpr int FLOAT_MAX = 10000;

using timePoint = std::chrono::system_clock::time_point;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

double parallelResult = 0;
double ompResult = 0;

int sizeArray = 500000000;
std::vector<double> integerArray(sizeArray);

struct ThreadInfo {
  int initialLine;
  int finalLine;
};

//------------------------------------------------------------------------

void StartSequencialSum();

void SumArrayElementsSequencial (double& result);

void StartParallelPThreadsSum();

void* Run (void* args);

void SumArrayElementsParalellPThreads (const int initialLine, const int finalLine);

void StartParallelOMPSum();

void SumArrayElementsParallelOMP();

void FillVectorWithRandomInteger(std::vector<double>& vector);

//------------------------------------------------------------------------

int main() {
  
  FillVectorWithRandomInteger(integerArray);
  
  printf("Tamanho do array: %d\n", sizeArray);
  
  StartSequencialSum();
  
  StartParallelPThreadsSum();
  
  StartParallelOMPSum();

  pthread_exit(nullptr);

  return 0;
}

//------------------------------------------------------------------------

void StartSequencialSum()
{
  double sequencialResult = 0;
  
  timePoint initialTime = std::chrono::system_clock::now();
  SumArrayElementsSequencial(sequencialResult);
  timePoint finalTime = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = finalTime - initialTime;
  std::cout << "Soma Array Sequencial: " << elapsed_seconds.count() << "s" << std::endl;
  std::cout << "Resultado Sequencial: " << sequencialResult << std::endl;
  printf("\n");
}

//------------------------------------------------------------------------

void SumArrayElementsSequencial (double& result)
{
  for(int i = 0; i < integerArray.size(); i++) {
    result += integerArray[i];
  }
}

//------------------------------------------------------------------------

void StartParallelPThreadsSum()
{
  const unsigned int numberOfThreads = 8;//std::thread::hardware_concurrency();
  
  pthread_t threads[numberOfThreads];
  
  struct ThreadInfo *threadInfo;
  std::vector<ThreadInfo*> threadsParam;
  const int linesPerThread = sizeArray / numberOfThreads;
  int resto = sizeArray % numberOfThreads;
  int finalLine = 0;
  
  for (int i = 0; i < numberOfThreads; i++) {
    threadInfo = new ThreadInfo{0, 0};
    threadInfo->initialLine = finalLine;
    threadInfo->finalLine = threadInfo->initialLine + linesPerThread;
    
    if (resto != 0) {
      threadInfo->finalLine++;
      resto--;
    }
    finalLine = threadInfo->finalLine;
    threadsParam.push_back(threadInfo);
  }
  
  timePoint initialTime = std::chrono::system_clock::now();
  for (int i = 0; i < numberOfThreads; i++) {
    pthread_create(&threads[i], nullptr, Run, (void*)threadsParam[i]);
  }
  
  for(int i = 0; i < numberOfThreads; i++) {
    pthread_join(threads[i], nullptr);
  }
  
  timePoint finalTime = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = finalTime  - initialTime;
  std::cout << "Soma Array PThreads: " << elapsed_seconds.count() << "s" << std::endl;
  std::cout << "Resultado da soma: " << parallelResult << std::endl;
  printf("\n");
}

//------------------------------------------------------------------------

void* Run (void* arg)
{
  struct ThreadInfo *info = (ThreadInfo*)arg;
  
  SumArrayElementsParalellPThreads(info->initialLine, info->finalLine);
  
  return 0;
}

//------------------------------------------------------------------------

void SumArrayElementsParalellPThreads (const int initialLine, const int finalLine)
{
  double partialSum = 0;
  for(int i = initialLine; i < finalLine; i++) {
    partialSum += integerArray[i];
  }
  
  pthread_mutex_lock(&mutex);
  parallelResult += partialSum;
  pthread_mutex_unlock(&mutex);
}

//------------------------------------------------------------------------

void StartParallelOMPSum()
{
  timePoint initialTime = std::chrono::system_clock::now();
  SumArrayElementsParallelOMP();
  timePoint finalTime = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = finalTime - initialTime;
  std::cout << "Soma Array OMP: " << elapsed_seconds.count() << "s" << std::endl;
  std::cout << "Resultado OMP: " << ompResult << std::endl;
}

//------------------------------------------------------------------------

void SumArrayElementsParallelOMP ()
{
  double partialSum = 0;
  
  #pragma omp parallel for reduction(+:partialSum)
  for(int i = 0; i < integerArray.size(); i++) {
    partialSum += integerArray[i];
  }
  #pragma omp critical
  {
    ompResult += partialSum;
  }
}

//------------------------------------------------------------------------

void FillVectorWithRandomInteger(std::vector<double>& vector)
{
  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_real_distribution<> distr(FLOAT_MIN, FLOAT_MAX);
  std::setprecision(10);
  
  for (int i = 0; i < vector.size(); i++) {
  vector.at(i) = distr(eng);
  }
}

//------------------------------------------------------------------------
