#include "engine.h"
#include "datastructs.h"
#include "io.h"

/*
 * engine.c
 *
 * Notes:
 * - Implements simulation event handlers:
 *    - `handleArrivalEvent`: generates vehicles and enqueues them.
 *    - `handleDepartureEvent`: dequeues from the green lane and records stats.
 *    - `handleLightChangeEvent`: rotates the green light when timer expires.
 * - `runSimulation` shows a compact event loop: read arrivals, process events,
 *   update visualization, and log events. Use this to teach event-driven design.
 */

void handleArrivalEvent(Intersection *intersection, int arrivalActive[], int tick, AdvancedStats *stats, FILE *logFile)
{
    Direction allDirs[4] = {NORTH, SOUTH, EAST, WEST};
    int anyArrivalsThisTick = 0;
    for (int i = 0; i < 4; i++)
    {
        if (arrivalActive[i] == 1)
        {
            Vehicle v;
            sprintf_s(v.plateNumber, sizeof(v.plateNumber), "%c-%04d", allDirs[i], rand() % 10000);
            v.arrivalTime = tick;
            v.departureTime = 0;
            v.dir = allDirs[i];

            enqueue(&intersection->lanes[i], v);
            stats->totalVehiclesGenerated++;
            anyArrivalsThisTick = 1;

            printf("[+] Vehicle %s arrived at %c lane.\n", v.plateNumber, v.dir);
            if (logFile != NULL)
            {
                fprintf(logFile, "[TICK %02d] ARRIVAL  : Vehicle %s joined %c lane.\n", tick, v.plateNumber, v.dir);
                fflush(logFile);
            }
        }

        int currentCount = intersection->lanes[i].count;
        if (currentCount > stats->peakVehicleCount[i])
            stats->peakVehicleCount[i] = currentCount;
        if (currentCount > stats->maxQueueLength)
            stats->maxQueueLength = currentCount;
    }
    if (anyArrivalsThisTick == 0)
        printf("[i] No new vehicles arrived in this tick.\n");
}

/*
 * handleArrivalEvent
 * Tip: called each tick to add vehicles to active lanes.
 * - Updates stats and writes an arrival line to the log (if open).
 * - Try changing the arrival pattern (random/sample file) to see different outcomes.
 */

void handleDepartureEvent(Intersection *intersection, int tick, int maxTicks, AdvancedStats *stats, FILE *logFile, int visibleCars, int animSpeed, Queue *passedList)
{
    int greenIdx = getLaneIndex(intersection->currentGreen);
    Vehicle departingVehicle;
    if (dequeue(&intersection->lanes[greenIdx], &departingVehicle))
    {
        departingVehicle.departureTime = tick;
        printf("\n\n=== Traffic Simulation (Animation Mode) ===\n");
        printf("--- TICK %d/%d (Departure Phase) ---\n", tick, maxTicks);
        printf("[!] Vehicle %s passed from %c. (Waited: %d ticks)\n",
               departingVehicle.plateNumber, intersection->currentGreen, (tick - departingVehicle.arrivalTime));

        stats->totalVehiclesPassed++;
        stats->totalWaitTime += (tick - departingVehicle.arrivalTime);

        if (logFile != NULL)
        {
            fprintf(logFile, "[TICK %02d] DEPARTURE: Vehicle %s passed from %c (Waited %d ticks).\n",
                    tick, departingVehicle.plateNumber, intersection->currentGreen, (tick - departingVehicle.arrivalTime));
            fflush(logFile);
        }

        enqueue(passedList, departingVehicle);
        drawVisualIntersection(intersection, visibleCars);
        Sleep(animSpeed);
    }
}

/*
 * handleDepartureEvent
 * Tip: pops one vehicle from the green lane, records its wait time, and logs it.
 * - Moves the vehicle to `passedList` so searches can find it after passing.
 */

void handleLightChangeEvent(Intersection *intersection, int greenDuration, int tick, FILE *logFile)
{
    intersection->greenTicksLeft--;
    if (intersection->greenTicksLeft <= 0)
    {
        Direction newDirection;
        switch (intersection->currentGreen)
        {
        case NORTH:
            newDirection = EAST;
            break;
        case EAST:
            newDirection = SOUTH;
            break;
        case SOUTH:
            newDirection = WEST;
            break;
        case WEST:
        default:
            newDirection = NORTH;
            break;
        }
        intersection->currentGreen = newDirection;
        intersection->greenTicksLeft = greenDuration;
        if (logFile != NULL)
        {
            fprintf(logFile, "[TICK %02d] LIGHT    : Green light changed to %c lane.\n", tick, newDirection);
            fflush(logFile);
        }
    }
}

/*
 * handleLightChangeEvent
 * Tip: decrements the green-timer and picks a new green lane when time runs out.
 * - Uses round-robin ordering for deterministic scheduling.
 */

