/*
 * datastructs.h
 *
 * Notes: queue helpers and visualization.
 * - `enqueue` / `dequeue` implement a doubly-linked queue (check head/tail updates).
 * - `drawVisualIntersection` prints an ASCII map — modify it to try different layouts.
 */

#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H
#include "types.h"

/* datastructs API */
void initIntersection(Intersection *intersection);
void enqueue(Queue *q, Vehicle v);
int dequeue(Queue *q, Vehicle *outVehicle);
int getLaneIndex(Direction dir);
void freeIntersection(Intersection *intersection);
void drawVisualIntersection(Intersection *intersection, int visibleCars);

#endif
