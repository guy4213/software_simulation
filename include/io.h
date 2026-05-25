/*
 * io.h
 *
 * Notes: config and logging helpers.
 * - Read/write `config.txt`, `log.txt`, and `stats.txt` using simple line parsing.
 * - Tip: always check file-open and parse return values when testing.
 */

#ifndef IO_H
#define IO_H
#include "types.h"

/* I/O API */
int loadConfig(int *maxTicks, int *greenDuration, int *animSpeed, int *visibleCars);
int saveConfig(int maxTicks, int greenDuration, int animSpeed, int visibleCars);
void saveStatsToFile(AdvancedStats stats, int vehiclesRemaining);
void printStatsFromLog();
void printEventLog();

#endif
