#include "simulation.h"

/* =========================================================
   --- פרוטוטייפים של פונקציות פנימיות ---
   ========================================================= */

void runSimulation(int maxTicks, int greenDuration, int animSpeed, int visibleCars, Intersection* intersection, AdvancedStats* stats, Queue* passedList);
void saveStatsToFile(AdvancedStats stats, int vehiclesRemaining);
void printStatsFromLog();
void printEventLog();

/* אלגוריתמי חיפוש ומיון מתקדמים (Quick Sort) */
void searchVehicleByPlate(Intersection* intersection, Queue* passedList, const char* plateNumber);
void swapSortItems(SortItem* a, SortItem* b);
int partition(SortItem arr[], int low, int high);
void quickSortLanes(SortItem arr[], int low, int high);
void sortAndRankLanes(AdvancedStats stats);

/* Event Handlers */
void handleArrivalEvent(Intersection* intersection, int arrivalActive[], int tick, AdvancedStats* stats, FILE* logFile);
void handleDepartureEvent(Intersection* intersection, int tick, AdvancedStats* stats, FILE* logFile, int visibleCars, int animSpeed, Queue* passedList);
void handleLightChangeEvent(Intersection* intersection, int greenDuration, int tick, FILE* logFile);


/* =========================================================
   --- הפונקציה הראשית (main) ---
   ========================================================= */

