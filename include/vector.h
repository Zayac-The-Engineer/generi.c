#ifndef VECTOR_H_
#define VECTOR_H_

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

#include "list.h"
#include "macros.h"

#define Vec(T)                     \
    struct ADHOC {                 \
        T *body;                   \
        size_t size;               \
        size_t capacity;           \
        const size_t element_size; \
        const VecTrait impl;       \
        DERIVE(LIST, T)            \
    }

/* remove and pop methods return owned values that need to be freed */
#define DefineVec(NewType, T)                                 \
    typedef Vec(T) NewType;                                   \
    PROXY(LIST, NewType, T)                                   \
    NewType NewType##_new() {                                 \
        return (NewType) {.size = 0,                          \
                          .capacity = 0,                      \
                          .element_size = sizeof(T),          \
                          .body = NULL,                       \
                          .impl = VecImpl,                    \
                          PROXY_ASSIGN(LIST, NewType)};       \
    }                                                         \
    NewType NewType##_with_capacity(size_t capacity) {        \
        NewType self = NewType##_new();                       \
        self.capacity = capacity;                             \
        self.body = malloc(capacity * sizeof(T));             \
        return self;                                          \
    }                                                         \
    NewType NewType##_from_arr(T *arr, size_t bytecap) {      \
        NewType self = NewType##_new();                       \
        self.size = bytecap / sizeof(T);                      \
        self.capacity = bytecap / sizeof(T);                  \
        self.body = memcpy(malloc(bytecap), arr, bytecap);    \
        return self;                                          \
    }                                                         \
    const struct {                                            \
        NewType (*new)();                                     \
        NewType (*with_capacity)(size_t capacity);            \
        NewType (*from_arr)(T * arr, size_t bytecap);         \
    } NewType##_ = {.new = NewType##_new,                     \
                    .with_capacity = NewType##_with_capacity, \
                    .from_arr = NewType##_from_arr};

typedef struct {
    DERIVE_TRAIT(LIST)
    void (*destroy)(void *self);
    void (*resize)(void *self);
    void (*shrink_to_fit)(void *self);
    void (*shrink_to)(void *self, size_t new_cap);
    void (*reserve)(void *self, size_t additional);
    void (*truncate)(void *self, size_t new_cap);
} VecTrait;

extern const VecTrait VecImpl;

#endif // VECTOR_H_
