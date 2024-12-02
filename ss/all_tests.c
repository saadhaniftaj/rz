#include <stdio.h>
#include <stdlib.h>

int main() {
    int result = 0;

    printf("\033[0;34m\nRUNNING FORMAT TEST...\033[0m\n");
    result = system("./build/format.out");
    if (result != 0) {
        printf("Format test failed!\n");
        return result;
    }

    printf("\033[0;34m\nRUNNING CREATE TEST...\033[0m\n");
    result = system("./build/create.out");
    if (result != 0) {
        printf("Create test failed!\n");
        return result;
    }

    printf("\033[0;34m\nRUNNING LIST TEST...\033[0m\n");
    result = system("./build/list.out");
    if (result != 0) {
        printf("List test failed!\n");
        return result;
    }

    printf("\033[0;34m\nRUNNING REMOVE TEST...\033[0m\n");
    result = system("./build/remove.out");
    if (result != 0) {
        printf("Remove test failed!\n");
        return result;
    }

    printf("\033[0;34m\nRUNNING WRITE TEST...\033[0m\n");
    result = system("./build/write.out");
    if (result != 0) {
        printf("Write test failed!\n");
        return result;
    }

    printf("\033[0;34m\nRUNNING READ TEST...\033[0m\n");
    result = system("./build/read.out");
    if (result != 0) {
        printf("Read test failed!\n");
        return result;
    }

 
    printf("\n");
    // printf("\nAll tests passed successfully.\n\n");
    return 0;
}
