#ifndef Matrix_h
#define Matrix_h

#include <vector>
#include <string>

class Matrix {
  public:
    using MatrixVector = std::vector< std::vector<int> >;
    using String = std::string;
  
    Matrix(int rows, int columns, String nome = "");
  
    Matrix() = default;

    ~Matrix() = default;
  
    void CreateMatrix(int rows, int columns);
  
    int At(const int& row, const int& column) const;
  
    int& At(const int& row, const int& column);
  
    void FillMatrixWithRandomNumbers();
    
    void PrintMatrix() const;
    
    int GetRows() const
    {
      return (int)data.size();
    }
    
    int GetColumns() const
    {
      return (int)data[0].size();
    }
    
    String GetNome() const {
      return nome;
    }
  
  private:
    MatrixVector data;
    String nome;
  
    int rows = 0;
    int columns = 0;
};

#endif
