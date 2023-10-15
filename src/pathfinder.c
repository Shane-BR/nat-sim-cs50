#include "pathfinder.h"
#include "helpers.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

path_node* lowestCost(list_node* list);
list_node* getListNodeFromData(list_node* head, void* data);

// Erase and free the list after use please
list_node* getPath(position start, position goal, int search_area_size, tile search_area[search_area_size][search_area_size])
{

    path_node start_node;
    start_node.pos = start;
    start_node.prev_node = NULL;

    const int DIRECTIONS_SIZE = 8;
    const position DIRECTIONS[] = 
    {
        newPosition(-1, -1),
        newPosition(0, -1),
        newPosition(1, -1),
        newPosition(-1, 0),
        newPosition(1, 0),
        newPosition(-1, 1),
        newPosition(0, 1),
        newPosition(1, 1)
    };
    
    // A closed node is one that has already been discovered by the algorithm.
    // An open node is one that is on the edge of the searched area.
    list_node* open_list = NULL; 
    list_node* closed_list = NULL;

    path_node* cur = &start_node;

    // Add the start node to the open_list
    addLinkedListNode(&open_list, &start_node);

    while (open_list != NULL) 
    {
        cur = lowestCost(open_list);
        removeLinkedListNode(&open_list, getListNodeFromData(open_list, cur), false);
        addLinkedListNode(&closed_list, cur);

        if (comparePosition(cur->pos, goal))
            break;

        // Add neighbors to the open_list
        for (int i = 0; i < DIRECTIONS_SIZE; i++)
        {
            position direction = DIRECTIONS[i];

            path_node* neighbor = malloc(sizeof(path_node));
            neighbor->pos.x = cur->pos.x + direction.x;
            neighbor->pos.y = cur->pos.y + direction.y;

            // Check if already searched
            if (getListNodeFromData(closed_list, neighbor) != NULL) 
            {    
                free(neighbor);
                continue;
            }

            neighbor->prev_node = cur;

            // if not in bounds
            if 
            (
                neighbor->pos.x < search_area[0][0].position.x ||
                neighbor->pos.y < search_area[0][0].position.y ||
                neighbor->pos.x > search_area[search_area_size-1][search_area_size-1].position.x ||
                neighbor->pos.y > search_area[search_area_size-1][search_area_size-1].position.y
            ) 
            {
                free(neighbor);
                continue;
            }

            if (getListNodeFromData(open_list, neighbor) == NULL)
            {
                addLinkedListNode(&open_list, neighbor);
            }
            else 
            {
                free(neighbor);
            }

        }
    }

    eraseLinkedList(open_list);
    eraseLinkedList(closed_list);

    // Create path
    list_node* head;

    // write the path to the linked list
    while (!comparePosition(cur->pos, start))
    {
        // By default linked list nodes are added to the FRONT of the list.
        // We are also getting the path positions in reverse order.
        // So, by nature our linked list should come out in the correct order.
        // finally! some fucking convenience for once.
        addLinkedListNode(&head, &cur->pos);
        cur = cur->prev_node;
    }
    return head;
}




// Return the losest cost position in the open_list
path_node* lowestCost(list_node* list)
{
    list_node* cur = list;
    path_node* lowest;
    unsigned int current_lowest_cost = UINT32_MAX;

    while (cur != NULL) 
    {
        path_node* cur_node = (path_node*)cur->data;
        if (getMapTile(cur_node->pos).traversability < current_lowest_cost) 
        {
            lowest = cur_node;
            current_lowest_cost = getMapTile(cur_node->pos).traversability;
        }

        cur = cur->next;
    }

    return lowest;
}

list_node* getListNodeFromData(list_node* head, void* data)
{
    while (head != NULL)
    {
        if (comparePosition(((path_node *)(head->data))->pos, ((path_node *)data)->pos))
        {
            return head;
        }

        head = head->next;
    }

    return NULL;
}