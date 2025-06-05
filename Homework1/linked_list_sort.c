#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

// Split the linked list into two parts
void splitList(Node *head, Node **firstHalf, Node **secondHalf)
{
    asm volatile(
        "sd %[head], 0(%[firstHalf])    \n\t"   //  *firstHalf = head;
        "mv s1, %[head]            \n\t"   // Node *slow = head;
        "lw t0, 8(%[head])              \n\t"   // Node *fast = head->next;
        "mv s2, t0                 \n\t" 

        // while (fast && fast->next){
        //     slow = slow->next;
        //     fast = fast->next->next;
        // }
        "while:     \n\t"    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<WTF
        "beqz s2, end     \n\t"
        // 以下段落
        // "lw t5, 8(s2)          \n\t"
        // "beqz t5, end     \n\t"
        // "ld s1, 8(s1) \n\t"
        // "ld s2, 8(s2) \n\t"
        // "ld s2, 8(s2) \n\t"
        // 相當於
        "ld t0, 8(s2)               \n\t"  // t0 = fast->next
        "beqz t0, end               \n\t"  // if (fast->next == NULL) 跳出迴圈
        "ld s1, 8(s1)          \n\t"  // slow = slow->next
        "ld s2, 8(t0)               \n\t"  // fast = fast->next->next
        // \\ //
        "j while        \n\t"    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<WTF
        "end:                \n\t"

        "lw t2, 8(s1)      \n\t"   // *secondHalf = slow->next;
        "sd t2, 0(%[secondHalf])\n\t"
        "sd zero, 8(s1)    \n\t"   // slow->next = NULL;   // cant sw因為只會存4 word。sw存入double word共64bit為8 word。

        :   [firstHalf] "+r"(firstHalf), [secondHalf] "+r"(secondHalf)
        :   [head]"r"(head)
        :   "memory", "s1", "s2"
        // s1: slow
        // s2: fast    
    );
}

// Merge two sorted linked lists
Node *mergeSortedLists(Node *a, Node *b)
{
    Node *result = NULL;
    Node *tail = NULL;

    // ===========================================
    // if (a->data < b->data) {
    //     result = a;
    //     tail = a;
    //     a = a->next;
    // } else {
    //     result = b;
    //     tail = b;
    //     b = b->next;
    // }

    asm volatile(
        "lw t3, 0(%[a])         \n\t"       // a->data
        "lw t4, 0(%[b])         \n\t"       // b->data
        "bge t3, t4, elsebge0   \n\t"       // if
        "mv %[result], %[a]     \n\t"       // result = a;
        "mv %[tail], %[a]       \n\t"       // tail = a;
        "ld %[a], 8(%[a])       \n\t"       // a = a->next;
        "j end0                 \n\t"
        "elsebge0:              \n\t"       // else
        "mv %[result], %[b]     \n\t"       // result = b;
        "mv %[tail], %[b]       \n\t"       // tail = b;
        "ld %[b], 8(%[b])       \n\t"       // b = b->next;
        "end0:                  \n\t"


        // while (a && b) {
        //     if (a->data < b->data) {
        //         tail->next = a;
        //         tail = a;
        //         a = a->next;
        //     } else {
        //         tail->next = b;
        //         tail = b;
        //         b = b->next;
        //     }
        // }
        "whileM:                \n\t"
        "beqz %[a], endwhile    \n\t"
        "beqz %[b], endwhile    \n\t"

        "lw t3, 0(%[a])         \n\t"       // a->data
        "lw t4, 0(%[b])         \n\t"       // b->data
        "bge t3, t4, elsebge    \n\t"       // if
        "sd %[a], 8(%[tail])    \n\t"       // tail->next = a;
        "mv %[tail], %[a]       \n\t"       // tail = a;
        "ld %[a], 8(%[a])       \n\t"       // a = a->next;
        "j goback               \n\t"
        "elsebge:               \n\t"       // else
        "sd %[b], 8(%[tail])    \n\t"       // tail->next = b;
        "mv %[tail], %[b]       \n\t"       // tail = b;
        "ld %[b], 8(%[b])       \n\t"       // b = b->next;
        "goback:                \n\t"

        "j whileM               \n\t"
        "endwhile:              \n\t"

        // if (a) {
        //     tail->next = a;
        // }
        // if (b) {
        //     tail->next = b;
        // }
        "beqz %[a], ifB         \n\t"
        "sd %[a], 8(%[tail])    \n\t"
        "ifB:                   \n\t"
        "beqz %[b], endif         \n\t"
        "sd %[b], 8(%[tail])    \n\t"
        "endif:                  \n\t"
        :   [result]"+r"(result), [tail]"+r"(tail), [a]"+r"(a), [b]"+r"(b)
        :   
        :   "memory"
    );
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
