#ifndef COMPRESSIE_HUFFMAN_TREE_H
#define COMPRESSIE_HUFFMAN_TREE_H

typedef struct Leaf{
    struct Leaf* zero_child;
    struct Leaf* one_child;
    struct Leaf* parent;
    unsigned char character;
    int freq;
    int  zero_or_one_child;
} Leaf;


typedef struct encode_Item{
    unsigned char data;
    Leaf* code;
} encode_Item;

Leaf* make_leaf(Leaf* zero_child, Leaf* one_child, unsigned char character, int freq, int zero_or_one_child, encode_Item encode_array[], int where);
Leaf* make_only_leaf(Leaf* zero_child, Leaf* one_child, unsigned char character, int zero_or_one);
encode_Item* make_encode_Item(unsigned char data, Leaf* code);

#endif //COMPRESSIE_HUFFMAN_TREE_H
