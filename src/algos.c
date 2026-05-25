#include "algos.h"

/*
 * algos.c
 *
 * Notes:
 * - Illustrates simple algorithms used on simulation results:
 *    - Linear search through passed vehicles / queues (`searchVehicleByPlate`).
 *    - QuickSort applied to a small fixed array for ranking lanes (`sortAndRankLanes`).
 */

void searchVehicleByPlate(Intersection *intersection, Queue *passedList, const char *plateNumber)
{
    const char *laneNames[4] = {"North", "South", "East", "West"};
    int found = 0;

    Node *current = passedList->head;
    while (current != NULL)
    {
        if (strcmp(current->data.plateNumber, plateNumber) == 0)
        {
            printf("\n[V] SEARCH RESULT: Vehicle %s has successfully PASSED the intersection!\n", plateNumber);
            printf("    Origin Lane: %c | Arrived at Tick: %d | Departed at Tick: %d\n",
                   current->data.dir, current->data.arrivalTime, current->data.departureTime);
            found = 1;
            return;
        }
        current = current->next;
    }

    for (int i = 0; i < 4; i++)
    {
        current = intersection->lanes[i].head;
        int position = 1;
        while (current != NULL)
        {
            if (strcmp(current->data.plateNumber, plateNumber) == 0)
            {
                printf("\n[V] SEARCH RESULT: Vehicle %s is STILL WAITING in the queue.\n", plateNumber);
                printf("    Location: %s Lane | Position in queue: %d | Arrived at Tick: %d\n",
                       laneNames[i], position, current->data.arrivalTime);
                found = 1;
                return;
            }
            current = current->next;
            position++;
        }
    }

    if (!found)
    {
        printf("\n[X] SEARCH RESULT: Vehicle %s was not found in the records of the last run.\n", plateNumber);
    }
}

/*
 * searchVehicleByPlate
 * Tip: does a linear scan through passed vehicles and current queues.
 * - Simple and easy to understand; consider faster indexes for large data.
 */

void swapSortItems(SortItem *a, SortItem *b)
{
    SortItem temp = *a;
    *a = *b;
    *b = temp;
}

/*
 * swapSortItems
 * Tip: swaps two `SortItem` entries; tiny helper for the sort.
 */

int partition(SortItem arr[], int low, int high)
{
    int pivot = arr[high].value;
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++)
    {
        if (arr[j].value >= pivot)
        {
            i++;
            swapSortItems(&arr[i], &arr[j]);
        }
    }
    swapSortItems(&arr[i + 1], &arr[high]);
    return (i + 1);
}

/*
 * partition
 * Tip: partition step for QuickSort (pivot = last element).
 * - Good topic: how pivot choice affects performance.
 */

void quickSortLanes(SortItem arr[], int low, int high)
{
    if (low < high)
    {
        int pi = partition(arr, low, high);
        quickSortLanes(arr, low, pi - 1);
        quickSortLanes(arr, pi + 1, high);
    }
}

/*
 * quickSortLanes
 * Tip: recursive QuickSort on a tiny array (4 items).
 */

void sortAndRankLanes(AdvancedStats stats)
{
    const char *laneNames[4] = {"North", "South", "East", "West"};
    SortItem items[4];
    for (int i = 0; i < 4; i++)
    {
        strcpy_s(items[i].name, sizeof(items[i].name), laneNames[i]);
        items[i].value = stats.finalQueueStatus[i];
    }
    quickSortLanes(items, 0, 3);

    printf("\n===================================\n");
    printf("   LANES RANKING BY FINAL CONGESTION   \n");
    printf("===================================\n");
    for (int i = 0; i < 4; i++)
    {
        int cnt = items[i].value;
        const char *noun = (cnt == 1) ? "vehicle" : "vehicles";
        printf("%d. %s Lane - %d %s still waiting\n", i + 1, items[i].name, cnt, noun);
    }
    printf("===================================\n");
}

/*
 * sortAndRankLanes
 * Tip: build a small array of lane stats and sort to show rankings.
 * - Useful demo of sorting + formatted output for reports.
 */
