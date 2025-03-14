#ifndef MATHS_MATRIX_H

template <unsigned short r, unsigned short c>
struct Matrix {
    Matrix(){}
    Matrix(std::initializer_list<float> values){
        unsigned i = 0;
        for(float value : values) {
            data[i / r][i % r] = value;
            i++;
        }
    }

    float data[r][c]{};

    Matrix operator* (const float& f){
        Matrix<r, c> result = Matrix<r, c>();
        for(unsigned row = 0; row < r; row++)
            for(unsigned col = 0; col < c; col++)
                result.data[row][col] = data[row][col] * f;
        return result;
    }

    Matrix operator* (const Matrix& input) {
        Matrix<r, c> result = Matrix<r, c>();
        for(unsigned row1 = 0; row1 < r; row1++)
            for(unsigned col = 0; col < c; col++)
                for(unsigned row2 = 0; row2 < r; row2++) // for now matrices have to match
                    result.data[row1][col] += data[row1][row2] * input.data[row2][col];
        return result;
    }

    VectorXF<r> operator* (const VectorXF<r>& input){
        VectorXF<r> result = VectorXF<r>();
        for(unsigned row = 0; row < r; row++)
            for(unsigned col = 0; col < c; col++)
                result[row] += data[row][col] * input.data[col];
        return result;
    }

    static Matrix translation(const VectorXF<r - 1>& input){
        Matrix<r, c> result = Matrix<r, c>();
        for(unsigned row = 0; row < r; row++)
            for(unsigned col = 0; col < c; col++)
                result.data[row][col] = (row == col)? 1.0F : (col != c - 1)? 0.0F : input.data[r];
        return result;
    }

    static Matrix scale(const VectorXF<r - 1>& input){
        Matrix<r, c> result = Matrix<r, c>();
        for(unsigned row = 0; row < r; row++)
            for(unsigned col = 0; col < c; col++)
                result.data[row][col] = (row == r - 1)? 1.0 : (row != col)? 0.0F : input.data[r]; 
        return result;
    }

    // TODO: Include rotation

    std::string toString() const {
        std::string matStr = "{\n";
        for(unsigned row = 0; row < r; row++)
            for(unsigned col = 0; col < c; col++){
                if(col == 0) matStr += "{ ";
                matStr += std::to_string(data[row][col]) + ", ";
                if(col == c - 1) matStr += " }\n";
            }
        return matStr + "\n}";
    }

    bool isIdentity(){
        if(c != r) return false;
        for(unsigned i = 0; i < c; i++) if(data[i][i] != 1) return false;
        return true;
    }
};

typedef Matrix<2, 2> Mat2x2;
typedef const Mat2x2* const mat2x2_cptr_t;
typedef Matrix<3, 3> Mat3x3;
typedef const Mat3x3* const mat3x3_cptr_t;
typedef Matrix<4, 4> Mat4x4;
typedef const Mat4x4* const mat4x4_cptr_t;

#define MAT_3x3_IDENTITY Mat3x3({ 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f })
#define MAT_4x4_IDENTITY Mat4x4({ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f })
// #define MAT_4x4_TEST Mat4x4({{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f }})

#define MATHS_MATRIX_H
#endif