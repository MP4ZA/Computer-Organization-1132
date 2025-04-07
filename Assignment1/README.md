# Assignment 1

In this assignment, the goal is to implement inline assembly in a C program to ensure the program functions correctly and performs as intended. In the C program, there are multiple asm blocks where RISC-V assembly code needs to be filled in.

## Array Sort
This program aims to implement a bubble sort algorithm, and the asm block specifically handles the swapping of elements.
<!-- ```c
``` -->

## Array Search
This program searches for a specific value in an array and returns -1 if not found. The assembly section is responsible for looping through the entire array and performing the search.
<!-- ```c
``` -->

## Linked List Sort
This program is intended to perform merge sort on a linked list. Traversing the linked list involves some pointer manipulations. 

The following is the corresponding C code that achieves the same goal as writing assembly.

#### splitList
In the `splitlist` function, the objective is to split the linked list into two approximately equal segments. To achieve this, we use the `fast` and `slow` pointers. As the `fast` pointer moves to the end of the list, the `slow` pointer will be positioned at the middle.

```c
*firstHalf = head;
Node *slow = head;
Node *fast = head->next;
while (fast && fast->next)
{
    slow = slow->next;
    fast = fast->next->next;
}

*secondHalf = slow->next;
slow->next = NULL;
```

#### mergeSortedLists
The `mergeSortedLists` function merges two sorted linked lists by repeatedly selecting the node with the smaller value and appending it to the result list.
```c
if (a->data < b->data) {
    result = a;
    tail = a;
    a = a->next;
} else {
    result = b;
    tail = b;
    b = b->next;
}

while (a && b) {
    if (a->data < b->data) {
        tail->next = a;
        tail = a;
        a = a->next;
    } else {
        tail->next = b;
        tail = b;
        b = b->next;
    }
}

if (a) {
    tail->next = a;
}
if (b) {
    tail->next = b;
}
```

#### main
In `main`, it simply moves the pointer to the next node:
```c
cur = cur -> next;
```

Note that in C, accessing the next node is very simple using the `->` operator. However, in RISC-V assembly, accessing the next node's address requires manually calculating the memory offset.

To determine the offset, the size of the variables must be known in advance. This information is crucial for calculating memory addresses, as shown below:
```c
printf("%d\n", sizeof(Node));
printf("%d\n", sizeof(int));
printf("%d\n", sizeof(Node*));
```
The output on the machine is:
```
16
4
8
```

In a 64-bit system, a pointer is represented as 8 Bytes. Due to memory alignment, even though `int` is 4 bytes and `Node*` is 8 bytes, the total size of struct `Node` is 16 bytes instead of 12 bytes.
After printing the `sizeof` values for the structure and its members, the address offset of the `next` member variable in the structure can be determined.
Since the structure needs to be aligned to an 8-byte boundary, to access the second member (`next`), the address of the structure needs to be offset by `8` bytes to access `next`.
Thus, the offset in the RISC-V instruction would look like:
Thus, the RISC-V instruction would look like:
```asm
ld %[cur], 8(%[cur])
```