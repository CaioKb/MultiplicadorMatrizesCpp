#include "Matrix/Matrix.h"

#include <chrono>
#include <iostream>
#include <thread>

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

//-------------------------------------------------
using timePoint = std::chrono::system_clock::time_point;

const unsigned int numberOfThreads = std::thread::hardware_concurrency();
Matrix m1(500, 500, "M1");
Matrix m2(500, 500, "M2");

timePoint endTimeProduct;
timePoint endTimePosicional;
timePoint inicialTimePosicional;

struct ThreadInfo {
  Matrix *result1;
  int initialLine;
  int finalLine;
};

namespace {
  void MultiplyMatrixSequencial(Matrix& result)
  {
    const int rows = m1.GetRows();
    const int columns = m2.GetColumns();
    
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < columns; j++) {
        for (int k = 0; k < rows; k++) {
          result.At(i, j) = result.At(i, j) + (m1.At(i, k) * m2.At(k, j));
        }
      }
    }
  }
  
  //-------------------------------------------------
  
  void MultiplyMatrixParallelPThreads(Matrix& result, const int initialLine, const int finalLine)
  {
    const int start = initialLine;
    const int end = finalLine == 0 ? m1.GetRows() : finalLine;
    const int columns = m2.GetColumns();
    
    for (int i = start; i < end; i++) {
      for (int j = 0; j < columns; j++) {
        for (int k = 0; k < columns; k++) {
          result.At(i, j) = result.At(i, j) + (m1.At(i, k) * m2.At(k, j));
        }
      }
    }
  }
  
  //-------------------------------------------------
  
  void MultiplyMatrixParallelOpenMP(Matrix& result)
  {
    const int rows = m1.GetRows();
    const int columns = m2.GetColumns();
    
    #pragma omp parallel for
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < columns; j++) {
        for (int k = 0; k < rows; k++) {
          result.At(i, j) = result.At(i, j) + (m1.At(i, k) * m2.At(k, j));
        }
      }
    }
  }
  
  //-------------------------------------------------

  void MultiplyMatrixByPositionSequencial(Matrix& result)
  {
    const int rows = m1.GetRows();
    const int columns = m2.GetColumns();
    
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < columns; j++) {
        result.At(i, j) = m1.At(i, j) * m2.At(i, j);
      }
    }
  }
  
  //-------------------------------------------------

  void MultiplyMatrixByPositionParallelPThreads(Matrix& result, const int initialLine, const int finalLine)
  {
    const int start = initialLine;
    const int end = finalLine == 0 ? m1.GetRows() : finalLine;
    const int columns = m2.GetColumns();
    
    for (int i = start; i < end; i++) {
      for (int j = 0; j < columns; j++) {
        result.At(i, j) = m1.At(i, j) * m2.At(i, j);
      }
    }
  }
  
  //-------------------------------------------------

  void MultiplyMatrixByPositionParallelOpenMP(Matrix& result)
  {
    const int rows = m1.GetRows();
    const int columns = m2.GetColumns();
    
    #pragma omp parallel for
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < columns; j++) {
        result.At(i, j) = m1.At(i, j) * m2.At(i, j);
      }
    }
  }
  
  //-------------------------------------------------

  void StartSequencial(const int rows, const int columns)
  {
    Matrix sequencialProductResult(rows, columns, "ProdutoMatricial");
    Matrix sequencialPosicionalResult(rows, columns, "ProdutoPosicional");
    
    timePoint initialTime = std::chrono::system_clock::now();
    MultiplyMatrixSequencial(sequencialProductResult);
    timePoint finalTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = finalTime - initialTime;
    std::cout << "Produto matricial sequencial: " << elapsed_seconds.count() << std::endl;
    
    initialTime = std::chrono::system_clock::now();
    MultiplyMatrixByPositionSequencial(sequencialPosicionalResult);
    finalTime = std::chrono::system_clock::now();
    elapsed_seconds = finalTime - initialTime;
    std::cout << "Produto posicional sequencial: " << elapsed_seconds.count() << std::endl;
  }
  
  //-------------------------------------------------
  
  void* runProduct(void* arg)
  {
    struct ThreadInfo *info = (ThreadInfo*)arg;
    MultiplyMatrixParallelPThreads(*info->result1, info->initialLine, info->finalLine);
    endTimeProduct = std::chrono::system_clock::now();
    inicialTimePosicional = std::chrono::system_clock::now();
    MultiplyMatrixByPositionParallelPThreads(*info->result1, info->initialLine, info->finalLine);
    endTimePosicional = std::chrono::system_clock::now();
    return 0;
  }
  
  void StartPThreads(const int rows, const int columns)
  {
    Matrix pThreadsProductResult(rows, columns, "ProdutoMatricialPThreads");
    Matrix pThreadsPosicionalResult(rows, columns, "ProdutoPosicionalPThreads");
    
    pthread_t threads[numberOfThreads];
    
    const int totalLines = rows;
    int resto = totalLines % numberOfThreads;
    int linesPerThread = totalLines / numberOfThreads;
    int finalLine = 0;

    struct ThreadInfo *threadInfo;
    std::vector<ThreadInfo*> threadsParam;
    for (int i = 0; i < numberOfThreads; i++) {
      threadInfo = new ThreadInfo{&pThreadsProductResult, 0, 0};
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
      pthread_create(&threads[i], nullptr, runProduct, (void*)threadsParam[i]);
    }
    
    for(int i = 0; i < numberOfThreads; i++) {
      pthread_join(threads[i], nullptr);
    }
    
    std::chrono::duration<double> elapsed_seconds = endTimeProduct  - initialTime;
    std::cout << "Produto Matricial PThreads: " << elapsed_seconds.count() << std::endl;
    elapsed_seconds = endTimePosicional  - inicialTimePosicional ;
    std::cout << "Produto Posicional PThreads: " << elapsed_seconds.count() << std::endl;
    pthread_exit(nullptr);
  }

  void StartOpenMP (const int rows, const int columns)
  {
    Matrix ompProductResult(rows, columns, "ProdutoMatricialParalelo");
    Matrix ompPosicionalResult(rows, columns, "ProdutoPosicionalParalelo");
    
    timePoint initialTime = std::chrono::system_clock::now();
    MultiplyMatrixParallelOpenMP(ompProductResult);
    timePoint finalTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = finalTime - initialTime;
    std::cout << "Produto Matricial OMP: " << elapsed_seconds.count() << std::endl;
    
    initialTime = std::chrono::system_clock::now();
    MultiplyMatrixByPositionParallelOpenMP(ompPosicionalResult);
    finalTime = std::chrono::system_clock::now();
    elapsed_seconds = finalTime - initialTime;
    std::cout << "Produto Posicional OMP: " << elapsed_seconds.count() << std::endl;
  }
}

//-------------------------------------------------

int main()
{
  const int rowsFromM1 = m1.GetRows();
  const int columnsFromM2 = m2.GetColumns();
  
  const bool canMultiply = rowsFromM1 == columnsFromM2;
  
  if (canMultiply) {
    m1.FillMatrixWithRandomNumbers();
    m2.FillMatrixWithRandomNumbers();
    
    const int rowsFromM1 = m1.GetRows();
    const int columnsFromM2 = m2.GetColumns();
    
    StartSequencial(rowsFromM1, columnsFromM2);
    
    StartPThreads(rowsFromM1, columnsFromM2);
    
    StartOpenMP(rowsFromM1, columnsFromM2);
  }
  else {
    std::cout << "Numero de colunas da m1 deve ser igual ao numero de linhas da m2." << std::endl;
  }
  
  return 0;
}

