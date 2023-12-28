#include "pathfinder.h"
#include "helpers.h"
#include "map.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

path_node* lowestCost(list_node* list, position start, position goal);
list_node* getListNodeFromPosData(list_node* head, void* data);
void freeNonPathNodesFromClosedList(list_node* closed_list, list_node* final_path);

// Erase and free the path list after use please
list_node* getPath(position start, position goal, int search_area_size, tile search_area[search_area_size][search_area_size])
{

    path_node* start_node = malloc(sizeof(path_node));

    if (start_node == NULL)
    {
        printf("Unable to allocate memory for path node.");
        return NULL;
    }

    start_node->pos = start;
    start_node->prev_node = NULL;

    const int DIRECTIONS_SIZE = 8;
    const position DIRECTIONS[] = 
    {
        {-1, -1},
        {0, -1},
        {1, -1},
        {-1, 0},
        {1, 0},
        {-1, 1},
        {0, 1},
        {1, 1}
    };
    
    // A closed node is one that has already been searched.
    // An open node is one that is on the edge of the searched area.
    list_node* open_list = NULL; 
    list_node* closed_list = NULL;

    path_node* cur = start_node;

    // Add the start node to the open_list
    addLinkedListNode(&open_list, start_node);

    bool found_goal = false;

    // A* algorithm
    while (open_list != NULL) 
    {
        cur = lowestCost(open_list, start, goal);
        removeLinkedListNode(&open_list, getListNodeFromPosData(open_list, cur), false);
        addLinkedListNode(&closed_list, cur);

        if (comparePosition(cur->pos, goal))
        {
            found_goal = true;
            break;
        }

        // Add neighbors to the open_list
        for (int i = 0; i < DIRECTIONS_SIZE; i++)
        {
            position direction = DIRECTIONS[i];

            path_node* neighbor = malloc(sizeof(path_node));

            if (start_node == NULL)
            {
                printf("Unable to allocate memory for path node.");
                continue;
            }

            neighbor->pos.x = cur->pos.x + direction.x;
            neighbor->pos.y = cur->pos.y + direction.y;

            // Check if already searched
            if (getListNodeFromPosData(closed_list, neighbor) != NULL) 
            {    
                free(neighbor);
                continue;
            }

            neighbor->prev_node = cur;

            // if not in bounds
            // TODO add non-traversable exception too
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

            if (getListNodeFromPosData(open_list, neighbor) == NULL)
            {
                addLinkedListNode(&open_list, neighbor);
            }
            else 
            {
                free(neighbor);
            }

        }
    }

    if (!found_goal) return NULL;

    // Create path
    list_node* head = NULL;

    // write the path to the linked list
    do
    {
        /* 
            By default linked list nodes are added to the FRONT of the list.
            We are also getting the path positions in reverse order.
            So, by nature our linked list should arrange itself in the correct order.
            finally! some fucking convenience for once.
        */

        addLinkedListNode(&head, &cur->pos);
        cur = cur->prev_node;
    } 
    while (cur != NULL && !comparePosition(cur->pos, start));

    // Free open list entirely
    eraseLinkedList(&open_list, true);

    // Free nodes not in the path from the closed list
    freeNonPathNodesFromClosedList(closed_list, head);


    return head;
}

// Return the lowest cost position in the open_list
path_node* lowestCost(list_node* list, position start, position goal)
{
    list_node* cur = list;
    path_node* lowest;
    unsigned int current_lowest_cost = UINT32_MAX;

    while (cur != NULL) 
    {
        path_node* cur_node = (path_node*)cur->data;
        int c = 
            (((float)getMapTile(cur_node->pos)->traversability / MAX_TRAVERSABILITY) * MAP_SIZE) +
            distanceBetweenPositions(cur_node->pos, start) +
            distanceBetweenPositions(cur_node->pos, goal);

        if (c < current_lowest_cost) 
        {
            lowest = cur_node;
            current_lowest_cost = c;
        }

        cur = cur->next;
    }

    return lowest;
}

list_node* getListNodeFromPosData(list_node* head, void* data)
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

void freeNonPathNodesFromClosedList(list_node* closed_list, list_node* final_path)
{
    list_node* cur = closed_list;
    while (cur != NULL)
    {
        list_node* next = cur->next;

        if (getListNodeFromPosData(final_path, cur->data) == NULL)
        {
            removeLinkedListNode(&closed_list, cur, true);
        }
        
        cur = next;
    }
}