void runSimulation(int maxTicks, int greenDuration, int animSpeed, int visibleCars, Intersection *intersection, AdvancedStats *stats, Queue *passedList)
{
    stats->totalVehiclesGenerated = 0;
    stats->totalVehiclesPassed = 0;
    stats->totalWaitTime = 0;
    stats->avgWaitTime = 0.0;
    stats->maxQueueLength = 0;
    for (int i = 0; i < 4; i++)
    {
        stats->peakVehicleCount[i] = 0;
        stats->finalQueueStatus[i] = 0;
    }
    intersection->greenTicksLeft = greenDuration;

    FILE *arrivalsFile = NULL;
    fopen_s(&arrivalsFile, "arrivals.txt", "r");
    if (arrivalsFile == NULL)
    {
        printf("[!] arrivals.txt not found. Creating a sample simulation file...\n");
        fopen_s(&arrivalsFile, "arrivals.txt", "w");
        if (arrivalsFile != NULL)
        {
            for (int t = 0; t < maxTicks; t++)
                fprintf(arrivalsFile, "%d %d %d %d\n", rand() % 2, rand() % 2, rand() % 2, rand() % 2);
            fclose(arrivalsFile);
            fopen_s(&arrivalsFile, "arrivals.txt", "r");
        }
    }

    FILE *logFile = NULL;
    fopen_s(&logFile, "log.txt", "w");
    if (logFile != NULL)
    {
        fprintf(logFile, "=========================================\n");
        fprintf(logFile, "      TRAFFIC SIMULATION EVENT LOG    \n");
        fprintf(logFile, "=========================================\n");
        fprintf(logFile, "Max Ticks: %d | Green Duration: %d\n\n", maxTicks, greenDuration);
        fflush(logFile);
    }

    for (int tick = 1; tick <= maxTicks; tick++)
    {
        printf("\n\n=== Traffic Simulation (Animation Mode) ===\n");
        printf("--- TICK %d/%d (Arrival Phase) ---\n", tick, maxTicks);

        int arrivalActive[4] = {0, 0, 0, 0};
        if (arrivalsFile != NULL)
        {
            char line[100];
            if (fgets(line, sizeof(line), arrivalsFile))
                sscanf_s(line, "%d %d %d %d", &arrivalActive[0], &arrivalActive[1], &arrivalActive[2], &arrivalActive[3]);
        }

        handleArrivalEvent(intersection, arrivalActive, tick, stats, logFile);
        drawVisualIntersection(intersection, visibleCars);
        Sleep(animSpeed);

        handleDepartureEvent(intersection, tick, maxTicks, stats, logFile, visibleCars, animSpeed, passedList);
        handleLightChangeEvent(intersection, greenDuration, tick, logFile);
    }

    if (arrivalsFile != NULL)
        fclose(arrivalsFile);
    if (logFile != NULL)
    {
        fprintf(logFile, "\n=== SIMULATION ENDED ===\n");
        fflush(logFile);
        fclose(logFile);
    }

    int vehiclesRemaining = 0;
    for (int i = 0; i < 4; i++)
    {
        stats->finalQueueStatus[i] = intersection->lanes[i].count;
        vehiclesRemaining += intersection->lanes[i].count;
    }
    if (stats->totalVehiclesPassed > 0)
        stats->avgWaitTime = (double)stats->totalWaitTime / stats->totalVehiclesPassed;

    const char *laneNames[4] = {"North", "South", "East", "West"};
    int busiestLaneIdx = 0;
    for (int i = 1; i < 4; i++)
        if (stats->peakVehicleCount[i] > stats->peakVehicleCount[busiestLaneIdx])
            busiestLaneIdx = i;

    printf("\n===================================================\n");
    printf("                SIMULATION SUMMARY                 \n");
    printf("===================================================\n");
    printf("Total Vehicles Generated      : %d\n", stats->totalVehiclesGenerated);
    printf("Total Vehicles Passed         : %d\n", stats->totalVehiclesPassed);
    printf("Vehicles Still Waiting in Lane: %d\n", vehiclesRemaining);
    printf("---------------------------------------------------\n");
    printf("Combined Wait Time (All Cars) : %d ticks\n", stats->totalWaitTime);
    printf("Average Wait Time Per Car     : %.2f ticks\n", stats->avgWaitTime);
    printf("Max Queue Length Seen (Peak)  : %d cars\n", stats->maxQueueLength);
    printf("Busiest Lane (Most Congested) : %s (Reached %d cars)\n", laneNames[busiestLaneIdx], stats->peakVehicleCount[busiestLaneIdx]);
    printf("---------------------------------------------------\n");
    printf("Final Queues Status at End:\n");
    printf(" >> North: %d | South: %d | East: %d | West: %d\n",
           stats->finalQueueStatus[0], stats->finalQueueStatus[1], stats->finalQueueStatus[2], stats->finalQueueStatus[3]);
    printf("===================================================\n");

    saveStatsToFile(*stats, vehiclesRemaining);
    printf("\n[!] Simulation summary saved successfully to stats.txt\n");
    printf("[!] Event log saved successfully to log.txt\n");
    printf("\nPress Enter to return to menu...");
    getchar();
}

/*
 * runSimulation
 * Tip: main simulation loop — read arrivals, handle events, update display, log.
 */
