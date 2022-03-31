#include "Matrix/Matrix.h"

#include <iostream>
#include <thread>

#include <pthread.h>
#include <stdlib.h>


//-------------------------------------------------

namespace {
  void MultiplyMatrix(const Matrix& m1, const Matrix& m2, Matrix& result)
  {
    for (int i = 0; i < m1.GetRows(); i++) {
      for (int j = 0; j < m2.GetColumns(); j++) {
        for (int k = 0; k < m1.GetRows(); k++) {
          result.At(i, j) = result.At(i, j) + (m1.At(i, k) * m2.At(k, j));
        }
      }
    }
  }
  
  //-------------------------------------------------

  void MultiplyMatrixByPosition(const Matrix& m1, const Matrix& m2, Matrix& result)
  {
    for (int i = 0; i < m1.GetRows(); i++) {
      for (int j = 0; j < m2.GetColumns(); j++) {
        result.At(i, j) = m1.At(i, j) * m2.At(i, j);
      }
    }
  }
}

//-------------------------------------------------

int main()
{
  Matrix m1("M1");
  m1.CreateMatrix(10, 10);
  m1.FillMatrixWithRandomNumbers();
  
  Matrix m2("M2");
  m2.CreateMatrix(10, 10);
  m2.FillMatrixWithRandomNumbers();
  
  Matrix result1("ProdutoMatricial");
  Matrix result2("ProdutoPosicional");
  
  const bool canMultiply = m1.GetColumns() == m2.GetRows();
  if (canMultiply){
    unsigned int nummberOfThreads = std::thread::hardware_concurrency();
    pthread_t threads[nummberOfThreads];
    result1.CreateMatrix(m1.GetRows(), m2.GetColumns());
    MultiplyMatrix(m1, m2, result1);
    
    result2.CreateMatrix(m1.GetRows(), m2.GetColumns());
    MultiplyMatrixByPosition(m1, m2, result2);
  }
  else {
    std::cout << "Numero de colunas da m1 deve ser igual ao numero de linhas da m2." << std::endl;
    return 0;
  }
  
  if (m1.GetRows() <= 10){
    m1.PrintMatrix();
    m2.PrintMatrix();
    result1.PrintMatrix();
    result2.PrintMatrix();
  }
}


