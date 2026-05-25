#include "datastructs.h"

/*
 * datastructs.c
 *
 * Notes:
 * - Review `initIntersection`, `enqueue`, `dequeue` to learn pointer manipulation
 *   and memory management (malloc/free) for linked lists.
 * - Visualization: `drawVisualIntersection` is a good exercise in mapping
 *   logical lane counts to an ASCII grid and practicing 2D indexing.
 */

void initIntersection(Intersection *intersection)
{
    for (int i = 0; i < 4; i++)
    {
        intersection->lanes[i].head = NULL;
        intersection->lanes[i].tail = NULL;
        intersection->lanes[i].count = 0;
    }
    intersection->currentGreen = NORTH;
}

/*
 * initIntersection
 * Tip: Zeroes each lane and sets the initial green direction.
 * - No allocations here; just setup the structure for use.
 */

int getLaneIndex(Direction dir)
{
    switch (dir)
    {
    case NORTH:
        return 0;
    case SOUTH:
        return 1;
    case EAST:
        return 2;
    case WEST:
        return 3;
    default:
        return -1;
    }
}

void enqueue(Queue *q, Vehicle v)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL)
    {
        printf("Memory allocation failed in enqueue!\n");
        return;
    }
    newNode->data = v;
    newNode->next = NULL;
    if (q->count == 0)
    {
        newNode->prev = NULL;
        q->head = newNode;
        q->tail = newNode;
    }
    else
    {
        newNode->prev = q->tail;
        q->tail->next = newNode;
        q->tail = newNode;
    }
    q->count++;
}

/*
 * enqueue
 * Tip: add a vehicle to the tail in O(1).
 * - Allocates a node and updates pointers/counts; watch for malloc failures.
 */

int dequeue(Queue *q, Vehicle *outVehicle)
{
    if (q->count == 0 || q->head == NULL)
        return 0;
    Node *nodeToRemove = q->head;
    *outVehicle = nodeToRemove->data;
    q->head = nodeToRemove->next;
    if (q->head == NULL)
        q->tail = NULL;
    else
        q->head->prev = NULL;
    free(nodeToRemove);
    q->count--;
    return 1;
}

/*
 * dequeue
 * Tip: remove and return the head vehicle in O(1).
 * - Copies data into caller buffer and adjusts head/tail accordingly.
 */

void freeIntersection(Intersection *intersection)
{
    Vehicle dummy;
    for (int i = 0; i < 4; i++)
    {
        while (dequeue(&intersection->lanes[i], &dummy))
            ;
    }
}

/*
 * freeIntersection
 * Tip: drains all lanes and frees nodes — use before exit to avoid leaks.
 */

void drawVisualIntersection(Intersection *intersection, int visibleCars)
{
    int n_count = intersection->lanes[getLaneIndex(NORTH)].count;
    int s_count = intersection->lanes[getLaneIndex(SOUTH)].count;
    int e_count = intersection->lanes[getLaneIndex(EAST)].count;
    int w_count = intersection->lanes[getLaneIndex(WEST)].count;

    const char *GREEN = "\x1b[32m";
    const char *RED = "\x1b[31m";
    const char *YELLOW = "\x1b[33m";
    const char *RESET = "\x1b[0m";

    const char *n_color = (intersection->currentGreen == NORTH) ? GREEN : RED;
    const char *s_color = (intersection->currentGreen == SOUTH) ? GREEN : RED;
    const char *e_color = (intersection->currentGreen == EAST) ? GREEN : RED;
    const char *w_color = (intersection->currentGreen == WEST) ? GREEN : RED;

    char n_light = (intersection->currentGreen == NORTH) ? 'G' : 'R';
    char s_light = (intersection->currentGreen == SOUTH) ? 'G' : 'R';
    char e_light = (intersection->currentGreen == EAST) ? 'G' : 'R';
    char w_light = (intersection->currentGreen == WEST) ? 'G' : 'R';

    int rows = (2 * visibleCars) + 3;
    int cols = (4 * visibleCars) + 5;

    int center_r = visibleCars + 1;
    int center_c = (visibleCars * 2) + 2;

    char **matrix = (char **)malloc(rows * sizeof(char *));
    if (matrix == NULL)
        return;

    for (int i = 0; i < rows; i++)
    {
        matrix[i] = (char *)malloc(cols * sizeof(char));
        if (matrix[i] == NULL)
        {
            for (int k = 0; k < i; k++)
                free(matrix[k]);
            free(matrix);
            return;
        }
        for (int j = 0; j < cols; j++)
            matrix[i][j] = ' ';
    }

    for (int r = 0; r < rows; r++)
    {
        if (r < center_r - 1 || r > center_r + 1)
        {
            matrix[r][center_c - 2] = '|';
            matrix[r][center_c + 2] = '|';
        }
    }
    for (int c = 0; c < cols; c++)
    {
        if (c < center_c - 2 || c > center_c + 2)
        {
            matrix[center_r - 1][c] = '-';
            matrix[center_r + 1][c] = '-';
        }
    }

    for (int i = 0; i < n_count && i < visibleCars; i++)
        matrix[center_r - 2 - i][center_c] = '#';
    for (int i = 0; i < s_count && i < visibleCars; i++)
        matrix[center_r + 2 + i][center_c] = '#';
    for (int i = 0; i < w_count && i < visibleCars; i++)
        matrix[center_r][center_c - 3 - (i * 2)] = '#';
    for (int i = 0; i < e_count && i < visibleCars; i++)
        matrix[center_r][center_c + 3 + (i * 2)] = '#';

    printf("\n--- VISUAL MAP (Capacity: %d Cars/Lane) ---\n", visibleCars);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (matrix[i][j] == '#')
                printf("%s#%s", YELLOW, RESET);
            else
                printf("%c", matrix[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < rows; i++)
        free(matrix[i]);
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

/*
 * drawVisualIntersection
 * Tip: prints an ASCII map of the intersection.
 */
