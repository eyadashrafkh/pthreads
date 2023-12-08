#include <iostream>
#include <vector>
#include <pthread.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <fstream>

using namespace std;
using namespace chrono;


// Structure to pass arguments to the thread function
struct ThreadArgs {
    const vector<vector<int>>& matrix_a;
    const vector<vector<int>>& matrix_b;
    vector<vector<int>>& result_matrix;
    int row;
    int col;
};


// Function to create a matrix of given size
vector<vector<int>> createMatrix(size_t rows, size_t cols) {
    return vector<vector<int>>(rows, vector<int>(cols, 0));
}


// Function to multiply two matrices (element by element)
void* compute_element(void* args) {
    ThreadArgs* thread_args = static_cast<ThreadArgs*>(args);
    int value = 0;

    for (size_t k = 0; k < thread_args->matrix_b.size(); ++k) {
        value += thread_args->matrix_a[thread_args->row][k] * thread_args->matrix_b[k][thread_args->col];
    }

    thread_args->result_matrix[thread_args->row][thread_args->col] = value;

    pthread_exit(nullptr);
}


// Function to multiply two matrices (element by element)
void matrix_multiply_parallel_element(const vector<vector<int>>& matrix_a,
                               const vector<vector<int>>& matrix_b,
                               vector<vector<int>>& result_matrix) {
    pthread_t threads[matrix_a.size() * matrix_b[0].size()];
    size_t thread_index = 0;

    for (size_t i = 0; i < matrix_a.size(); ++i) {
        for (size_t j = 0; j < matrix_b[0].size(); ++j) {
            ThreadArgs* args = new ThreadArgs{matrix_a, matrix_b, result_matrix, static_cast<int>(i), static_cast<int>(j)};
            pthread_create(&threads[thread_index++], nullptr, compute_element, static_cast<void*>(args));
        }
    }

    for (size_t i = 0; i < thread_index; ++i) {
        pthread_join(threads[i], nullptr);
    }
}




// Function to multiply two matrices (row by row)
void* compute_row(void* args) {
    ThreadArgs* thread_args = static_cast<ThreadArgs*>(args);

    for (size_t j = 0; j < thread_args->matrix_b[0].size(); ++j) {
        int value = 0;
        for (size_t k = 0; k < thread_args->matrix_b.size(); ++k) {
            value += thread_args->matrix_a[thread_args->row][k] * thread_args->matrix_b[k][j];
        }
        thread_args->result_matrix[thread_args->row][j] = value;
    }

    pthread_exit(nullptr);
}



// Function to multiply two matrices (row by row)
void matrix_multiply_parallel_row(const vector<vector<int>>& matrix_a,
                               const vector<vector<int>>& matrix_b,
                               vector<vector<int>>& result_matrix) {
    pthread_t threads[matrix_a.size()];

    for (size_t i = 0; i < matrix_a.size(); ++i) {
        ThreadArgs* args = new ThreadArgs{matrix_a, matrix_b, result_matrix, static_cast<int>(i)};
        pthread_create(&threads[i], nullptr, compute_row, static_cast<void*>(args));
    }

    for (size_t i = 0; i < matrix_a.size(); ++i) {
        pthread_join(threads[i], nullptr);
    }
}




int main() {

    // Input filename
    string filename;
    cout << "Enter filename: ";
    cin >> filename;

    // Open the file
    ifstream inputFile(filename);
    

    if (!inputFile) {
        cerr << "Error opening the input file." << endl;
        return 1; // Exit with an error code
    }

    size_t rows_a, cols_a, rows_b, cols_b;

    // Read matrix A size and values
    inputFile >> rows_a >> cols_a;
    vector<vector<int>> matrix_a = createMatrix(rows_a, cols_a);

    for (size_t i = 0; i < rows_a; ++i) {
        for (size_t j = 0; j < cols_a; ++j) {
            inputFile >> matrix_a[i][j];
        }
    }


    // Read matrix B size and values
    inputFile >> rows_b >> cols_b;
    vector<vector<int>> matrix_b = createMatrix(rows_b, cols_b);

    for (size_t i = 0; i < rows_b; ++i) {
        for (size_t j = 0; j < cols_b; ++j) {
            inputFile >> matrix_b[i][j];
        }
    }

    inputFile.close();

    // Check if matrices can be multiplied
    if (cols_a != rows_b) {
        cerr << "Error: The number of columns in matrix A must be equal to the number of rows in matrix B for multiplication.\n";
        return 1;
    }

    
    // Measure the time taken for matrix multiplication (element by element)
    // auto start_time = steady_clock::now();
    // vector<vector<int>> result_matrix(rows_a, vector<int>(cols_b, 0));
    // matrix_multiply_parallel_element(matrix_a, matrix_b, result_matrix);
    // auto end_time = steady_clock::now();
    // chrono::duration<double> duration = chrono::duration_cast<chrono::duration<double>>(end_time - start_time);

    // Measure the time taken for matrix multiplication (element by element)
    auto start_time = high_resolution_clock::now();
    vector<vector<int>> result_matrix(rows_a, vector<int>(cols_b, 0));
    matrix_multiply_parallel_element(matrix_a, matrix_b, result_matrix);
    auto end_time = high_resolution_clock::now();
    auto time_taken = duration_cast<duration<double>>(end_time - start_time);

    // Print the result
    cout << "\nResulted matrix (element by element):\n";
    for (const auto& row : result_matrix) {
        for (int element : row) {
            cout << element << " ";
        }
        cout << endl;
    }

    // Print the time taken
    cout << fixed << setprecision(6) << "\nTime taken for matrix multiplication (element by element): " << time_taken.count() << " Seconds." << endl;

    // Measure the time taken for matrix multiplication (row by row)
    auto start_time_2 = high_resolution_clock::now();
    vector<vector<int>> result_matrix_2(rows_a, vector<int>(cols_b, 0));
    matrix_multiply_parallel_row(matrix_a, matrix_b, result_matrix_2);
    auto end_time_2 = high_resolution_clock::now();
    auto time_taken_2 = duration_cast<duration<double>>(end_time_2 - start_time_2);

    // Print the result
    cout << "\nResulted matrix (row by row):\n";
    for (const auto& row : result_matrix_2) {
        for (int element : row) {
            cout << element << " ";
        }
        cout << endl;
    }

    // Print the time taken by multiplication (row by row)
    cout << fixed << setprecision(6) << "\nTime taken for matrix multiplication (row by row): " << time_taken_2.count() << " Seconds." << endl;

    return 0;
}
