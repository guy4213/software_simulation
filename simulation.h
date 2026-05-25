#ifndef SIMULATION_H
#define SIMULATION_H
#include "portability.h"

/* --- הגדרות טיפוסים ומבני נתונים --- */

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
    int departureTime; /* <--- השדה החדש ששומר מתי הרכב יצא */
    Direction dir;
} Vehicle;

/* צומת ברשימה מקושרת כפולה עבור הרכבים בתור */
typedef struct Node
{
    Vehicle data;
    struct Node *prev;
    struct Node *next;
} Node;

/* מבנה המנהל את ראש וזנב התור עבור נתיב בודד */
typedef struct
{
    Node *head;
    Node *tail;
    int count;
} Queue;

typedef struct
{
    Queue lanes[4];         /* 0: צפון, 1: דרום, 2: מזרח, 3: מערב */
    Direction currentGreen; /* הכיוון הנוכחי שבו יש אור ירוק */
    int greenTicksLeft;     /* מספר הטיקים שנותרו לאור הירוק הנוכחי */
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

/* מבנה עזר זמני לצורך אלגוריתם המיון */
typedef struct
{
    char name[15];
    int value;
} SortItem;

/* --- פרוטוטייפים של פונקציות המערכת --- */
void initIntersection(Intersection *intersection);
void enqueue(Queue *q, Vehicle v);
int dequeue(Queue *q, Vehicle *outVehicle);
int getLaneIndex(Direction dir);
void freeIntersection(Intersection *intersection);
void drawVisualIntersection(Intersection *intersection, int visibleCars);

/* הפונקציה המרכזית להרצת הסימולציה */
void runSimulation(int maxTicks, int greenDuration, int animSpeed, int visibleCars, Intersection *intersection, AdvancedStats *stats, Queue *passedList);

/* פונקציות ניהול קבצים, קונפיגורציה וסטטיסטיקה */
int loadConfig(int *maxTicks, int *greenDuration, int *animSpeed, int *visibleCars);
int saveConfig(int maxTicks, int greenDuration, int animSpeed, int visibleCars);
void saveStatsToFile(AdvancedStats stats, int vehiclesRemaining);
void printStatsFromLog();
void printEventLog();

/* מטפלי אירועים (Event Handlers) */
void handleArrivalEvent(Intersection *intersection, int arrivalActive[], int tick, AdvancedStats *stats, FILE *logFile);
void handleDepartureEvent(Intersection *intersection, int tick, int maxTicks, AdvancedStats *stats, FILE *logFile, int visibleCars, int animSpeed, Queue *passedList);
void handleLightChangeEvent(Intersection *intersection, int greenDuration, int tick, FILE *logFile);

/* אלגוריתמים: חיפוש ומיון (דרישות סעיף 5) */
void searchVehicleByPlate(Intersection *intersection, Queue *passedList, const char *plateNumber);
void sortAndRankLanes(AdvancedStats stats);

#endif