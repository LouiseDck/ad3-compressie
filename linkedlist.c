#include <stdlib.h>
#include <stdio.h>
#include "linkedlist.h"

List* new_list(){
    List* list = (List*)calloc(1, sizeof(List));
    list->firstitem = NULL;
    list->lastitem = NULL;
    list->number_of_items = 0;
    return list;
}

Item* new_item(int freq, char data, Item* volgend){
    Item* new = (Item*) calloc(1, sizeof(Item));
    new->data = data;
    new->freq = freq;
    new->volgend = volgend;
}

Item* new_leaf_item(Leaf* leaf){
    Item* new = (Item*) calloc(1, sizeof(Item));
    new->leaf = leaf;
    new->freq = leaf->freq;
}

void add_leaf_last(List* list, Leaf* leaf){
    if(list->firstitem == NULL){
        Item* new = new_leaf_item(leaf);
        list->firstitem = new;
        list->lastitem = new;
        list->number_of_items++;
    } else{
        Item *new_last_item = new_leaf_item(leaf);
        list->lastitem->volgend = new_last_item;
        list->lastitem = new_last_item;
        list->number_of_items++;
    }
}

void add_element_last(List* list, int freq, char data){
    if(list->firstitem == NULL){
        Item* new_first_item = new_item(freq, data, NULL);
        list->firstitem = new_first_item;
        list->lastitem = new_first_item;
        list->number_of_items++;
    } else {
        Item *new_last_item = new_item(freq, data, NULL);
        list->lastitem->volgend = new_last_item;
        list->lastitem = new_last_item;
        list->number_of_items++;
    }
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

void add_element_pos(List* list, char data, int freq, int pos){
    if(pos == 0){
        Item* old_first = list->firstitem;
        Item* new_first = new_item(freq, data, old_first);
        list->firstitem = new_first;
    } else{
        int cur_pos = 1;
        Item* item = list->firstitem;
        while(cur_pos < pos && item->volgend){
            item = item->volgend;
            cur_pos++;
        }
        Item* previous = item;
        Item* next = item->volgend;
        Item* new = new_item(freq, data, next);
        previous->volgend = new;
    }
    list->number_of_items++;
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

void print_all(List* list){
    Item* item = list->firstitem;
    while(item){
        printf("%s", item->data);
        item = item->volgend;
    }
}

