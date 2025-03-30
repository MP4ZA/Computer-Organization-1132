#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

// Split the linked list into two parts
void splitList(Node *head, Node **firstHalf, Node **secondHalf)
{
    *firstHalf = head;
    *secondHalf = head;
    Node *slow = head;
    Node *fast = head->next;
    while (fast && fast->next)
    {
        slow = slow->next;
        fast = fast->next->next;
    }
    *secondHalf = slow->next;
    slow->next = NULL;
    // asm volatile(
    //     /*
    //     Block A (splitList), which splits the linked list into two halves
    //     */
    //     "");
}

// Merge two sorted linked lists
Node *mergeSortedLists(Node *a, Node *b)
{
    Node *result = NULL;
    Node *tail = NULL;

    if (a->data < b->data) {
        result = a;
        tail = a; // tail 指向a，因為 result 指向 a
        a = a->next; // 讓a向後移動
    } else {
        result = b;
        tail = b; // tail 指向b，因為 result 指向 b
        b = b->next; // 讓b向後移動
    }

    // 合併剩餘的節點
    while (a && b) {
        if (a->data < b->data) {
            tail->next = a;
            tail = a;  // 更新tail為a
            a = a->next; // 讓a向後移動
        } else {
            tail->next = b;
            tail = b;  // 更新tail為b
            b = b->next; // 讓b向後移動
        }
    }

    // 如果a還有剩餘節點
    if (a) {
        tail->next = a;
    }

    // 如果b還有剩餘節點
    if (b) {
        tail->next = b;
    }
    // asm volatile(
    //     /*
    //     Block B (mergeSortedList), which merges two sorted lists into one
    //     */
    //     "");

    return result;
}

// Merge Sort function for linked list
Node *mergeSort(Node *head)
{
    if (!head || !head->next)
        return head; // Return directly if there is only one node

    Node *firstHalf, *secondHalf;
    splitList(head, &firstHalf,&secondHalf); // Split the list into two sublists

    firstHalf = mergeSort(firstHalf);   // Recursively sort the left half
    secondHalf = mergeSort(secondHalf); // Recursively sort the right half

    return mergeSortedLists(firstHalf, secondHalf); // Merge the sorted sublists
}

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
    int list_size;
    fscanf(input, "%d", &list_size);
    Node *head = (list_size > 0) ? (Node *)malloc(sizeof(Node)) : NULL;
    Node *cur = head;
    for (int i = 0; i < list_size; i++) {
        fscanf(input, "%d", &(cur->data));
        if (i + 1 < list_size)
            cur->next = (Node *)malloc(sizeof(Node));
        cur = cur->next;
    }
    fclose(input);

    // Linked list sort
    head = mergeSort(head);

    cur = head;
    while (cur) {
        printf("%d ", cur->data);
        cur = cur -> next;
        // asm volatile(
        //     /*
        //     Block C (Move to the next node), which updates the pointer to
        //     traverse the linked list
        //     */
        //     "");
    }
    printf("\n");
    return 0;
}
