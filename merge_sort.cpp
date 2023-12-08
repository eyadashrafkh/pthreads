// mergeSort Algorithm


#include <iostream>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <fstream>
#include <thread>

using namespace std;

// Struct to pass arguments to the threads
struct ThreadArgs {
    vector<int>& arr;
    int left;
    int right;
};

// Function to merge two sorted subarrays
void merge(vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<int> L(arr.begin() + left, arr.begin() + left + n1);
    vector<int> R(arr.begin() + mid + 1, arr.begin() + mid + 1 + n2);
    
    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    }

    while (i < n1) {
        arr[k++] = L[i++];
    }

    while (j < n2) {
        arr[k++] = R[j++];
    }
}


// Function to perform merge sort on a subarray
void* mergeSort(void* arg) {
    ThreadArgs* args = static_cast<ThreadArgs*>(arg);
    int left = args->left;
    int right = args->right;

    if (left < right) {
        int mid = left + (right - left) / 2;

        // Create threads for left and right halves
        pthread_t leftThread, rightThread;

        ThreadArgs leftArgs{args->arr, left, mid};
        ThreadArgs rightArgs{args->arr, mid + 1, right};

        pthread_create(&leftThread, nullptr, mergeSort, static_cast<void*>(&leftArgs));
        pthread_create(&rightThread, nullptr, mergeSort, static_cast<void*>(&rightArgs));

        // Wait for the threads to finish
        pthread_join(leftThread, nullptr);
        pthread_join(rightThread, nullptr);

        // Merge the sorted halves
        merge(args->arr, left, mid, right);
    }

    pthread_exit(nullptr);
}


// Function to perform parallel merge sort
void parallelMergeSort(vector<int>& arr) {
    int left = 0;
    int right = arr.size() - 1;

    ThreadArgs args{arr, left, right};

    pthread_t initialThread;
    pthread_create(&initialThread, nullptr, mergeSort, static_cast<void*>(&args));

    // Wait for the initial thread to finish
    pthread_join(initialThread, nullptr);

}

int main() {
    // Input filename
    string filename;
    cout << "Enter the filename: ";
    cin >> filename;

    ifstream inputFile1(filename);

    int num_elements;

    if (!inputFile1.is_open() ) {
        cerr << "Error opening file." << endl;
        return 1;
    }
    inputFile1 >> num_elements;

    vector<int> arr(num_elements);

    for (int i=0;i<num_elements;i++){
        inputFile1 >> arr[i];
    }

    // Perform parallel merge sort
    parallelMergeSort(arr);


    // Print the sorted array
    cout << "\nSorted array:\n ";
    for (int num : arr) {
        cout << num << " ";
    }
    cout << endl;

    return 0;
}
