#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

typedef struct node_t {
    int idx;
    const char *name;
    struct node_t *next;
} node_t;

typedef struct {
    node_t *head;
    node_t *tail;
    unsigned int size;
} llist_t;

#define assert(p) if (p == NULL) return -1

#define READERS_NUM     15
#define NUM_MAX         1024

static pthread_t th_writer;
static pthread_t th_readers[READERS_NUM];
static pthread_mutex_t mtx_llist = PTHREAD_MUTEX_INITIALIZER; // it will cause low performance issues 

static const char* const dict[] = {
    "amor", "lux", "ventus", "terra", "aqua", "ignis", "sol", "luna",
    "veritas", "vita", "rex", "gloria", "natura", "animus", "fortis",
    "pax", "scientia", "sapientia", "tempus", "vis", "caelum", "ars",
    "cor", "deus", "fides", "honos", "iustitia", "mors", "nobis",
    "ordo", "prima", "quam", "rebus", "salus", "ultima", "aequus",
    "aurum", "bene", "bonus", "brevis", "caritas", "clarus", "coniunx",
    "crux", "cura", "deus", "dico", "divinus", "dolor", "dominus",
    "donum", "etiam", "facio", "felix", "fides", "filia", "filius",
    "finis", "fortuna", "frater", "geminus", "gloria", "gratia",
    "honor", "hora", "iam", "idem", "invenio", "iustus", "laetus",
    "leo", "lex", "libertas", "lingua", "longus", "lux", "magnus",
    "materia", "meus", "mors", "mundus", "natus", "nomen", "omnis",
    "pater", "patria", "pax", "pectus", "pes", "porta", "princeps",
    "prudentia", "puer", "pulcher", "quaero", "regina", "res", "salus",
    "sanctus", "sanguis", "scio", "senex", "servus", "solus", "somnus",
    "soror", "spes", "spiritus", "statim", "suus", "tempus", "terra",
    "totus", "turba", "ubi", "umbra", "unus", "urbs", "ut", "venio",
    "verbum", "verus", "via", "virtus", "vita", "vox", "vulnus", "vox",
};

int llist_init(llist_t* const list)
{
    assert(list);

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;

    return 0;
}

int llist_insert(llist_t* const list, const int idx, const char* name)
{
    assert(list);

    node_t *new = (node_t*)malloc(sizeof(node_t));
    if (NULL == new) {
        fprintf(stderr, "error alocating new node on llist_insert()\n");
        return -1;
    }

    new->idx = idx;
    new->name = name;

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

int llist_push(llist_t* const list, const int idx, const char* name)
{
    assert(list);

    node_t *new = (node_t*)malloc(sizeof(node_t));
    if (NULL == new) {
        fprintf(stderr, "error alocating new node on llist_push()\n");
        return -1;
    }

    new->idx = idx;
    new->name = name; 
    new->next = NULL;

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

int llist_insert_ord(llist_t* const list, int idx, const char* name)
{
    assert(list);
    node_t *curr, *prev;

    node_t *new = (node_t*)malloc(sizeof(node_t));
    if (NULL == new) {
        fprintf(stderr, "error alocating new node on llist_insert_ord()\n");
        return -1;
    }

    new->idx = idx;
    new->name = name;
    
    if (NULL == list->head || list->head->idx > idx) { // must insert in the beginning of the list
        if (NULL == list->tail) list->tail = new; // special case, to handle the 1st insertion
        new->next = list->head;
        list->head = new;
        list->size = 1;
        return 0;
    }

    curr = list->head;
    prev = curr;

    while (curr != NULL && curr->idx < idx) {
        prev = curr;
        curr = curr->next;
    }

    prev->next = new;
    new->idx = idx;
    new->next = curr;

    if (curr == NULL) { // reached the end of the list
        list->tail = new;
    }
 
    list->size++;
    
    return 0;
}

const char *llist_get_name(const llist_t* const list, const int idx)
{
    if (list == NULL) return NULL;

    node_t *aux = list->head;

    while (aux != NULL) {
        if (aux->idx == idx) {
            return aux->name;
        }
        aux = aux->next;
    }
    return NULL;
}

int llist_print(const llist_t* const list)
{
    assert(list);

    node_t *aux = list->head;
    printf("size of the list :: %d\n", list->size);
    printf("address of the head of the list :: %p\n", (void*)list->head);
    printf("address of the tail of the list :: %p\n", (void*)list->tail);

    while (NULL != aux) {
        printf("[%p] :: %d and -> %p\n", (void*)aux, aux->idx, (void*)aux->next);
        aux = aux->next;
    }
    return 0;
}

static unsigned int scaled_dict_pos(const int idx)
{
    const unsigned int max_pos = sizeof(dict)/sizeof(char*);
    return ((idx*max_pos)/NUM_MAX) % max_pos;
}

static const char* monitor_read(const llist_t* const list, const int idx)
{
    const char* name = NULL;
    pthread_mutex_lock(&mtx_llist);
    name = llist_get_name(list, idx);
    pthread_mutex_unlock(&mtx_llist);
    return name;
}

static void monitor_write(llist_t* list, const int idx, const char* name)
{
    pthread_mutex_lock(&mtx_llist);
    llist_insert_ord(list, idx, name);
    pthread_mutex_unlock(&mtx_llist);
}

static void *reader_task(void* args)
{
    llist_t* list = (llist_t*)args;

    for (unsigned int i=0; i<100; i++) {
        unsigned int r = rand() % NUM_MAX;

        const char *name = monitor_read(list, r);
        if (name == NULL) {
            printf("Index [%d] not found.\n", r);
        } else {
            printf("<%s> found in position %d\n", name, r);
        }
    }

    return NULL;
}

static char* name_create(const char* name, const int idx)
{
    const size_t len = strlen(name)+6;
    char* new_name = (char*)malloc(len);
    if (NULL == new_name) {
        return NULL;
    }
    snprintf(new_name, len, "%s_%04d", name, idx);

    return new_name;
}

static void* writer_task(void* args)
{
    llist_t* list = (llist_t*)args;

    for (unsigned int i=0; i<100; i++) {
        unsigned int r = rand()%NUM_MAX;
        unsigned int p = scaled_dict_pos(r);
        const char* name = NULL;
        if ((name = name_create(dict[p], r)) == NULL) {
            fprintf(stderr, "error alocating space to store the name on llist_insert()\n");
            exit(-1);
        }
        monitor_write(list, r, name);
        printf("inserted %d with name %s\n", r, name);
    }
    return NULL;
}

int ex07(void)
{
    llist_t list;
    llist_init(&list);
    srand(time(NULL));
    
    pthread_create(&th_writer, NULL, writer_task, &list);
    for (unsigned int i=0; i<READERS_NUM; i++) {
        pthread_create(&th_readers[i], NULL, reader_task, &list);
    }
 
    pthread_join(th_writer, NULL);
    for (unsigned int i=0; i<READERS_NUM; i++) {
        pthread_join(th_readers[i], NULL);
    }
 
    return 0;
}