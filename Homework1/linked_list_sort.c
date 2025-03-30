#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

// Split the linked list into two parts
void splitList(Node *head, Node **firstHalf, Node **secondHalf)
{

    //  *firstHalf = head;
    asm volatile(
        "sd %[head], 0(%[firstHalf])       \n\t"    
        :   [firstHalf] "+r"(firstHalf)
        :   [head]"r"(head)
        :   "memory"
    );

    Node *slow;
    // Node *slow = head;
    Node *fast;
    // Node *fast = head->next;

    asm volatile(
        "mv %[slow], %[head]     \n\t"    // slow = head;
        //////////
        "lw t0, 8(%[head])      \n\t"
        "mv %[fast], t0     \n\t" 

        :   [slow] "+r"(slow), [fast] "+r"(fast)
        :   [head] "r"(head)
        :   "memory"
    );

    // while (fast && fast->next)
    // {
    //     slow = slow->next;
    //     fast = fast->next->next;
    // }
    asm volatile(
        "while:     \n\t"
        "beqz %[fast], end     \n\t"

        // 以下段落
        // "lw t5, 8(%[fast])          \n\t"
        // "beqz t5, end     \n\t"
        // "ld %[slow], 8(%[slow]) \n\t"
        // "ld %[fast], 8(%[fast]) \n\t"
        // "ld %[fast], 8(%[fast]) \n\t"
        // 相當於
        "ld t0, 8(%[fast])               \n\t"  // t0 = fast->next
        "beqz t0, end               \n\t"  // if (fast->next == NULL) 跳出迴圈
        "ld %[slow], 8(%[slow])          \n\t"  // slow = slow->next
        "ld %[fast], 8(t0)               \n\t"  // fast = fast->next->next
        // \\ //


        "j while        \n\t"
        "end:                \n\t"
        :   [slow] "+r"(slow), [fast] "+r"(fast)
        :
        :   "memory"
    );

    // *secondHalf = slow->next;
    // slow->next = NULL;
    asm volatile(
        "lw t2, 8(%[slow])      \n\t"
        "sd t2, 0(%[secondHalf])\n\t"
        //////////
        "sd zero, 8(%[slow])    \n\t"// cant sw因為只會存4 word。sw存入double word共64bit為8 word。
        :   [secondHalf] "+r"(secondHalf)
        :   [slow] "r"(slow)
        :   "memory"
    );
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
    asm volatile(
        /*
        Block B (mergeSortedList), which merges two sorted lists into one
        */
        "");

    return result;
}

// Merge Sort function for linked list
Node *mergeSort(Node *head)
{
    if (!head || !head->next)
        return head; // Return directly if there is only one node

    Node *firstHalf, *secondHalf;
    splitList(head, &firstHalf,
              &secondHalf); // Split the list into two sublists

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

    Node* anss=cur;
    Node* ans2=cur;
    while (cur) {
        printf("%d ", cur->data);
        // printf("%d %p   %p\n", (cur->data),&(cur->data), cur);
        // // printf("%d,  %p\n",anss->data,anss);
        // printf("%p\n",ans2);
        // printf("\n");

        asm volatile(
            // "mv t1, %[cur]          \n\t"

            // "lw t0, 8(%[cur])       \n\t"       // 利用lw將存在cur指標位置的變數取出，offset為8為成員next，即下一個節點的位置
            // // "lw t0, 0(t0)       \n\t"
            // "mv %[anss], t0         \n\t"
            // "mv %[ans2], t1         \n\t"
            // "mv %[cur], t0         \n\t"        // 既然下一個節點的位置存在t0，將其賦給到cur。

            "ld %[cur], 8(%[cur])   \n\t"
            : [cur] "+r" (cur), [anss]"+r"(anss), [ans2]"+r"(ans2)
            : 
            : "memory"
        );
        
    }
    // printf("SIZE: %d %d %d",sizeof(Node),sizeof(Node*),sizeof(int));
    
    printf("\n");
    return 0;
}
