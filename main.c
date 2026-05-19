#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h> 
/* --- Data Structures Definitions --- */

typedef enum {
    NORTH = 'N',
    SOUTH = 'S',
    EAST = 'E',
    WEST = 'W'
} Direction;

typedef struct {
    char plateNumber[10];
    int arrivalTime;
    Direction dir;
} Vehicle;

/* Doubly Linked List Node */
typedef struct Node {
    Vehicle data;
    struct Node* prev;
    struct Node* next;
} Node;

/* Queue Wrapper for each Lane */
typedef struct {
    Node* head;
    Node* tail;
    int count;
} Queue;

typedef struct {
    Queue lanes[4];         /* 0: North, 1: South, 2: East, 3: West */
    Direction currentGreen; /* Current direction with green light */
    int greenTicksLeft;
} Intersection;


/* --- Function Prototypes --- */
void initIntersection(Intersection* intersection);
void enqueue(Queue* q, Vehicle v);
int dequeue(Queue* q, Vehicle* outVehicle);
int getLaneIndex(Direction dir);
Direction getNextDirection(Direction current);
void freeIntersection(Intersection* intersection);
void printIntersectionState(Intersection* intersection, int currentTick);
void drawVisualIntersection(Intersection* intersection);

/* --- Main Simulation --- */

int main() {
    /* Initialize random seed */
    srand((unsigned int)time(NULL));

    Intersection myIntersection;
    initIntersection(&myIntersection);

    int maxTicks = 20;
    int GREEN_DURATION = 4;
    int ANIMATION_SPEED = 800; /* Delay in milliseconds (800ms = 0.8 seconds) */

    for (int tick = 1; tick <= maxTicks; tick++) {

        /* ניקוי המסך ליצירת אפקט האנימציה */
        printf("=== Traffic Simulation (Animation Mode) ===\n");
        printf("--- TICK %d (Arrival Phase) ---\n", tick);

        /* 1. הגרלת הגעות רכבים חדשים */
        Direction allDirs[4] = { NORTH, SOUTH, EAST, WEST };
        for (int i = 0; i < 4; i++) {
            if (rand() % 100 < 30) {
                Vehicle v;
                sprintf_s(v.plateNumber, sizeof(v.plateNumber), "%c-%04d", allDirs[i], rand() % 10000);
                v.arrivalTime = tick;
                v.dir = allDirs[i];
                enqueue(&myIntersection.lanes[i], v);
            }
        }

        /* ציור הצומת מיד לאחר ההגעות - מאפשר לראות את הרכבים החדשים מצטרפים לתור */
        drawVisualIntersection(&myIntersection);

        /* הקפאת המסך כדי שהמשתמש יראה את המצב */
        Sleep(ANIMATION_SPEED);

        /* =========================================
           FRAME 2: שלב העזיבה (Departure Phase)
           ========================================= */
        int greenIdx = getLaneIndex(myIntersection.currentGreen);
        Vehicle departingVehicle;

        /* נבדוק אם יש רכב בנתיב הירוק שאפשר להוציא מהצומת */
        if (dequeue(&myIntersection.lanes[greenIdx], &departingVehicle)) {

            /* ניקוי מסך לפרייים השני */
            printf("=== Traffic Simulation (Animation Mode) ===\n");
            printf("--- TICK %d (Departure Phase) ---\n", tick);
            printf("[!] Vehicle %s passed from %c.\n", departingVehicle.plateNumber, myIntersection.currentGreen);

            /* ציור הצומת מחדש לאחר שהרכב עזב את הרשימה המקושרת - נראה אותו נעלם! */
            drawVisualIntersection(&myIntersection);

            /* השהיה נוספת כדי לראות את הצומת הריק/המעודכן */
            Sleep(ANIMATION_SPEED);
        }

        /* 3. עדכון הרמזורים לקראת הפעימה הבאה */
        myIntersection.greenTicksLeft--;
        if (myIntersection.greenTicksLeft <= 0) {
            Direction newDirection;

            /* הגרל כיוון חדש שוב ושוב, עד שהוא שונה מהכיוון הנוכחי */
            do {
                newDirection = allDirs[rand() % 4];
            } while (newDirection == myIntersection.currentGreen);

            myIntersection.currentGreen = newDirection;
            myIntersection.greenTicksLeft = GREEN_DURATION;
        }
    }

    /* סיום הסימולציה */

    printf("\n=== Simulation Ended ===\n");
    freeIntersection(&myIntersection);

    return 0;
}
/* --- Function Implementations --- */

