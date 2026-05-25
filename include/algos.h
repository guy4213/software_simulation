/*
 * algos.h
 *
 * Notes: simple search & sort helpers.
 * - `searchVehicleByPlate`: linear scan through passed and queued vehicles.
 * - `sortAndRankLanes`: quick example of sorting small arrays for reporting, via a quick sort algorithm.
 */

#ifndef ALGOS_H
#define ALGOS_H
#include "types.h"

/* algorithms API */
void searchVehicleByPlate(Intersection *intersection, Queue *passedList, const char *plateNumber);
void sortAndRankLanes(AdvancedStats stats);

#endif
