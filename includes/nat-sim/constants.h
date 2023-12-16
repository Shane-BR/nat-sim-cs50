#ifndef NS_CONSTANTS
#define NS_CONSTANTS

#define VERSION "1.0.0-alpha1"

#define UI_CHARACTER_LIMIT 1000

#define MAP_SIZE 16 // 16 km2
#define NAT_AMOUNT 4 // 4
#define START_SPREAD 15 // Minimum distance apart settlements must be on startup.
#define TICKS_PER_DAY 2
#define TPS 5 // Ticks per second, each tick is half a day (12hrs) in simulation time

#define MAX_TRAVERSABILITY 4

// Radius must be odd number
#define TOWN_BORDER_RADIUS 3
#define CITY_BORDER_RADIUS 5

#define TOWN 0
#define CITY 1

#define MAX_BORDER_WORKERS 12

#define MALE 0
#define FEMALE 1
#define MIN_CONCEPTION_AGE 16
#define MAX_CONCEPTION_AGE 55
#define MIN_WORKING_AGE 14
#define MAX_WORKING_AGE 70
#define STARVE_PERIOD 30 // How many days before DEATH by starvation one half is "pre-starve" and the other half is when the citizen starts taking damage
#define HUNGER_INCREASE_RATE 255.0f / STARVE_PERIOD / 2.0f
#define FOOD_TO_HUNGER HUNGER_INCREASE_RATE + HUNGER_INCREASE_RATE*(1.0f/8.0f)
#define MAX_FOOD_PRODUCED_PER_WORKER 4 // Per tick
#define MAX_MAT_PRODUCED_PER_WORKER 10 // Per tick
#define DEBILITATING_DISEASE_SEVERITY 100

// Units
#define MAX_SETTLER_CAPACITY 15

// Resource types
#define FOOD 0
#define MATERIALS 1

#endif