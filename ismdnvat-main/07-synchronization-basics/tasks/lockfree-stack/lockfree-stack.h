#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>

#define LFSTACK_EMPTY   (uintptr_t)NULL

typedef struct Node {
    uintptr_t value;
    struct Node* next;
} Node;

typedef struct {
    _Atomic(Node*) top;
} lfstack_t;


int lfstack_init(lfstack_t *stack) {
    Node* dummy = (Node*)malloc(sizeof(Node));
    if (!dummy) {
        return 1;
    }
    dummy->value = LFSTACK_EMPTY;
    dummy->next = NULL;
    atomic_store(&stack->top, dummy);
    return 0;
}

int lfstack_push(lfstack_t *stack, uintptr_t value) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) {
        return 1;
    }
    new_node->value = value;

    Node* old_top;
    do {
        old_top = atomic_load(&stack->top);
        new_node->next = old_top;
    } while (!atomic_compare_exchange_strong(&stack->top, &old_top, new_node));

    return 0;
}

int lfstack_pop(lfstack_t *stack, uintptr_t *value) {
    Node* old_top;
    Node* new_top;
<<<<<<< HEAD
    do {
=======
    while (!atomic_compare_exchange_strong(&stack->top, &old_top, new_top)) {
>>>>>>> 60ddfec1765a86682b84e1b169f3394d2d5ceb25
        old_top = atomic_load(&stack->top);
        if (old_top == NULL) {
            *value = LFSTACK_EMPTY;
            return 1;
        }
        new_top = old_top->next;
<<<<<<< HEAD
    } while (!atomic_compare_exchange_strong(&stack->top, &old_top, new_top));
=======
    }
>>>>>>> 60ddfec1765a86682b84e1b169f3394d2d5ceb25

    *value = old_top->value;
    free(old_top);
    return 0;
}

int lfstack_destroy(lfstack_t *stack) {
    Node* current = atomic_load(&stack->top);
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    return 0;
<<<<<<< HEAD
}
=======
}
>>>>>>> 60ddfec1765a86682b84e1b169f3394d2d5ceb25
