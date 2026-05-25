#include "io.h"

/*
 * io.c
 *
 * Notes:
 * - Simple examples of file I/O: reading a small config file and writing logs/stats.
 */

int loadConfig(int *maxTicks, int *greenDuration, int *animSpeed, int *visibleCars)
{
    FILE *f;
    if (fopen_s(&f, "config.txt", "r") != 0 || f == NULL)
    {
        return 0;
    }
    char line[50];
    while (fgets(line, sizeof(line), f))
    {
        if (strncmp(line, "MAX_TICKS=", 10) == 0)
        {
            *maxTicks = atoi(line + 10);
        }
        else if (strncmp(line, "GREEN_DURATION=", 15) == 0)
        {
            *greenDuration = atoi(line + 15);
        }
        else if (strncmp(line, "ANIMATION_SPEED=", 16) == 0)
        {
            *animSpeed = atoi(line + 16);
        }
        else if (strncmp(line, "VISIBLE_CARS=", 13) == 0)
        {
            *visibleCars = atoi(line + 13);
        }
    }
    fclose(f);
    return 1;
}

/*
 * loadConfig
 * Tip: reads `config.txt` with simple key=value lines.
 * - Uses `fgets` + `strncmp` to parse values; check return values when testing.
 */

int saveConfig(int maxTicks, int greenDuration, int animSpeed, int visibleCars)
{
    FILE *f;
    if (fopen_s(&f, "config.txt", "w") != 0 || f == NULL)
    {
        return 0;
    }
    fprintf(f, "MAX_TICKS=%d\n", maxTicks);
    fprintf(f, "GREEN_DURATION=%d\n", greenDuration);
    fprintf(f, "ANIMATION_SPEED=%d\n", animSpeed);
    fprintf(f, "VISIBLE_CARS=%d\n", visibleCars);
    fclose(f);
    return 1;
}

/*
 * saveConfig
 * Tip: writes current settings to `config.txt`.
 * - Basic `fprintf` usage; useful for saving experiments.
 */

void saveStatsToFile(AdvancedStats stats, int vehiclesRemaining)
{
    FILE *f;
    if (fopen_s(&f, "stats.txt", "w") != 0 || f == NULL)
    {
        printf("Error writing statistics to file.\n");
        return;
    }
    const char *laneNames[4] = {"North", "South", "East", "West"};
    int busiestLaneIdx = 0;
    for (int i = 1; i < 4; i++)
    {
        if (stats.peakVehicleCount[i] > stats.peakVehicleCount[busiestLaneIdx])
            busiestLaneIdx = i;
    }
    fprintf(f, "--- Last Run Statistics ---\n");
    fprintf(f, "Total Vehicles Generated      : %d\n", stats.totalVehiclesGenerated);
    fprintf(f, "Total Vehicles Passed         : %d\n", stats.totalVehiclesPassed);
    fprintf(f, "Vehicles Still Waiting in Lane: %d\n", vehiclesRemaining);
    fprintf(f, "Combined Wait Time (All Cars) : %d ticks\n", stats.totalWaitTime);
    fprintf(f, "Average Wait Time Per Car     : %.2f ticks\n", stats.avgWaitTime);
    fprintf(f, "Max Queue Length (Single Lane): %d\n", stats.maxQueueLength);
    fprintf(f, "Busiest Lane (Peak Record)    : %s (%d cars)\n", laneNames[busiestLaneIdx], stats.peakVehicleCount[busiestLaneIdx]);
    fprintf(f, "Final Queues Status           : [ North:%d | South:%d | East:%d | West:%d ]\n",
            stats.finalQueueStatus[0], stats.finalQueueStatus[1], stats.finalQueueStatus[2], stats.finalQueueStatus[3]);
    fclose(f);
}

/*
 * saveStatsToFile
 * Tip: writes a readable `stats.txt` summarizing the last run.
 * - Calculates busiest lane and final queue sizes for the report.
 */

void printStatsFromLog()
{
    FILE *f;
    if (fopen_s(&f, "stats.txt", "r") != 0 || f == NULL)
    {
        printf("\n[!] No statistics found. Run the simulation first.\n");
        return;
    }
    char ch;
    printf("\n===================================================\n");
    while ((ch = fgetc(f)) != EOF)
        putchar(ch);
    printf("===================================================\n");
    fclose(f);
}

/*
 * printStatsFromLog
 * Tip: prints `stats.txt` to the console.
 * - Quick utility for checking results without opening the file.
 */

void printEventLog()
{
    FILE *f;
    if (fopen_s(&f, "log.txt", "r") != 0 || f == NULL)
    {
        printf("\n[!] No event log found. Run the simulation first.\n");
        return;
    }
    char line[256];
    printf("\n--- PRINTING EVENT LOG (log.txt) ---\n");
    while (fgets(line, sizeof(line), f))
        printf("%s", line);
    printf("------------------------------------\n");
    fclose(f);
}

/*
 * printEventLog
 * Tip: prints `log.txt` contents; useful to inspect per-tick events.
 * - Make sure the log exists (run a simulation first).
 */
