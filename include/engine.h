/*
 * engine.h
 *
 * Notes: engine & event loop.
 * - `runSimulation` drives ticks and calls event handlers (arrival, departure, light change).
 */

#ifndef ENGINE_H
#define ENGINE_H
#include "datastructs.h"
#include "io.h"

/* engine API */
void handleArrivalEvent(Intersection *intersection, int arrivalActive[], int tick, AdvancedStats *stats, FILE *logFile);
void handleDepartureEvent(Intersection *intersection, int tick, int maxTicks, AdvancedStats *stats, FILE *logFile, int visibleCars, int animSpeed, Queue *passedList);
void handleLightChangeEvent(Intersection *intersection, int greenDuration, int tick, FILE *logFile);
void runSimulation(int maxTicks, int greenDuration, int animSpeed, int visibleCars, Intersection *intersection, AdvancedStats *stats, Queue *passedList);

#endif