int main() {
    srand((unsigned int)time(NULL));

    int maxTicks = 20;
    int greenDuration = 4;
    int animSpeed = 800;
    int visibleCars = 5;

    /* יצירת מבני הנתונים ברמת ה-main כדי שישמרו על מצבם גם לאחר סיום הסימולציה */
    Intersection globalIntersection;
    AdvancedStats globalStats = { 0, 0, 0, 0.0, 0, {0, 0, 0, 0}, {0, 0, 0, 0} };
    Queue passedVehiclesList = { NULL, NULL, 0 }; /* רשימה מקושרת לשמירת כל הרכבים שעברו */

    int hasRunSimulation = 0; /* דגל הגנה - מוודא שלא מחפשים/ממיינים לפני שרצה סימולציה */

    /* אתחול ראשוני של מערכת הצומת */
    initIntersection(&globalIntersection);

    /* טעינת הגדרות ראשונית מהקובץ */
    loadConfig(&maxTicks, &greenDuration, &animSpeed, &visibleCars);

    int choice;
    do {
        printf("\n===================================\n");
        printf("    TRAFFIC SIMULATION CONTROL     \n");
        printf("===================================\n");
        printf("1. Start Simulation (From arrivals.txt)\n");
        printf("2. Configure Simulation Settings\n");
        printf("3. Print Last Run Statistics (stats.txt)\n");
        printf("4. Print Event Log (log.txt)\n");
        printf("5. Search Vehicle by Plate Number\n");
        printf("6. Sort and Rank Lanes by Congestion (Quick Sort)\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");

        if (scanf_s("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }
        while (getchar() != '\n');

        switch (choice) {
        case 1:
            /* ניקוי זיכרון של הרצות קודמות לפני שמתחילים סימולציה חדשה */
            freeIntersection(&globalIntersection);
            initIntersection(&globalIntersection);
            while (globalIntersection.lanes[0].count > 0 || passedVehiclesList.count > 0) {
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
        case 2: {
            char inputBuffer[100];
            int tempVal;

            printf("\n--- Current Settings: Ticks=%d, Green Light=%d, Speed=%dms, Visible Cars=%d ---\n",
                maxTicks, greenDuration, animSpeed, visibleCars);

            /* שדה 1: טיקים */
            printf("Enter Total Simulation Ticks [%d] (Press Enter to keep): ", maxTicks);
            if (fgets(inputBuffer, sizeof(inputBuffer), stdin)) {
                if (sscanf_s(inputBuffer, "%d", &tempVal) == 1) {
                    maxTicks = tempVal;
                }
            }

            /* שדה 2: משך אור ירוק */
            printf("Enter Green Light Duration [%d] (Press Enter to keep): ", greenDuration);
            if (fgets(inputBuffer, sizeof(inputBuffer), stdin)) {
                if (sscanf_s(inputBuffer, "%d", &tempVal) == 1) {
                    greenDuration = tempVal;
                }
            }

            /* שדה 3: מהירות אנימציה */
            printf("Enter Animation Speed in ms [%d] (Press Enter to keep): ", animSpeed);
            if (fgets(inputBuffer, sizeof(inputBuffer), stdin)) {
                if (sscanf_s(inputBuffer, "%d", &tempVal) == 1) {
                    animSpeed = tempVal;
                }
            }

            /* שדה 4: כמות רכבים גלויה - עם לולאת בדיקת תקינות אי-זוגית */
            while (1) {
                printf("Enter Max Visible Cars Per Lane (Must be ODD) [%d] (Press Enter to keep): ", visibleCars);
                if (fgets(inputBuffer, sizeof(inputBuffer), stdin)) {

                    /* זיהוי לחיצה על Enter בלבד */
                    if (inputBuffer[0] == '\n' || inputBuffer[0] == '\r') {
                        break; /* יוצאים מהלולאה ושומרים על הערך הקיים */
                    }

                    if (sscanf_s(inputBuffer, "%d", &tempVal) != 1) {
                        printf("[X] Error: Invalid input. Please enter an integer only!\n");
                        continue;
                    }
                    if (tempVal < 1) {
                        printf("[X] Error: Number of cars must be greater than 0. Try again.\n");
                        continue;
                    }
                    if (tempVal % 2 == 0) {
                        printf("[X] Error: You entered an EVEN number! Size must be ODD to maintain symmetry. Try again.\n");
                        continue;
                    }

                    /* אם הכל תקין, מעדכנים את המשתנה ויוצאים מהלולאה */
                    visibleCars = tempVal;
                    break;
                }
            }

            if (saveConfig(maxTicks, greenDuration, animSpeed, visibleCars)) {
                printf("[V] Settings saved successfully to config.txt!\n");
            }
            else {
                printf("[X] Error saving settings.\n");
            }
            break;
        }
        case 3:
            printStatsFromLog();
            break;
        case 4:
            printEventLog();
            break;
        case 5:
            if (!hasRunSimulation) {
                printf("[!] Please run the simulation (Option 1) first to populate vehicle logs.\n");
            }
            else {
                char targetPlate[20];
                printf("Enter vehicle plate number to search (e.g., N-1234, W-5678): ");
                scanf_s("%s", targetPlate, (unsigned int)sizeof(targetPlate));
                searchVehicleByPlate(&globalIntersection, &passedVehiclesList, targetPlate);
            }
            break;
        case 6:
            if (!hasRunSimulation) {
                printf("[!] Please run the simulation (Option 1) first to analyze data.\n");
            }
            else {
                sortAndRankLanes(globalStats);
            }
            break;
        case 7:
            printf("Cleaning up memory and exiting system. Goodbye!\n");
            freeIntersection(&globalIntersection);
            /* ניקוי רשימת הרכבים שעברו מהזיכרון */
            struct Node* current = passedVehiclesList.head;
            while (current != NULL) {
                struct Node* next = current->next;
                free(current);
                current = next;
            }
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 7);

    return 0;
}


/* =========================================================
   --- אלגוריתמים: חיפוש ומיון ---
   ========================================================= */

   /**
    * פונקציה: searchVehicleByPlate (אלגוריתם חיפוש ליניארי)
    */
void searchVehicleByPlate(Intersection* intersection, Queue* passedList, const char* plateNumber) {
    const char* laneNames[4] = { "North", "South", "East", "West" };
    int found = 0;

    /* סריקה 1: בדיקה ברשימת הרכבים שכבר חצו ועברו את הצומת */
    Node* current = passedList->head;
    while (current != NULL) {
        if (strcmp(current->data.plateNumber, plateNumber) == 0) {
            printf("\n[V] SEARCH RESULT: Vehicle %s has successfully PASSED the intersection!\n", plateNumber);
            /* כאן הוספנו את ההדפסה של ה- Departed at Tick */
            printf("    Origin Lane: %c | Arrived at Tick: %d | Departed at Tick: %d\n",
                current->data.dir, current->data.arrivalTime, current->data.departureTime);
            found = 1;
            return;
        }
        current = current->next;
    }

    /* סריקה 2: בדיקה האם הרכב עדיין ממתין באחד מארבעת התורים בצומת */
    for (int i = 0; i < 4; i++) {
        current = intersection->lanes[i].head;
        int position = 1;
        while (current != NULL) {
            if (strcmp(current->data.plateNumber, plateNumber) == 0) {
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

    if (!found) {
        printf("\n[X] SEARCH RESULT: Vehicle %s was not found in the records of the last run.\n", plateNumber);
    }
}
/**
 * פונקציית עזר: swapSortItems
 * תפקיד: החלפת מיקומים של שני תאים במערך לטובת המיון.
 */
void swapSortItems(SortItem* a, SortItem* b) {
    SortItem temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * פונקציית עזר: partition
 * תפקיד: חלוקת המערך סביב איבר ציר (Pivot) לטובת Quick Sort (מיון יורד).
 */
int partition(SortItem arr[], int low, int high) {
    int pivot = arr[high].value;
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        /* מיון יורד - הערכים הגדולים בהתחלה */
        if (arr[j].value >= pivot) {
            i++;
            swapSortItems(&arr[i], &arr[j]);
        }
    }
    swapSortItems(&arr[i + 1], &arr[high]);
    return (i + 1);
}

/**
 * פונקציה: quickSortLanes
 * תפקיד: אלגוריתם מיון מהיר רקורסיבי O(N log N)
 */
void quickSortLanes(SortItem arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);

        quickSortLanes(arr, low, pi - 1);
        quickSortLanes(arr, pi + 1, high);
    }
}

/**
 * פונקציה: sortAndRankLanes
 * תפקיד: הכנת הנתונים, קריאה ל-Quick Sort, והדפסת הפלט המדורג.
 */
void sortAndRankLanes(AdvancedStats stats) {
    const char* laneNames[4] = { "North", "South", "East", "West" };
    SortItem items[4];

    /* העתקת הנתונים למערך המיון */
    for (int i = 0; i < 4; i++) {
        strcpy_s(items[i].name, sizeof(items[i].name), laneNames[i]);
        items[i].value = stats.finalQueueStatus[i];
    }

    /* הפעלת המיון המהיר (Quick Sort) על 4 הנתיבים */
    quickSortLanes(items, 0, 3);

    /* הדפסת הדירוג הממוין */
    printf("\n===================================\n");
    printf("   LANES RANKING BY FINAL CONGESTION   \n");
    printf("===================================\n");
    for (int i = 0; i < 4; i++) {
        printf("%d. %s Lane - %d vehicles still waiting\n", i + 1, items[i].name, items[i].value);
    }
    printf("===================================\n");
}


/* =========================================================
   --- פונקציות ניהול קבצים ויומנים ---
   ========================================================= */

int loadConfig(int* maxTicks, int* greenDuration, int* animSpeed, int* visibleCars) {
    FILE* f;
    if (fopen_s(&f, "config.txt", "r") != 0 || f == NULL) {
        return 0;
    }
    char line[50];
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "MAX_TICKS=", 10) == 0) {
            *maxTicks = atoi(line + 10);
        }
        else if (strncmp(line, "GREEN_DURATION=", 15) == 0) {
            *greenDuration = atoi(line + 15);
        }
        else if (strncmp(line, "ANIMATION_SPEED=", 16) == 0) {
            *animSpeed = atoi(line + 16);
        }
        else if (strncmp(line, "VISIBLE_CARS=", 13) == 0) {
            *visibleCars = atoi(line + 13);
        }
    }
    fclose(f);
    return 1;
}

int saveConfig(int maxTicks, int greenDuration, int animSpeed, int visibleCars) {
    FILE* f;
    if (fopen_s(&f, "config.txt", "w") != 0 || f == NULL) {
        return 0;
    }
    fprintf(f, "MAX_TICKS=%d\n", maxTicks);
    fprintf(f, "GREEN_DURATION=%d\n", greenDuration);
    fprintf(f, "ANIMATION_SPEED=%d\n", animSpeed);
    fprintf(f, "VISIBLE_CARS=%d\n", visibleCars);
    fclose(f);
    return 1;
}

void saveStatsToFile(AdvancedStats stats, int vehiclesRemaining) {
    FILE* f;
    if (fopen_s(&f, "stats.txt", "w") != 0 || f == NULL) {
        printf("Error writing statistics to file.\n");
        return;
    }

    const char* laneNames[4] = { "North", "South", "East", "West" };
    int busiestLaneIdx = 0;
    for (int i = 1; i < 4; i++) {
        if (stats.peakVehicleCount[i] > stats.peakVehicleCount[busiestLaneIdx]) {
            busiestLaneIdx = i;
        }
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

void printStatsFromLog() {
    FILE* f;
    if (fopen_s(&f, "stats.txt", "r") != 0 || f == NULL) {
        printf("\n[!] No statistics found. Run the simulation first.\n");
        return;
    }
    char ch;
    printf("\n===================================================\n");
    while ((ch = fgetc(f)) != EOF) {
        putchar(ch);
    }
    printf("===================================================\n");
    fclose(f);
}

void printEventLog() {
    FILE* f;
    if (fopen_s(&f, "log.txt", "r") != 0 || f == NULL) {
        printf("\n[!] No event log found. Run the simulation first.\n");
        return;
    }
    char line[256];
    printf("\n--- PRINTING EVENT LOG (log.txt) ---\n");
    while (fgets(line, sizeof(line), f)) {
        printf("%s", line);
    }
    printf("------------------------------------\n");
    fclose(f);
}


/* =========================================================
   --- מטפלי אירועים (Event Handlers) ---
   ========================================================= */
void handleArrivalEvent(Intersection* intersection, int arrivalActive[], int tick, AdvancedStats* stats, FILE* logFile) {
    Direction allDirs[4] = { NORTH, SOUTH, EAST, WEST };
    int anyArrivalsThisTick = 0; /* דגל שיעזור לנו לדעת אם הגיעו רכבים בכלל */

    for (int i = 0; i < 4; i++) {
        if (arrivalActive[i] == 1) {
            Vehicle v;
            sprintf_s(v.plateNumber, sizeof(v.plateNumber), "%c-%04d", allDirs[i], rand() % 10000);
            v.arrivalTime = tick;
            v.departureTime = 0; /* אתחול ל-0 כי הרכב עדיין לא יצא מהצומת */
            v.dir = allDirs[i];

            enqueue(&intersection->lanes[i], v);
            stats->totalVehiclesGenerated++;
            anyArrivalsThisTick = 1;

            /* --- התוספת החסרה: הדפסה חיה למסך בזמן האנימציה --- */
            printf("[+] Vehicle %s arrived at %c lane.\n", v.plateNumber, v.dir);

            if (logFile != NULL) {
                fprintf(logFile, "[TICK %02d] ARRIVAL  : Vehicle %s joined %c lane.\n", tick, v.plateNumber, v.dir);
            }
        }

        int currentCount = intersection->lanes[i].count;
        if (currentCount > stats->peakVehicleCount[i]) {
            stats->peakVehicleCount[i] = currentCount;
        }
        if (currentCount > stats->maxQueueLength) {
            stats->maxQueueLength = currentCount;
        }
    }

    /* אם אף רכב לא הגיע באף נתיב בטיק הזה, נדפיס הודעה מסודרת */
    if (anyArrivalsThisTick == 0) {
        printf("[i] No new vehicles arrived in this tick.\n");
    }
}

void handleDepartureEvent(Intersection* intersection, int tick, AdvancedStats* stats, FILE* logFile, int visibleCars, int animSpeed, Queue* passedList) {
    int greenIdx = getLaneIndex(intersection->currentGreen);
    Vehicle departingVehicle;

    if (dequeue(&intersection->lanes[greenIdx], &departingVehicle)) {
        /* מתעדים את זמן העזיבה בתוך הרכב ברגע שהוא יוצא מהתור */
        departingVehicle.departureTime = tick;

        printf("\n\n=== Traffic Simulation (Animation Mode) ===\n");
        printf("--- TICK %d/%d (Departure Phase) ---\n", tick, tick);
        printf("[!] Vehicle %s passed from %c. (Waited: %d ticks)\n",
            departingVehicle.plateNumber, intersection->currentGreen, (tick - departingVehicle.arrivalTime));

        stats->totalVehiclesPassed++;
        stats->totalWaitTime += (tick - departingVehicle.arrivalTime);

        if (logFile != NULL) {
            fprintf(logFile, "[TICK %02d] DEPARTURE: Vehicle %s passed from %c (Waited %d ticks).\n",
                tick, departingVehicle.plateNumber, intersection->currentGreen, (tick - departingVehicle.arrivalTime));
        }

        /* שומרים את הרכב שיצא (כולל שעת העזיבה המעודכנת שלו) ברשימת ההיסטוריה */
        enqueue(passedList, departingVehicle);

        drawVisualIntersection(intersection, visibleCars);
        Sleep(animSpeed);
    }
}

void handleLightChangeEvent(Intersection* intersection, int greenDuration, int tick, FILE* logFile) {
    Direction allDirs[4] = { NORTH, SOUTH, EAST, WEST };
    intersection->greenTicksLeft--;

    if (intersection->greenTicksLeft <= 0) {
        Direction newDirection;
        do {
            newDirection = allDirs[rand() % 4];
        } while (newDirection == intersection->currentGreen);

        intersection->currentGreen = newDirection;
        intersection->greenTicksLeft = greenDuration;

        if (logFile != NULL) {
            fprintf(logFile, "[TICK %02d] LIGHT    : Green light changed to %c lane.\n", tick, newDirection);
        }
    }
}


/* =========================================================
   --- לולאת הסימולציה הראשית (DES Engine) ---
   ========================================================= */

void runSimulation(int maxTicks, int greenDuration, int animSpeed, int visibleCars, Intersection* intersection, AdvancedStats* stats, Queue* passedList) {
    /* איפוס סטטיסטיקה להרצה החדשה */
    stats->totalVehiclesGenerated = 0;
    stats->totalVehiclesPassed = 0;
    stats->totalWaitTime = 0;
    stats->avgWaitTime = 0.0;
    stats->maxQueueLength = 0;
    for (int i = 0; i < 4; i++) {
        stats->peakVehicleCount[i] = 0;
        stats->finalQueueStatus[i] = 0;
    }

    intersection->greenTicksLeft = greenDuration;

    FILE* arrivalsFile = NULL;
    fopen_s(&arrivalsFile, "arrivals.txt", "r");
    if (arrivalsFile == NULL) {
        printf("[!] arrivals.txt not found. Creating a sample simulation file...\n");
        fopen_s(&arrivalsFile, "arrivals.txt", "w");
        if (arrivalsFile != NULL) {
            for (int t = 0; t < maxTicks; t++) {
                fprintf(arrivalsFile, "%d %d %d %d\n", rand() % 2, rand() % 2, rand() % 2, rand() % 2);
            }
            fclose(arrivalsFile);
            fopen_s(&arrivalsFile, "arrivals.txt", "r");
        }
    }

    FILE* logFile = NULL;
    fopen_s(&logFile, "log.txt", "w");
    if (logFile != NULL) {
        fprintf(logFile, "=========================================\n");
        fprintf(logFile, "   TRAFFIC SIMULATION EVENT LOG (DES)    \n");
        fprintf(logFile, "=========================================\n");
        fprintf(logFile, "Max Ticks: %d | Green Duration: %d\n\n", maxTicks, greenDuration);
    }

    for (int tick = 1; tick <= maxTicks; tick++) {
        printf("\n\n=== Traffic Simulation (Animation Mode) ===\n");
        printf("--- TICK %d/%d (Arrival Phase) ---\n", tick, maxTicks);

        int arrivalActive[4] = { 0, 0, 0, 0 };
        if (arrivalsFile != NULL) {
            char line[100];
            if (fgets(line, sizeof(line), arrivalsFile)) {
                sscanf_s(line, "%d %d %d %d", &arrivalActive[0], &arrivalActive[1], &arrivalActive[2], &arrivalActive[3]);
            }
        }

        /* 1. אירוע הגעת רכבים */
        handleArrivalEvent(intersection, arrivalActive, tick, stats, logFile);

        drawVisualIntersection(intersection, visibleCars);
        Sleep(animSpeed);

        /* 2. אירוע עזיבת רכב (ומעבר לרשימת שעזבו) */
        handleDepartureEvent(intersection, tick, stats, logFile, visibleCars, animSpeed, passedList);

        /* 3. אירוע החלפת רמזור */
        handleLightChangeEvent(intersection, greenDuration, tick, logFile);
    }

    if (arrivalsFile != NULL) {
        fclose(arrivalsFile);
    }

    if (logFile != NULL) {
        fprintf(logFile, "\n=== SIMULATION ENDED ===\n");
        fclose(logFile);
    }

    /* איסוף נתונים מסכמים ועדכון מבנה הסטטיסטיקה */
    int vehiclesRemaining = 0;
    for (int i = 0; i < 4; i++) {
        stats->finalQueueStatus[i] = intersection->lanes[i].count;
        vehiclesRemaining += intersection->lanes[i].count;
    }

    if (stats->totalVehiclesPassed > 0) {
        stats->avgWaitTime = (double)stats->totalWaitTime / stats->totalVehiclesPassed;
    }

    const char* laneNames[4] = { "North", "South", "East", "West" };
    int busiestLaneIdx = 0;
    for (int i = 1; i < 4; i++) {
        if (stats->peakVehicleCount[i] > stats->peakVehicleCount[busiestLaneIdx]) {
            busiestLaneIdx = i;
        }
    }

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

    /* שמירת הנתונים לקובץ הסטטיסטיקה */
    saveStatsToFile(*stats, vehiclesRemaining);

    printf("\n[!] Event log saved successfully to log.txt\n");
    printf("Press Enter to return to menu...");
    getchar();
}


/* =========================================================
   --- מימושי הפונקציות של מבני הנתונים ותורים ---
   ========================================================= */

void initIntersection(Intersection* intersection) {
    for (int i = 0; i < 4; i++) {
        intersection->lanes[i].head = NULL;
        intersection->lanes[i].tail = NULL;
        intersection->lanes[i].count = 0;
    }
    intersection->currentGreen = NORTH;
}

int getLaneIndex(Direction dir) {
    switch (dir) {
    case NORTH: return 0;
    case SOUTH: return 1;
    case EAST:  return 2;
    case WEST:  return 3;
    default:    return -1;
    }
}

void enqueue(Queue* q, Vehicle v) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed in enqueue!\n");
        return;
    }
    newNode->data = v;
    newNode->next = NULL;
    if (q->count == 0) {
        newNode->prev = NULL;
        q->head = newNode;
        q->tail = newNode;
    }
    else {
        newNode->prev = q->tail;
        q->tail->next = newNode;
        q->tail = newNode;
    }
    q->count++;
}

int dequeue(Queue* q, Vehicle* outVehicle) {
    if (q->count == 0 || q->head == NULL) return 0;
    Node* nodeToRemove = q->head;
    *outVehicle = nodeToRemove->data;
    q->head = nodeToRemove->next;
    if (q->head == NULL) q->tail = NULL;
    else q->head->prev = NULL;
    free(nodeToRemove);
    q->count--;
    return 1;
}

void freeIntersection(Intersection* intersection) {
    Vehicle dummy;
    for (int i = 0; i < 4; i++) {
        while (dequeue(&intersection->lanes[i], &dummy));
    }
}

void drawVisualIntersection(Intersection* intersection, int visibleCars) {
    int n_count = intersection->lanes[getLaneIndex(NORTH)].count;
    int s_count = intersection->lanes[getLaneIndex(SOUTH)].count;
    int e_count = intersection->lanes[getLaneIndex(EAST)].count;
    int w_count = intersection->lanes[getLaneIndex(WEST)].count;

    const char* GREEN = "\x1b[32m";
    const char* RED = "\x1b[31m";
    const char* YELLOW = "\x1b[33m";
    const char* RESET = "\x1b[0m";

    const char* n_color = (intersection->currentGreen == NORTH) ? GREEN : RED;
    const char* s_color = (intersection->currentGreen == SOUTH) ? GREEN : RED;
    const char* e_color = (intersection->currentGreen == EAST) ? GREEN : RED;
    const char* w_color = (intersection->currentGreen == WEST) ? GREEN : RED;

    char n_light = (intersection->currentGreen == NORTH) ? 'G' : 'R';
    char s_light = (intersection->currentGreen == SOUTH) ? 'G' : 'R';
    char e_light = (intersection->currentGreen == EAST) ? 'G' : 'R';
    char w_light = (intersection->currentGreen == WEST) ? 'G' : 'R';

    int rows = (2 * visibleCars) + 3;
    int cols = (4 * visibleCars) + 5;

    int center_r = visibleCars + 1;
    int center_c = (visibleCars * 2) + 2;

    char** matrix = (char**)malloc(rows * sizeof(char*));
    if (matrix == NULL) return;

    for (int i = 0; i < rows; i++) {
        matrix[i] = (char*)malloc(cols * sizeof(char));
        if (matrix[i] == NULL) {
            for (int k = 0; k < i; k++) free(matrix[k]);
            free(matrix);
            return;
        }
        for (int j = 0; j < cols; j++) matrix[i][j] = ' ';
    }

    for (int r = 0; r < rows; r++) {
        if (r < center_r - 1 || r > center_r + 1) {
            matrix[r][center_c - 2] = '|';
            matrix[r][center_c + 2] = '|';
        }
    }
    for (int c = 0; c < cols; c++) {
        if (c < center_c - 2 || c > center_c + 2) {
            matrix[center_r - 1][c] = '-';
            matrix[center_r + 1][c] = '-';
        }
    }

    for (int i = 0; i < n_count && i < visibleCars; i++) matrix[center_r - 2 - i][center_c] = '#';
    for (int i = 0; i < s_count && i < visibleCars; i++) matrix[center_r + 2 + i][center_c] = '#';
    for (int i = 0; i < w_count && i < visibleCars; i++) matrix[center_r][center_c - 3 - (i * 2)] = '#';
    for (int i = 0; i < e_count && i < visibleCars; i++) matrix[center_r][center_c + 3 + (i * 2)] = '#';

    printf("\n--- VISUAL MAP (Capacity: %d Cars/Lane) ---\n", visibleCars);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (matrix[i][j] == '#') printf("%s#%s", YELLOW, RESET);
            else printf("%c", matrix[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);

    printf("\n--- DASHBOARD --- \n");
    printf("            |       |\n");
    printf("            |   N   |\n");
    printf("            | %s%c%s[%02d] |\n", n_color, n_light, RESET, n_count);
    printf("            |   v   |\n");
    printf("    ========+=======+========\n");
    printf("    W %s%c%s[%02d]> |       | <[%02d]%s%c%s E\n", w_color, w_light, RESET, w_count, e_count, e_color, e_light, RESET);
    printf("    ========+=======+========\n");
    printf("            |   ^   |\n");
    printf("            | %s%c%s[%02d] |\n", s_color, s_light, RESET, s_count);
    printf("            |   S   |\n");
    printf("            |       |\n");
}