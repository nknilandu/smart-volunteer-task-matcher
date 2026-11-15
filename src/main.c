#include <stdio.h>

#define MAX_VOL 10
#define MAX_TASK 5
#define MAX_N (MAX_VOL + MAX_TASK + 2)

// Global arrays
char tasks[MAX_TASK][25];
char volunteers[MAX_VOL][25];

int needed[MAX_TASK];
int canDo[MAX_VOL][MAX_TASK];
int order[MAX_TASK];

int residual[MAX_N][MAX_N];
int parent[MAX_N];

int taskCount = 0, volunteerCount = 0;
int tasksReady = 0, volunteersReady = 0;

// Clear extra keyboard input
void clearLine(void)
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF)
    {
    }
}

// Read a valid number
int readNumber(const char message[], int low, int high)
{
    int number;

    while (1)
    {
        printf("%s", message);

        int result = scanf("%d", &number);

        if (result == EOF)
            return -1;

        clearLine();

        if (result == 1 && number >= low && number <= high)
            return number;

        printf("[!] Enter a number between %d and %d.\n",
               low, high);
    }
}

// ==========================================
// ALGORITHM 1: QUICK SORT
// ==========================================

int partition(int low, int high)
{
    int pivot = needed[order[high]];
    int i = low - 1;

    for (int j = low; j < high; j++)
    {
        if (needed[order[j]] < pivot)
        {
            i++;

            int temp = order[i];
            order[i] = order[j];
            order[j] = temp;
        }
    }

    int temp = order[i + 1];
    order[i + 1] = order[high];
    order[high] = temp;

    return i + 1;
}

void quickSort(int low, int high)
{
    if (low < high)
    {
        int p = partition(low, high);

        quickSort(low, p - 1);
        quickSort(p + 1, high);
    }
}

// ==========================================
// BFS: FIND AVAILABLE PATH
// ==========================================

int bfs(int source, int sink, int nodes)
{
    int visited[MAX_N] = {0};
    int queue[MAX_N];

    int front = 0, rear = 0;

    for (int i = 0; i < nodes; i++)
        parent[i] = -1;

    queue[rear++] = source;
    visited[source] = 1;

    while (front < rear)
    {
        int u = queue[front++];

        for (int v = 0; v < nodes; v++)
        {
            if (!visited[v] && residual[u][v] > 0)
            {
                visited[v] = 1;
                parent[v] = u;
                queue[rear++] = v;
            }
        }
    }

    return visited[sink];
}

// ==========================================
// ALGORITHM 2: FORD-FULKERSON
// ==========================================

int fordFulkerson(int source, int sink, int nodes)
{
    int maxFlow = 0;

    while (bfs(source, sink, nodes))
    {

        // Each path assigns one volunteer
        for (int v = sink; v != source; v = parent[v])
        {
            int u = parent[v];

            residual[u][v]--;
            residual[v][u]++;
        }

        maxFlow++;
    }

    return maxFlow;
}

// ==========================================
// MENU 1: ENTER TASKS
// ==========================================

void enterTasks(void)
{
    puts("\n+================= ENTER TASKS =================+");

    puts("[!] Changing tasks will clear old volunteer data.");

    tasksReady = 0;
    volunteersReady = 0;
    volunteerCount = 0;

    int count = readNumber(
        "How many tasks (1-5)? ", 1, MAX_TASK);

    if (count == -1)
        return;

    for (int j = 0; j < count; j++)
    {
        printf("\nTask %d name (one word): ", j + 1);

        if (scanf("%24s", tasks[j]) != 1)
            return;

        clearLine();

        needed[j] = readNumber(
            "Volunteers needed (0-10): ", 0, MAX_VOL);

        if (needed[j] == -1)
            return;
    }

    taskCount = count;
    tasksReady = 1;

    printf("\n[OK] %d task(s) saved.\n", taskCount);
    puts("Now choose menu 2 to enter volunteers.");
}

// ==========================================
// MENU 2: ENTER VOLUNTEERS
// ==========================================

void enterVolunteers(void)
{
    if (!tasksReady)
    {
        puts("\n[!] Enter tasks first using menu 1.");
        return;
    }

    puts("\n+=============== ENTER VOLUNTEERS ==============+");

    volunteersReady = 0;

    int count = readNumber(
        "How many volunteers (1-10)? ", 1, MAX_VOL);

    if (count == -1)
        return;

    for (int i = 0; i < count; i++)
    {
        printf("\nVolunteer %d name (one word): ", i + 1);

        if (scanf("%24s", volunteers[i]) != 1)
            return;

        clearLine();

        // Volunteer skill matching
        for (int j = 0; j < taskCount; j++)
        {
            printf("Can %s do %s?\n",
                   volunteers[i], tasks[j]);

            canDo[i][j] = readNumber(
                "  1=Yes, 0=No: ", 0, 1);

            if (canDo[i][j] == -1)
                return;
        }
    }

    volunteerCount = count;
    volunteersReady = 1;

    printf("\n[OK] %d volunteer(s) saved.\n", volunteerCount);
    puts("Choose menu 4 to run matching.");
}

// ==========================================
// MENU 3: VIEW SAVED DATA
// ==========================================

