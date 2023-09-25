#include "datatypes.h"
#include "constants.h"
#include "helpers.h"
#include "borders.h"
#include "disease.h"

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

extern tile map[MAP_SIZE][MAP_SIZE];
extern settlement settlements[MAP_SIZE*MAP_SIZE]; // Simple hash table of settlements
nation nations[NAT_AMOUNT];

// Constructer-like function for getting a new position
position newPosition(int x, int y)
{
    position pos;
    pos.x = x;
    pos.y = y;
    return pos;
}

citizen* newCitizen(uint8_t age, uint8_t gender, citizen* parents[2])
{
    citizen* c = malloc(sizeof(citizen));
    c->age = age;
    c->gender = gender;
    c->health = UINT8_MAX;
    c->living_conditions = 127;
    c->pregnant = false;
    c->days_pregnant = 0;

    c->disease = noDisease();

    c->med_history_size = 0;
    c->medical_history = NULL;

    c->days_in_relationship = 0;
    c->partner = NULL;
    c->child_num = 0;
    c->children = NULL;
    c->unborn_child = NULL;
    c->citizen_class = NONE;

    
    c->parents[0] = parents != NULL ? parents[0] : NULL;
    c->parents[1] = parents != NULL ? parents[1] : NULL;

    return c;
}

// Creates and adds a new node to the given dictonary.  Updates dict_size
void addDictNode(dict_node** dict, int* dict_size, char* key, int value)
{
    dict_node node;
    node.key = key;
    node.value = value;

    // Add dict_node to dict.  Will work if dict is NULL
    *dict = realloc(*dict, sizeof(dict_node)*++(*dict_size));

    if(*dict == NULL)
    {
        printf("Unable to add dict_node to dictonary");
        exit(1);
    }

    (*dict)[(*dict_size)-1] = node;

}

void eraseDict(dict_node* dict, int* dict_size)
{
    *dict_size = 0;
    free(dict);
}

// Returns the index of a given key if found.  Returns -1 if no element with that key is found.
int dictFind(dict_node* dict, int dict_size, char* key)
{

    if (dict == NULL || dict_size <= 0) return -1;
    for (int i = dict_size-1; i >= 0; i--) // Start from the end to get the most recent entry in case of a dictonary that allows dupes (like medical_history).
    {
        if (compareString(dict[i].key, key))
        {
            return i;
        }
    }

    return -1;
}

// Add an element to the head of a linked list
void addLinkedListNode(list_node** head, void* data)
{
    list_node* new = malloc(sizeof(list_node));

    if (new == NULL)
    {
        exit(1);
    }

    new->data = data;
    new->next =  *head == NULL ? NULL : *head;
    *head = new;
}

void eraseLinkedList(list_node* head)
{
    if (head == NULL) return;
    eraseLinkedList(head->next);
    free(head);
}