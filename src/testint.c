#include <stdio.h>


void printInt(int arr[2]) {
    printf( "%d\n", 256*arr[1] + arr[0] );
}

int main( ) {
    int arr[2] = {0, 0};
    printInt(arr);

    int arr1[2] = {255, 0};
    printInt(arr1);

    int arr2[2] = {0, 255};
    printInt(arr2);

    int arr3[2] = {255, 255};
    printInt(arr3);

    return 0;
}
