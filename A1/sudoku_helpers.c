#include <stdio.h>

int is_vaild(int *num, int l) {
     int i, j;
     for (i = 0; i < l; i++) {
       if (num[i] > 0 && num[i] < l + 1) {
         for (j = (i + 1); j < l; j++) {
           if (num[i] == num[j]) {
             return 1;
           }
         }
       } else {
         return 1;
       }
     }
     return 0;
  }


/* Each of the n elements of array elements, is the address of an
 * array of n integers.
 * Return 0 if every integer is between 1 and n^2 and all
 * n^2 integers are unique, otherwise return 1.
 */

int check_group(int **elements, int n) {
    // TODO: replace this return statement with a real function body
    int i, j, count;
    int n_sq = n * n;
    int arr[n_sq];

    count = 0;

    for (i = 0; i < n; i++) {
      for (j = 0; j < n; j++) {
        arr[count] = elements[i][j];
        count ++;
      }
    }

    if (is_vaild(arr, n_sq)) {
      return 1;
    }
    return 0;
}

/* puzzle is a 9x9 sudoku, represented as a 1D array of 9 pointers
 * each of which points to a 1D array of 9 integers.
 * Return 0 if puzzle is a valid sudoku and 1 otherwise. You must
 * only use check_group to determine this by calling it on
 * each row, each column and each of the 9 inner 3x3 squares
 */
int check_regular_sudoku(int **puzzle) {

    // TODO: replace this return statement with a real function body
    int i, j;
    int *temp[3];

    // row
    for (i = 0; i < 9; i++) {
      temp[0] = puzzle[i];
      temp[1] = puzzle[i] + 3;
      temp[2] = puzzle[i] + 6;
      if (check_group(temp, 3)) {
        return 1;
      }
    }

    // colunm
    for (i = 0; i < 9; i++) {
      int a[3] = {puzzle[0][i], puzzle[1][i], puzzle[2][i]};
      int b[3] = {puzzle[3][i], puzzle[4][i], puzzle[5][i]};
	    int c[3] = {puzzle[6][i], puzzle[7][i], puzzle[8][i]};
      temp[0] = a;
      temp[1] = b;
      temp[2] = c;
      if (check_group(temp, 3)) {
        return 1;
      }
    }

    // 3x3 squares
    for (i = 0; i < 9; i = i + 3) {
      for (j = 0; j < 9; j = j + 3) {
        temp[0] = puzzle[j] + i;
        temp[1] = puzzle[j + 1] + i;
        temp[2] = puzzle[j + 2] + i;
        if (check_group(temp, 3)) {
          return 1;
        }
      }
    }
    return 0;
}
