//
// Created by louise on 18/10/16.
//
#include <stdlib.h>
#include "huffman_tree.h"

Leaf* make_leaf(Leaf* zero_child, Leaf* one_child, unsigned char character, int freq, int zero_or_one, encode_Item encode_array[], int where){
    Leaf* new_leaf = calloc(1, sizeof(Leaf));

    new_leaf->one_child = one_child;
    if(one_child != NULL) new_leaf->one_child->parent = new_leaf;

    new_leaf->zero_child = zero_child;
    if(zero_child != NULL) new_leaf->zero_child->parent = new_leaf;

    new_leaf->character = character;
    new_leaf->freq = freq;
    new_leaf->zero_or_one_child = zero_or_one;

    if(zero_child == NULL && one_child == NULL){
        encode_array[where] = *make_encode_Item(character, new_leaf);
    }
    return new_leaf;
}

Leaf* make_only_leaf(Leaf* zero_child, Leaf* one_child, unsigned char character, int zero_or_one){
    Leaf* new_leaf = calloc(1, sizeof(Leaf));

    new_leaf->one_child = one_child;
    if(one_child != NULL) new_leaf->one_child->parent = new_leaf;

    new_leaf->zero_child = zero_child;
    if(zero_child != NULL) new_leaf->zero_child->parent = new_leaf;

    new_leaf->character = character;
    new_leaf->freq = 0;
    new_leaf->zero_or_one_child = zero_or_one;
    return new_leaf;
}

encode_Item* make_encode_Item(unsigned char data, Leaf* code){
    encode_Item* item = calloc(1, sizeof(encode_Item));
    item->data = data;
    item->code = code;
    return item;
}
