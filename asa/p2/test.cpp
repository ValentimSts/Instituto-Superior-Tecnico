#include <iostream>


int main() {
    int** test = (int**) malloc(sizeof(int*) * 5);
    int val = 0;

    for (int i = 0; i < 5; i++) {
        test[i] = &val;
    }

    for (int i = 0; i < 5; i++) {
        printf("%d\n", *test[i]);
    }

    return 0;
}