void viewData(void)
{
    if (!tasksReady)
    {
        puts("\n[!] No tasks saved. Use menu 1.");
        return;
    }

    puts("\n+================== SAVED DATA =================+");

    puts("\nTASKS:");

    for (int j = 0; j < taskCount; j++)
    {
        printf("  %d. %-20s Need: %d\n",
               j + 1, tasks[j], needed[j]);
    }

    if (!volunteersReady)
    {
        puts("\nNo volunteers saved yet. Use menu 2.");
        return;
    }

    puts("\nVOLUNTEERS AND ELIGIBLE TASKS:");

    for (int i = 0; i < volunteerCount; i++)
    {
        int found = 0;

        printf("  %-20s : ", volunteers[i]);

        for (int j = 0; j < taskCount; j++)
        {
            if (canDo[i][j])
            {
                printf("%s ", tasks[j]);
                found = 1;
            }
        }

        if (!found)
            printf("None");

        printf("\n");
    }
}

// ==========================================
// MENU 4: RUN VOLUNTEER MATCHING
// ==========================================

void runMatching(void)
{
    if (!tasksReady || !volunteersReady)
    {
        puts("\n[!] First enter tasks (1) and volunteers (2).");
        return;
    }

    // Clear previous graph
    for (int i = 0; i < MAX_N; i++)
    {
        for (int j = 0; j < MAX_N; j++)
        {
            residual[i][j] = 0;
        }
    }

    // ======================================
    // STEP 1: QUICK SORT
    // ======================================

    for (int j = 0; j < taskCount; j++)
        order[j] = j;

    quickSort(0, taskCount - 1);

    puts("\n+================ QUICK SORT ==================+");
    puts("TASKS (SMALL CAPACITY TO LARGE):");

    for (int j = 0; j < taskCount; j++)
    {
        int t = order[j];

        printf("  %-20s : %d needed\n",
               tasks[t], needed[t]);
    }

    // ======================================
    // STEP 2: BUILD GRAPH
    // ======================================

    int source = 0;
    int sink = volunteerCount + taskCount + 1;
    int nodes = sink + 1;

    // Source -> Volunteer -> Task
    for (int i = 0; i < volunteerCount; i++)
    {
        residual[source][i + 1] = 1;

        for (int j = 0; j < taskCount; j++)
        {
            int t = order[j];

            residual[i + 1][volunteerCount + 1 + j] =
                canDo[i][t];
        }
    }

    // Task -> Sink
    for (int j = 0; j < taskCount; j++)
    {
        residual[volunteerCount + 1 + j][sink] =
            needed[order[j]];
    }

    // ======================================
    // STEP 3: FORD-FULKERSON
    // ======================================

    int maxFlow = fordFulkerson(source, sink, nodes);

    int filled[MAX_TASK] = {0};

    // ======================================
    // STEP 4: FINAL ASSIGNMENTS
    // ======================================

    puts("\n+=============== FINAL ASSIGNMENTS =============+");

    printf("  %-20s | %-20s\n",
           "VOLUNTEER", "TASK");

    puts("  ---------------------------------------------");

    for (int i = 0; i < volunteerCount; i++)
    {
        int matched = 0;

        for (int j = 0; j < taskCount; j++)
        {
            int t = order[j];
            int taskNode = volunteerCount + 1 + j;

            if (canDo[i][t] && residual[i + 1][taskNode] == 0)
            {

                printf("  %-20s | %-20s\n",
                       volunteers[i], tasks[t]);

                filled[t]++;
                matched = 1;
                break;
            }
        }

        if (!matched)
        {
            printf("  %-20s | UNASSIGNED\n",
                   volunteers[i]);
        }
    }

    // ======================================
    // STEP 5: SUMMARY
    // ======================================

    puts("\n+=================== SUMMARY ==================+");

    printf("  Total volunteers : %d\n", volunteerCount);
    printf("  Maximum assigned : %d\n", maxFlow);
    printf("  Unassigned       : %d\n",
           volunteerCount - maxFlow);

    puts("\n  TASK VACANCIES:");

    for (int j = 0; j < taskCount; j++)
    {
        printf("  %-20s : %d/%d filled, %d empty\n",
               tasks[j],
               filled[j],
               needed[j],
               needed[j] - filled[j]);
    }

    puts("+==============================================+");
}

// ==========================================
// MAIN MENU
// ==========================================

int main(void)
{
    while (1)
    {
        puts("\n+================================================+");
        puts("|           SMART VOLUNTEER TASK MATCHER         |");
        puts("|         Quick Sort + Ford-Fulkerson (BFS)      |");
        puts("+================================================+");
        puts("|  1. Enter / Replace Tasks                      |");
        puts("|  2. Enter / Replace Volunteers                 |");
        puts("|  3. View Saved Data                            |");
        puts("|  4. Run Volunteer Matching                     |");
        puts("|  0. Exit                                       |");
        puts("+================================================+");

        int choice = readNumber(
            "Choose an option: ", 0, 4);

        if (choice == 1)
        {
            enterTasks();
        }
        else if (choice == 2)
        {
            enterVolunteers();
        }
        else if (choice == 3)
        {
            viewData();
        }
        else if (choice == 4)
        {
            runMatching();
        }
        else if (choice == 0 || choice == -1)
        {
            puts("\n+================= GOODBYE! =====================+");
            break;
        }
    }

    return 0;
}
