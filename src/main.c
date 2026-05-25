/*
 * main.c
 *
 * Notes:
 * - Entry point and simple text UI for the simulation.
 * - Demonstrates reading configuration, menu-driven interaction, and calling
 *   into the engine (`runSimulation`).
 */

#include "datastructs.h"
#include "io.h"
#include "engine.h"
#include "algos.h"

int main()
{
    srand(1u);

    int maxTicks = 20;
    int greenDuration = 4;
    int animSpeed = 800;
    int visibleCars = 5;

    Intersection globalIntersection;
    AdvancedStats globalStats = {0, 0, 0, 0.0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}};
    Queue passedVehiclesList = {NULL, NULL, 0};

    int hasRunSimulation = 0;
    initIntersection(&globalIntersection);
    loadConfig(&maxTicks, &greenDuration, &animSpeed, &visibleCars);

    int choice;
    do
    {
        printf("\n===================================\n");
        printf("    TRAFFIC SIMULATION CONTROL     \n");
        printf("===================================\n");
        printf("1. Start Simulation (Via arrivals.txt)\n");
        printf("2. Configure Simulation Settings (Via config.txt)\n");
        printf("3. Print Last Run Statistics (Via stats.txt)\n");
        printf("4. Print Event Log (Via log.txt)\n");
        printf("5. Search Vehicle by Plate Number\n");
        printf("6. Sort and Rank Lanes by Congestion\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");

        if (scanf_s("%d", &choice) != 1)
        {
            printf("Invalid input. Please enter a number.\n");
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF)
                ;
            continue;
        }
        while (getchar() != '\n')
            ;

        switch (choice)
        {
        case 1:
            freeIntersection(&globalIntersection);
            initIntersection(&globalIntersection);
            while (globalIntersection.lanes[0].count > 0 || passedVehiclesList.count > 0)
            {
                Vehicle dummy;
                dequeue(&passedVehiclesList, &dummy);
            }
            passedVehiclesList.head = NULL;
            passedVehiclesList.tail = NULL;
            passedVehiclesList.count = 0;
            loadConfig(&maxTicks, &greenDuration, &animSpeed, &visibleCars);
            runSimulation(maxTicks, greenDuration, animSpeed, visibleCars, &globalIntersection, &globalStats, &passedVehiclesList);
            hasRunSimulation = 1;
            break;
        case 2:
        {
            char inputBuffer[100];
            int tempVal;
            printf("\n--- Current Settings: Ticks=%d, Green Light=%d, Speed=%dms, Visible Cars=%d ---\n",
                   maxTicks, greenDuration, animSpeed, visibleCars);
            printf("Enter Total Simulation Ticks [%d] (Press Enter to keep): ", maxTicks);
            if (fgets(inputBuffer, sizeof(inputBuffer), stdin))
            {
                if (sscanf_s(inputBuffer, "%d", &tempVal) == 1)
                    maxTicks = tempVal;
            }
            printf("Enter Green Light Duration [%d] (Press Enter to keep): ", greenDuration);
            if (fgets(inputBuffer, sizeof(inputBuffer), stdin))
            {
                if (sscanf_s(inputBuffer, "%d", &tempVal) == 1)
                    greenDuration = tempVal;
            }
            printf("Enter Animation Speed in ms [%d] (Press Enter to keep): ", animSpeed);
            if (fgets(inputBuffer, sizeof(inputBuffer), stdin))
            {
                if (sscanf_s(inputBuffer, "%d", &tempVal) == 1)
                    animSpeed = tempVal;
            }
            while (1)
            {
                printf("Enter Max Visible Cars Per Lane (Must be ODD) [%d] (Press Enter to keep): ", visibleCars);
                if (fgets(inputBuffer, sizeof(inputBuffer), stdin))
                {
                    if (inputBuffer[0] == '\n' || inputBuffer[0] == '\r')
                        break;
                    if (sscanf_s(inputBuffer, "%d", &tempVal) != 1)
                    {
                        printf("[X] Error: Invalid input. Please enter an integer only!\n");
                        continue;
                    }
                    if (tempVal < 1)
                    {
                        printf("[X] Error: Number of cars must be greater than 0. Try again.\n");
                        continue;
                    }
                    if (tempVal % 2 == 0)
                    {
                        printf("[X] Error: You entered an EVEN number! Size must be ODD to maintain symmetry. Try again.\n");
                        continue;
                    }
                    visibleCars = tempVal;
                    break;
                }
            }
            if (saveConfig(maxTicks, greenDuration, animSpeed, visibleCars))
                printf("[V] Settings saved successfully to config.txt!\n");
            else
                printf("[X] Error saving settings.\n");
            break;
        }
        case 3:
            printStatsFromLog();
            break;
        case 4:
            printEventLog();
            break;
        case 5:
            if (!hasRunSimulation)
                printf("[!] Please run the simulation (Option 1) first to populate vehicle logs.\n");
            else
            {
                char targetPlate[20];
                printf("Enter vehicle plate number to search (e.g., N-1234, W-5678): ");
                scanf_s("%s", targetPlate, (unsigned int)sizeof(targetPlate));
                searchVehicleByPlate(&globalIntersection, &passedVehiclesList, targetPlate);
            }
            break;
        case 6:
            if (!hasRunSimulation)
                printf("[!] Please run the simulation (Option 1) first to analyze data.\n");
            else
                sortAndRankLanes(globalStats);
            break;
        case 0:
            printf("Cleaning up memory and exiting system. Goodbye!\n");
            freeIntersection(&globalIntersection);
            struct Node *current = passedVehiclesList.head;
            while (current != NULL)
            {
                struct Node *next = current->next;
                free(current);
                current = next;
            }
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);
    return 0;
}

/*
 * main
 * Tip: program entry and menu UI.
 * - Use the menu to run simulations, tweak settings, and inspect logs.
 * - Note how `globalIntersection` and `AdvancedStats` are passed by pointer to avoid copies.
 */
