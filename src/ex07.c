#include <stdlib.h>
#include <stdio.h>

typedef struct node_t {
    int data;
    struct node_t *next;
} node_t;

typedef struct {
    node_t *head;
    node_t *tail;
    unsigned int size;
} llist_t;

#define assert(p) if (p == NULL) return -1

int llist_init(llist_t* const list)
{
    assert(list);

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;

    return 0;
}

int llist_insert(llist_t* const list, const int data)
{
    assert(list);
    printf("list insert\n");

    node_t *new = (node_t*)malloc(sizeof(node_t));
    if (NULL == new) {
        fprintf(stderr, "error alocating new node on llist_insert()\n");
        return -1;
    }

    new->data = data;
    if (list->size == 0) {
        list->head = list->tail = new;
        list->size++;
        return 0;
    }

    new->next = list->head;
    list->head = new;
    list->size++;

    return 0;
}

int llist_push(llist_t* const list, const int data)
{
    assert(list);

    node_t *new = (node_t*)malloc(sizeof(node_t));
    if (NULL == new) {
        fprintf(stderr, "error alocating new node on llist_push()\n");
        return -1;
    }

    new->next = NULL;
    new->data = data;

    if (list->size == 0) {
        list->size++;
        list->head = list->tail = new;
        return 0;
    }

    list->tail->next = new;
    list->tail = new;
    list->size++;

    return 0;
}

int llist_insert_ord(llist_t* const list, int data)
{
    assert(list);
    node_t *curr, *prev;

    node_t *new = (node_t*)malloc(sizeof(node_t));
    if (NULL == new) {
        fprintf(stderr, "error alocating new node on llist_insert_ord()\n");
        return -1;
    }
    new->data = data;
    
    if (NULL == list->head || list->head->data > data) { // must insert in the beginning of the list
        if (NULL == list->tail) list->tail = new; // special case, to handle the 1st insertion
        new->next = list->head;
        list->head = new;
        list->size = 1;
        return 0;
    }

    curr = list->head;

    printf("curr :: %p\n", (void*)curr);
    while (curr != NULL && curr->data < data) {
        prev = curr;
        curr = curr->next;
    }

    prev->next = new;
    new->data = data;
    new->next = curr;

    if (curr == NULL) { // reached the end of the list
        list->tail = new;
    }
 
    list->size++;
    
    return 0;
}

void llist_print(llist_t list)
{
    node_t *aux = list.head;
    printf("size of the list :: %d\n", list.size);
    printf("address of the head of the list :: %p\n", (void*)list.head);
    printf("address of the tail of the list :: %p\n", (void*)list.tail);

    while (NULL != aux) {
        printf("[%p] :: %d and -> %p\n", (void*)aux, aux->data, (void*)aux->next);
        aux = aux->next;
    }
    return;
}

int ex07(void)
{
    llist_t list;
    llist_init(&list);

    llist_insert_ord(&list, 3);
    llist_insert_ord(&list, 7);
    llist_insert_ord(&list, 4);
    llist_insert_ord(&list, 5);
    llist_insert_ord(&list, 2);
    llist_insert_ord(&list, 1);
    llist_insert_ord(&list, 8);
    llist_insert_ord(&list, 6);
    llist_print(list);

    return 0;
}