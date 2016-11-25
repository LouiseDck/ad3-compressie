#include <stdlib.h>
#include <stdio.h>
#include "linkedlist.h"

List* new_list(){
    List* list = (List*)calloc(1, sizeof(List));
    list->firstitem = NULL;
    list->number_of_items = 0;
    return list;
}

Item* new_item(int freq, unsigned char data, Item* volgend){
    Item* new = (Item*) calloc(1, sizeof(Item));
    new->data = data;
    new->freq = freq;
    new->volgend = volgend;
    return new;
}

Item* new_leaf_item(Leaf* leaf){
    Item* new = (Item*) calloc(1, sizeof(Item));
    new->leaf = leaf;
    new->freq = leaf->freq;
    return new;
}

void add_element_freq(List* list, Item* item){
    if(list->firstitem == NULL){
        list->firstitem = item;
        list->number_of_items++;
    } else if(list->firstitem->freq > item->freq){
        Item* next = list->firstitem;
        item->volgend = next;
        list->firstitem = item;
        list->number_of_items++;
    } else {
        Item* current = list->firstitem;
        while (current->volgend && item->freq > current->volgend->freq) {
            current = current->volgend;
        }
        Item* prev = current;
        Item* next = current->volgend;
        item->volgend = next;
        prev->volgend = item;
        list->number_of_items++;
    }
}

void remove_element(List* list, int pos){
    if(pos == 0){
        Item* new_first = list->firstitem->volgend;
        free(list->firstitem);
        list->firstitem = new_first;
    } else {
        int i = 0;
        Item *item = list->firstitem;
        for (i = 0; i < pos - 1; i++) {
            item = item->volgend;
        }
        Item *previous = item;
        Item *next = item->volgend->volgend;
        previous->volgend = next;
        free(item->volgend);
    }
    list->number_of_items--;
}

void remove_list(List* list){
    Item* item = list->firstitem;
    while(item){
        Item* new_item = item->volgend;
        free(item);
        item = new_item;
    }
    free(list);
}

