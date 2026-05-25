/*
 * types.h
 *
 * Notes:
 * - Core types for the project: `Vehicle`, `Node`, `Queue`, `Intersection`, `AdvancedStats`.
 * - Read this file before other modules — it defines the data you will manipulate.
 */

#ifndef TYPES_H
#define TYPES_H
#include "portability.h"

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
    int departureTime;
    Direction dir;
} Vehicle;

typedef struct Node
{
    Vehicle data;
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct
{
    Node *head;
    Node *tail;
    int count;
} Queue;

typedef struct
{
    Queue lanes[4];
    Direction currentGreen;
    int greenTicksLeft;
} Intersection;

typedef struct
{
    int totalVehiclesGenerated;
    int totalVehiclesPassed;
    int totalWaitTime;
    double avgWaitTime;
    int maxQueueLength;
    int peakVehicleCount[4];
    int finalQueueStatus[4];
} AdvancedStats;

typedef struct
{
    char name[15];
    int value;
} SortItem;

#endif
