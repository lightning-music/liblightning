#include <assert.h>
#include <stddef.h>

#include "list.h"
#include "mem.h"

typedef struct ListNode *ListNode;

struct ListNode {
    ListNode prev;
    ListNode next;
    void *value;
};

struct List {
    ListNode head;
    ListNode tail;
    unsigned length;
};

static ListNode
ListNode_init(void *value);

static void
ListNode_free(ListNode *node);

/**
 * Initialize a new list
 */
List
List_init() {
    List l;
    NEW(l);
    l->head = ListNode_init(NULL);
    l->tail = ListNode_init(NULL);
    l->head->next = l->tail;
    l->head->prev = NULL;
    l->tail->next = NULL;
    l->tail->prev = l->head;
    l->length = 0;
    return l;
}

/**
 * Push a value onto the end of the list
 */
List
List_push(List l,
          void *x) {
    assert(l);
    ListNode node = ListNode_init(x);
    ListNode last = l->tail->prev;
    last->next = node;
    l->tail->prev = node;
    node->prev = last;
    node->next = l->tail;
    l->length++;
    return l;
}

/**
 * Pop a value from the end of the list
 */
List
List_pop(List l,
         void **x) {
    assert(l);
    if (l->tail->prev != l->head) {
        ListNode last = l->tail->prev;
        last->prev->next = l->tail;
        l->tail->prev = last->prev;
        *x = last->value;
        ListNode_free(&last);
        l->length--;
    }
    return l;
}

/**
 * Add a value to the front of the list
 */
List
List_shift(List l,
           void *x) {
    assert(l);
    ListNode node = ListNode_init(x);
    ListNode front = l->head->next;
    l->head->next = node;
    node->prev = l->head;
    node->next = front;
    front->prev = node;
    l->length++;
    return l;
}

/**
 * Pop a value off the front of the list
 */
List
List_unshift(List l,
             void **x) {
    assert(l);
    if (l->head->next != l->tail) {
        ListNode front = l->head->next;
        l->head->next = front->next;
        front->next->prev = l->head;
        *x = front->value;
        ListNode_free(&front);
        l->length--;
    }
    return l;
}

/**
 * Remove a particular node in the list
 */
void *
List_remove(List l,
            void *x) {
    assert(l);
    if (l->length) {
        ListNode p;
        for (p = l->head->next; p; p = p->next) {
            if (p->value == x) {
                p->prev->next = p->next;
                ListNode_free(&p);
                break;
            }
        }
    }
    return l;
}

/**
 * Free the resources allocated for a list
 */
void
List_free(List *l) {
    assert(l && *l);
    ListNode p, q;
    for (p = (*l)->head->next; p; p = q) {
        q = p->next;
        ListNode_free(&p);
    }
    ListNode_free(&(*l)->head);
    ListNode_free(&(*l)->tail);
    FREE(*l);
}

static ListNode
ListNode_init(void *value) {
    ListNode node;
    NEW(node);
    node->value = value;
    return node;
}

static void
ListNode_free(ListNode *node) {
    assert(node && *node);
    FREE(node);
}
