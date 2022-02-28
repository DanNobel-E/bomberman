#include "double_list_item.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#define new(value, typename)                       \
    typename *new_##typename(value v)              \
    {                                              \
        typename *item = malloc(sizeof(typename)); \
        if (!item)                                 \
        {                                          \
            return NULL;                           \
        }                                          \
        item->object = v;                          \
        item->node.next = NULL;                    \
        item->node.prev = NULL;                    \
        return item;                               \
    }

#define printList(list_type, typename)           \
    void printList_##typename(list_type * list)  \
    {                                            \
        list_type *item = list;                  \
        while (item)                             \
        {                                        \
            print_##typename(*item);             \
            item = (list_type *)item->node.next; \
        }                                        \
                                                 \
        printf("\n\n");                          \
    }

new (int, int_item);
new (float, float_item);
new (char, char_item);
new (size_t, size_t_item);
new (const char *, string_item);
new (bomberman_t *, player_item);


printList(int_item, int_item);
printList(float_item, float_item);
printList(char_item, char_item);
printList(size_t_item, size_t_item);
printList(string_item, string_item);

void print_int_item(int_item item)
{

    printf("%d\n", item.object);
}

void print_float_item(float_item item)
{

    printf("%f\n", item.object);
}

void print_char_item(char_item item)
{

    printf("%c\n", item.object);
}

void print_size_t_item(size_t_item item)
{

    printf("%zu\n", item.object);
}

void print_string_item(string_item item)
{

    printf("%s\n", item.object);
}