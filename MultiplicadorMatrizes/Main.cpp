#include "Matrix/Matrix.h"

#include <iostream>
#include <thread>

#include <pthread.h>
#include <stdlib.h>


//-------------------------------------------------

static int totalLines = 0;

struct ThreadLines {
  Matrix m1;
  Matrix m2;
  Matrix *result1;
  Matrix *result2;
  int initialLine;
  int finalLine;
};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

namespace {
  void MultiplyMatrix(const Matrix& m1, const Matrix& m2, Matrix& result, int initialLine, int finalLine)
  {
    int start = finalLine == 0 ? 0 : finalLine;
    int end = initialLine == 0 ? m1.GetRows() : initialLine;
    const int columns = m2.GetColumns();
    
    //int initialLine = totalLines
    
    for (int i = start; i < end; i++) {
      for (int j = 0; j < columns; j++) {
        for (int k = 0; k < end; k++) {
          result.At(i, j) = result.At(i, j) + (m1.At(i, k) * m2.At(k, j));
        }
      }
    }
  }
  
  //-------------------------------------------------

  void MultiplyMatrixByPosition(const Matrix& m1, const Matrix& m2, Matrix& result, int line)
  {
    int rows = line == 0 ? m1.GetRows() : line;
    const int columns = m2.GetColumns();
    
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < columns; j++) {
        result.At(i, j) = m1.At(i, j) * m2.At(i, j);
      }
    }
  }
  
  //-------------------------------------------------

  void* run(void* arg)
  {
    struct ThreadLines *info = (ThreadLines*)arg;

    
    MultiplyMatrix(info->m1, info->m2, *info->result1, info->initialLine, info->finalLine);
    pthread_mutex_lock(&mutex);
    std::cout << "initial: " << info->initialLine << "final: " << info->finalLine << std::endl;
    pthread_mutex_unlock(&mutex);
    std::cout << "Thread finished.\n" << std::endl;
    
    return 0;
  }
  
}

//-------------------------------------------------

int main()
{
  Matrix m1(4, 4, "M1");
  Matrix m2(4, 4, "M2");

  const int rowsFromM1 = m1.GetRows();
  const int columnsFromM2 = m2.GetColumns();
  
  const bool canMultiply = rowsFromM1 == columnsFromM2;
  
  if (canMultiply) {
    m1.FillMatrixWithRandomNumbers();
    m2.FillMatrixWithRandomNumbers();
    
    Matrix result1(rowsFromM1, columnsFromM2, "ProdutoMatricial");
    Matrix result2(rowsFromM1, columnsFromM2, "ProdutoPosicional");
    Matrix resultParalelo1(rowsFromM1, columnsFromM2, "ProdutoMatricialParalelo");
    Matrix resultParalelo2(rowsFromM1, columnsFromM2, "ProdutoPosicionalParalelo");
    
    totalLines = rowsFromM1;
    
    const unsigned int numberOfThreads = std::thread::hardware_concurrency();
    pthread_t threads[numberOfThreads];
  
    struct ThreadLines *threadLines;
    
    int resto = totalLines % numberOfThreads;
    int linesPerThread = totalLines / numberOfThreads;
    
    for(int i = 0; i < numberOfThreads; i++) {
      threadLines = new ThreadLines{m1, m2, &resultParalelo1, &resultParalelo2, 0, 0};
      threadLines->initialLine = i * linesPerThread;
      threadLines->finalLine = threadLines->initialLine + linesPerThread;
      
      if(resto != 0) {
        threadLines->finalLine++;
        resto--;
      }
      
      pthread_create(&threads[i], nullptr, run, (void*)threadLines);
    }
    
    for(int i = 0; i < numberOfThreads; i++) {
      pthread_join(threads[i], nullptr);
    }
    
    //MultiplyMatrix(m1, m2, result1, 0, 0);
    //MultiplyMatrixByPosition(m1, m2, result2, 0);
    
    if (m1.GetRows() <= 10){
      m1.PrintMatrix();
      m2.PrintMatrix();
      resultParalelo1.PrintMatrix();
      resultParalelo2.PrintMatrix();
    }
  }
  else {
    std::cout << "Numero de colunas da m1 deve ser igual ao numero de linhas da m2." << std::endl;
  }
  
  pthread_exit(nullptr);
  return 0;
}

