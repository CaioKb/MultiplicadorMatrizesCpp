#include "Matrix.h"

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//-------------------------------------------------

Matrix::Matrix(int _rows, int _columns, std::string _nome) :
  data({ }),
  rows(_rows),
  columns(_columns),
  nome(_nome)
{
  CreateMatrix(_rows, _columns);
}

//-------------------------------------------------

void Matrix::CreateMatrix(int rows, int columns)
{
  data.assign(rows, std::vector<int>(columns));
  for(int i = 0; i < this->GetRows(); i++) {
    for (int j = 0; j < this->GetColumns(); j++){
      this->At(i, j) = 0;
    }
  }
}

//-------------------------------------------------

int Matrix::At(const int& row, const int& column) const
{
  return data.at(row).at(column);
}

//-------------------------------------------------

int& Matrix::At(const int& row, const int& column)
{
  return data.at(row).at(column);
}

//-------------------------------------------------

void Matrix::FillMatrixWithRandomNumbers()
{
  sleep(1);
  srand((int)time(NULL));
  for(int i = 0; i < this->GetRows(); i++) {
    for (int j = 0; j < this->GetColumns(); j++){
      this->At(i, j) = rand() % 10;
    }
  }
}

//-------------------------------------------------

void Matrix::PrintMatrix() const
{
  std::string result = this->GetNome() + ":";
  result.append("\n[");
  for(int i = 0; i < this->GetRows(); i++) {
    for (int j = 0; j < this->GetColumns(); j++){
      result.append(" " + std::to_string(this->At(i, j)) + " ");
    }
    result.append("\n ");
  }
  result.replace(result.end()-3, result.end(), " ]\n");
  
  std::cout << result << std::endl;
}

//-------------------------------------------------