void initIntersection(Intersection* intersection) {
    for (int i = 0; i < 4; i++) {
        intersection->lanes[i].head = NULL;
        intersection->lanes[i].tail = NULL;
        intersection->lanes[i].count = 0;
    }
    intersection->currentGreen = NORTH;
    intersection->greenTicksLeft = 4;
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

Direction getNextDirection(Direction current) {
    switch (current) {
    case NORTH: return EAST;
    case EAST:  return SOUTH;
    case SOUTH: return WEST;
    case WEST:  return NORTH;
    default:    return NORTH;
    }
}

void enqueue(Queue* q, Vehicle v) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Error: Memory allocation failed!\n");
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
    if (q->count == 0 || q->head == NULL) {
        return 0;
    }

    Node* nodeToRemove = q->head;
    *outVehicle = nodeToRemove->data;
    q->head = nodeToRemove->next;

    if (q->head == NULL) {
        q->tail = NULL;
    }
    else {
        q->head->prev = NULL;
    }

    free(nodeToRemove);
    q->count--;

    return 1;
}

void freeIntersection(Intersection* intersection) {
    Vehicle dummy;
    for (int i = 0; i < 4; i++) {
        while (dequeue(&intersection->lanes[i], &dummy)) {
            /* Dequeue takes care of freeing the memory */
        }
    }
    printf("Memory cleanup completed successfully.\n");
}

void printIntersectionState(Intersection* intersection, int currentTick) {
    printf("State: Green=%c | Queue counts: N:%d S:%d E:%d W:%d\n",
        intersection->currentGreen,
        intersection->lanes[getLaneIndex(NORTH)].count,
        intersection->lanes[getLaneIndex(SOUTH)].count,
        intersection->lanes[getLaneIndex(EAST)].count,
        intersection->lanes[getLaneIndex(WEST)].count);
}


void drawVisualIntersection(Intersection* intersection) {
    /* 1. שליפת הנתונים (מהקוד המקורי שלכם) */
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


    /* =========================================================
       חלק א': מטריצה דינמית - ציור מבט-על של הצומת והרכבים
       (עונה על דרישת החובה לפרויקט של מטריצות דינמיות)
       ========================================================= */
    int SIZE = 11;
    char** matrix = (char**)malloc(SIZE * sizeof(char*));
    for (int i = 0; i < SIZE; i++) {
        matrix[i] = (char*)malloc(SIZE * sizeof(char));
        for (int j = 0; j < SIZE; j++) {
            matrix[i][j] = ' '; /* אתחול הלוח ברווחים */
        }
    }

    /* שרטוט גבולות הכביש */
    for (int i = 0; i < SIZE; i++) {
        matrix[i][4] = '|'; matrix[i][6] = '|'; /* נתיבי צפון-דרום */
        matrix[4][i] = '-'; matrix[6][i] = '-'; /* נתיבי מזרח-מערב */
    }
    /* ניקוי מרכז הצומת */
    for (int i = 4; i <= 6; i++) {
        for (int j = 4; j <= 6; j++) matrix[i][j] = ' ';
    }

    /* הצבת הרכבים במטריצה (עד 4 רכבים ויזואלית לכל כיוון כדי לא לחרוג מהגודל) */
    for (int i = 0; i < n_count && i < 4; i++) matrix[3 - i][5] = '#';
    for (int i = 0; i < s_count && i < 4; i++) matrix[7 + i][5] = '#';
    for (int i = 0; i < e_count && i < 4; i++) matrix[5][7 + i] = '#';
    for (int i = 0; i < w_count && i < 4; i++) matrix[5][3 - i] = '#';

    /* הדפסת המטריצה הדינמית למסך */
    printf("\n--- VISUAL MAP ---\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (matrix[i][j] == '#') {
                printf("%s#%s", YELLOW, RESET); /* נצבע את הרכבים בצהוב */
            }
            else {
                printf("%c", matrix[i][j]);
            }
        }
        printf("\n");
    }

    /* שחרור הזיכרון של המטריצה הדינמית (חובה!) */
    for (int i = 0; i < SIZE; i++) {
        free(matrix[i]);
    }
    free(matrix);


    /* =========================================================
       חלק ב': לוח הבקרה המפורט (הקוד המקורי שלכם)
       ========================================================= */
    printf("\n--- DASHBOARD ---");
    printf("\n");
    printf("            |       |\n");
    printf("            |   N   |\n");
    printf("            | %s%c%s[%02d] |\n", n_color, n_light, RESET, n_count);
    printf("            |   v   |\n");
    printf("    ========+=======+========\n");
    printf("    W %s%c%s[%02d]> |       | <[%02d]%s%c%s E\n",
        w_color, w_light, RESET, w_count,
        e_count, e_color, e_light, RESET);
    printf("    ========+=======+========\n");
    printf("            |   ^   |\n");
    printf("            | %s%c%s[%02d] |\n", s_color, s_light, RESET, s_count);
    printf("            |   S   |\n");
    printf("            |       |\n");
    printf("\n");
}