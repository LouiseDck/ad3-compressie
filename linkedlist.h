//
// Created by louise on 17/10/16.
//

#ifndef COMPRESSIE_LINKEDLIST_H
#define COMPRESSIE_LINKEDLIST_H

#include "huffman_tree.h"

typedef struct Item{
    char data;
    Leaf* leaf;
    int freq;
    struct Item* volgend;
} Item;

typedef struct List{
    Item* firstitem;
    Item* lastitem;
    int number_of_items;
} List;

List* new_list();

Item* new_item(int freq, char data, Item* volgend);

Item* new_leaf_item(Leaf* leaf);

void add_element_freq(List* list, Item* item);

void remove_element(List* list, int pos);

#endif //COMPRESSIE_LINKEDLIST_H
