#include <iostream>

void RecurrentSearch(int** array_of_arrays, int* size_of_arrays, long long part_of_summ,
                     int iter, long long& summ, int max_iter, int* index_arr, int index_arr_size) {
  if (iter < max_iter) {
    for (int tmp = 0; tmp < size_of_arrays[iter]; ++tmp) {
      if(index_arr[tmp] == 0) {
        index_arr[tmp] = 1;
        RecurrentSearch(array_of_arrays, size_of_arrays, part_of_summ * array_of_arrays[iter][tmp],
                        iter + 1, summ, max_iter, index_arr, index_arr_size);
        index_arr[tmp] = 0;
      }
    }
  } else {
    for (int tmp = 0; tmp < size_of_arrays[iter]; ++tmp) {
      if(index_arr[tmp] == 0) {
        summ += array_of_arrays[iter][tmp] * part_of_summ;
      }
    }
  }
}

int main(int cnt_arr, char* argv[]) {
  int* size_of_arrays = new int[--cnt_arr];
  int index_arr_size = 0;
  for (int ind = 1; ind <= cnt_arr; ++ind) {
    size_of_arrays[ind - 1] = atoi(argv[ind]);
    if (size_of_arrays[ind - 1] > index_arr_size) {
      index_arr_size = size_of_arrays[ind - 1];
    }
  }
  int** array_of_arrays = new int*[cnt_arr];
  for (int tmp = 0; tmp < cnt_arr; ++tmp) {
    array_of_arrays[tmp] = new int[size_of_arrays[tmp]];
  }

  for (int tmp = 0; tmp < cnt_arr; ++tmp) {
    for (int ind = 0; ind < size_of_arrays[tmp]; ++ind) {
      std::cin >> array_of_arrays[tmp][ind];
    }
  }
  long long part_of_summ = 1;
  long long summ = 0;
  int* index_arr = new int[index_arr_size];
  for (int tmp = 0; tmp < index_arr_size; ++tmp) {
    index_arr[tmp] = 0;
  }
  RecurrentSearch(array_of_arrays, size_of_arrays, part_of_summ,
                  0, summ, cnt_arr - 1, index_arr, index_arr_size);
  for (int tmp = 0; tmp < cnt_arr; ++tmp) {
    delete[] array_of_arrays[tmp];
  }
  delete[] array_of_arrays;
  delete[] size_of_arrays;
  delete[] index_arr;
  std::cout << summ;
}
