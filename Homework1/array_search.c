#include <stdio.h>

int arraySearch(int *p_a, int arr_size, int target)
{
    int result = -1;

    asm volatile(
        "addi t5, %[size], 0    \n\t"
        "addi s1, %[tar], 0     \n\t"
        "lw s2, 0(%[pa])        \n\t"
        "li t6, -1              \n\t"
        "li t1, 0               \n\t"
        "li t4, -1              \n\t"
        "loop:                  \n\t"
        "addi t6, t6, 1         \n\t"
        "lw s2, 0(%[pa])        \n\t"
        "addi %[pa], %[pa], 4   \n\t"
        "beq s1, s2, done       \n\t"
        "blt t6, t5, loop       \n\t"
        "j end                  \n\t"
        "done:                  \n\t"
        "mv %[ans], t6          \n\t"
        "end:                   \n\t"
        : [ans] "+r" (result)   
        : [size] "r"(arr_size), [tar]  "r"(target),[pa] "r"(p_a)
        : "t5", "s1", "s2", "t1", "t2", "t6", "a0", "memory"
    );
    

    return result;
}

// Main function to test the implementation
int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        fprintf(stderr, "Error opening file: %s\n", argv[1]);
        return 1;
    }
    int arr_size;
    fscanf(input, "%d", &arr_size);
    int arr[arr_size];

    // Read integers from input file into the array
    for (int i = 0; i < arr_size; i++) {
        int data;
        fscanf(input, "%d", &data);
        arr[i] = data;
    }
    int target;
    fscanf(input, "%d", &target);
    fclose(input);

    int *p_a = &arr[0];

    int index = arraySearch(p_a, arr_size, target);

    // Print the result
    printf("%d ", index);
    printf("\n");

    return 0;
}
