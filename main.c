#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Data Structures Definitions --- */

typedef enum
{
    NORTH = 'N',
    SOUTH = 'S',
    EAST = 'E',
    WEST = 'W'
} Direction;

typedef struct
{
    char plateNumber[10];
    int arrivalTime;
    Direction dir;
} Vehicle;

/* Doubly Linked List Node */
typedef struct Node
{
    Vehicle data;
    struct Node *prev;
    struct Node *next;
} Node;

/* Queue Wrapper for each Lane */
typedef struct
{
    Node *head; /* Points to the first vehicle (next to leave) */
    Node *tail; /* Points to the last vehicle (newly arrived) */
    int count;
} Queue;

typedef struct
{
    Queue lanes[4];         /* 0: North, 1: South, 2: East, 3: West */
    Direction currentGreen; /* Current direction with green light */
    int greenTicksLeft;
} Intersection;

/* --- Function Prototypes --- */

void initIntersection(Intersection *intersection);
void enqueue(Queue *q, Vehicle v);
int dequeue(Queue *q, Vehicle *outVehicle);
int getLaneIndex(Direction dir);
void printIntersectionState(Intersection *intersection, int currentTick);

/* --- Main Simulation Skeleton --- */

int main()
{
    Intersection myIntersection;
    initIntersection(&myIntersection);

    int maxTicks = 5;

    printf("=== Starting Traffic Simulation ===\n\n");

    for (int tick = 1; tick <= maxTicks; tick++)
    {

        /* 1. Event Generation (Mocking vehicle arrivals) */
        if (tick == 1)
        {
            Vehicle v1 = {"123-ABC", tick, NORTH};
            enqueue(&myIntersection.lanes[getLaneIndex(NORTH)], v1);
        }
        if (tick == 2)
        {
            Vehicle v2 = {"987-XYZ", tick, NORTH};
            enqueue(&myIntersection.lanes[getLaneIndex(NORTH)], v2);
        }

        /* 2. Traffic Logic (Processing the green light lane) */
        Vehicle departingVehicle;
        // todo-fix : currentGreen param needs to have its name changed to better reflect its purpose
        int laneIdx = getLaneIndex(myIntersection.currentGreen);

        if (dequeue(&myIntersection.lanes[laneIdx], &departingVehicle))
        {
            printf("[Tick %d] Vehicle %s passed from lane %c.\n",
                   tick, departingVehicle.plateNumber, departingVehicle.dir);
        }

        /* 3. Display current state */
        printIntersectionState(&myIntersection, tick);
    }

    printf("\n=== Simulation Ended ===\n");

    /* TODO: Implement a cleanup function to free all memory */

    return 0;
}

/* --- Function Implementations (Stubs) --- */

void initIntersection(Intersection *intersection)
{
    for (int i = 0; i < 4; i++)
    {
        intersection->lanes[i].head = NULL;
        intersection->lanes[i].tail = NULL;
        intersection->lanes[i].count = 0;
    }
    intersection->currentGreen = NORTH;
    intersection->greenTicksLeft = 3;
}

/* Helper to map Enum character to array index */
int getLaneIndex(Direction dir)
{
    switch (dir)
    {
    case NORTH:
        return 0;
    case SOUTH:
        return 1;
    case EAST:
        return 2;
    case WEST:
        return 3;
    default:
        return -1;
    }
}

void enqueue(Queue *q, Vehicle v)
{
    /* TODO: Implement Doubly Linked List insertion at tail */
    q->count++; /* Temporary increment to show activity in skeleton */
}

int dequeue(Queue *q, Vehicle *outVehicle)
{
    /* TODO: Implement Doubly Linked List removal from head */
    if (q->count > 0)
    {
        q->count--;
        /* Mocking a return for the skeleton */
        // strcpy_s(outVehicle->plateNumber, "MOCK-1");
        // outVehicle->dir = NORTH;
        // return 1;
    }
    return 0;
}

void printIntersectionState(Intersection *intersection, int currentTick)
{
    int northIdx = getLaneIndex(NORTH);
    printf("[State at Tick %d] Green Light: %c | Waiting in North: %d\n",
           currentTick, intersection->currentGreen, intersection->lanes[northIdx].count);
    printf("----------------------------------------\n");
}