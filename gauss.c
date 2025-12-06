#include "common.h"

matrix_t create_matrix(int width, int height) {
    matrix_t ret;
    ret.width = width;
    ret.height = height;
    ret.data = calloc(width*height,sizeof(float));
    return ret;
}
void destroy_matrix(matrix_t mat) {
    free(mat.data);
}
matrix_t copy_matrix(matrix_t mat) {
    matrix_t ret;
    ret.width = mat.width;
    ret.height = mat.height;
    ret.data = calloc(mat.width*mat.height,sizeof(float));
    memcpy(ret.data, mat.data, mat.width*mat.height*sizeof(float));
    return ret;
}
float get(matrix_t mat, int col, int row) {
    return mat.data[col*mat.height + row];
}
void set(matrix_t mat, int col, int row, float val) {
    mat.data[col*mat.height + row] = val;
}
void print_matrix(matrix_t mat) {
    for(int col = 0; col < mat.width; col++) {
        for(int row = 0; row < mat.height; row++) {
            printf("%f,", get(mat, col, row));
        }
        printf("\n");
    }
    printf("\n");
}
void swap_columns(matrix_t mat, int col1, int col2) {
    for(int row = 0; row < mat.height; row++) {
        float tmp = get(mat, col1, row);
        set(mat, col1, row, get(mat, col2, row));
        set(mat, col2, row, tmp);
    }
}
void swap_rows(matrix_t mat, int row1, int row2) {
    for(int col = 0; col < mat.width; col++) {
        float tmp = get(mat, col, row1);
        set(mat, col, row1, get(mat, col, row2));
        set(mat, col, row2, tmp);
    }
}
void multiply_row_by(matrix_t mat, int row, float factor) {
    for(int col = 0; col < mat.width; col++) {
        float tmp = get(mat, col, row);
        set(mat, col, row, tmp*factor);
    }
}
void add_multiple_of_row(matrix_t mat, int row_dst, int row_src, float factor) {
    if(factor == 0.0f) return;
    for(int col = 0; col < mat.width; col++) {
        float tmp = get(mat, col, row_dst);
        set(mat, col, row_dst, tmp + get(mat, col, row_src)*factor);
    }
}
matrix_t gaussian(matrix_t mat_input, matrix_t v) {
    assert(v.width == 1);
    assert((mat_input.width == mat_input.height) && (mat_input.height == v.height));
    matrix_t mat = copy_matrix(mat_input);
    matrix_t res = copy_matrix(v);

    for(int col = 0; col < mat.width; col++) {
        if(get(mat, col, col) == 0.0f) {
            for(int row = col; row < mat.height; row++) {
                if(get(mat, col, row) != 0.0f) {
                    swap_rows(mat, row, col);
                    swap_rows(res, row, col);
                    break;
                }
            }
            die("matrix inadequate!\n");
        }
        float divide_factor = 1/get(mat, col, col);
        multiply_row_by(mat, col, divide_factor);
        multiply_row_by(res, col, divide_factor);
        for(int row = 0; row < mat.height; row++) {
            if(row != col) {
            float factor = get(mat, col, row);
            add_multiple_of_row(mat, row, col, -factor);
            add_multiple_of_row(res, row, col, -factor);
            }
        }
    }


    destroy_matrix(mat);
    return res;
}
