#ifndef NS_DATATYPES
#define NS_DATATYPES

#include <stdbool.h>
#include <stdint.h>
#include "constants.h"

typedef struct
{
    short int x;
    short int y;
} position;

// Struct of individual tiles in the 64x64 map
typedef struct
{
    int8_t ruling_nation; // index of the nation this tile belongs to. -1 for no ruling nation

    uint8_t food_abundance;
    uint8_t material_abundance;
    uint8_t traversability; // int to tell how many turns it takes to move through from 0 (1/2 a day) to 4 (2 days)
    uint8_t survivability; // int to tell how high the birth and death rates would be if it was settled on

    position position;
} tile;

typedef struct
{
    bool active;
    unsigned short int days_to_recover;
    uint8_t incubation_period;
    uint8_t severity;
    uint8_t infectivity_rate;
    unsigned short int immunity_period; // days
    double damage;
    char* type;
} disease;

typedef struct 
{
    const char* key;
    int value;
} dict_node;

typedef struct list_node
{
    void* data;
    struct list_node* next;
} list_node;

typedef enum
{
    NONE, CRAFTSMAN, WARRIOR, GATHERER
} CitClass;

typedef enum
{
    NATURAL, DISEASE, STARVATION, INJURY, CHILDBIRTH, STILLBIRTH
} CitDamageSource;

typedef struct citizen
{
    uint8_t age;
    position position;

    float health;
    float hunger;
    CitDamageSource last_damage_source;

    disease disease;
    int med_history_size;
    dict_node* medical_history; // Dynamic array containing a dictonary of this citizen's medical history.  The keys are diseases/injury and value is the tick it took place.
    uint8_t living_conditions; // Controlled by settlement infrastructure
    uint8_t gender; // 0 = MALE  1 = FEMALE

    bool pregnant;
    short int days_pregnant;
    struct citizen* unborn_child;

    short int days_in_relationship;
    struct citizen* partner;

    short int child_num;
    struct citizen** children; // Dynamic array of pointers to children of citizen (youngest at element n)

    // Parent array
    struct citizen* parents[2];

    CitClass citizen_class; // LEAST TO MOST IMPORTANT

} citizen;

typedef enum
{
    SETTLER,
    MILITARY
} UnitClass;

typedef struct
{
    position position;
    uint8_t nation;

    // Defines the behaviour of the unit
    UnitClass unit_class;

    list_node* path; // Must be a list of path_nodes
    int current_traversability; // Keeps track of the current tiles traversability minus move_speed each tick
    int move_speed;
    
    citizen** citizens;
    unsigned int cits_amt;

} unit;

typedef struct 
{
    tile* tile;
    citizen* workers[MAX_BORDER_WORKERS];
    unsigned short workers_count;
} border;

typedef struct path_node
{
    position pos;
    struct path_node* prev_node;
} path_node;

typedef struct
{
    uint8_t nation;
    position position;

    uint8_t level; // 0 = TOWN, 1 = CITY // TODO replace with enum

    // A bool that indicates if the settlement is active.  Used for protection against listing non-existant settlements in the settlements hash table
    bool active;

    border** borders;

    // Local stats
    citizen** citizens; // Dynamic array of pointers to citizens

    unsigned long long last_settler_tick;

    unsigned int local_population;
    unsigned int population_capacity;

    double food;
    double materials;

    double local_infrastructure;
    uint8_t local_morale;
    uint8_t cultivation_efficiency;

} settlement;

typedef struct
{
    char* name;

    unit** units;
    unsigned int units_amt;
    unsigned int units_mem_capacity;

    unsigned int population;

    unsigned int money;

    list_node* death_list; // A linked list containing pointers to the dead population

    uint8_t health;
    uint8_t trade;
    uint8_t magic;
    uint8_t morale;
    uint8_t fear;
    uint8_t infamy;
    uint8_t infrastructure;
    uint8_t military;
    uint8_t artifacts;

} nation;

citizen* newCitizen(uint8_t age, uint8_t gender, citizen* parents[2], position pos);
void addDictNode(dict_node** dict, int* dict_size, const char* key, int value);
void eraseDict(dict_node* dict, int* dict_size);
int dictFind(dict_node* dict, int dict_size, const char* key);
void addLinkedListNode(list_node** head, void* data);
void removeLinkedListNode(list_node** head, list_node* remove, bool free_data);
void eraseLinkedList(list_node** head, bool free_data);

#endif