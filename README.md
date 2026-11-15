# Smart Volunteer Task Matcher

**A C console application that assigns volunteers to eligible tasks using Quick Sort and maximum flow.**

Developed for **Algorithms Lab (CSE 208)** at **Green University of Bangladesh**.

[Overview](#overview) · [Run the Project](#run-the-project) · [Sample Run](#sample-run) · [Algorithms](#algorithms) · [Documentation](#documentation) · [Author](#author)

## Overview

University events need volunteers for registration, photography, logistics, and other activities. Each volunteer may be eligible for several tasks, while each task has a limited number of positions.

Smart Volunteer Task Matcher finds the **maximum number of valid assignments** under two constraints:

- Each volunteer receives at most one task.
- Each task receives no more volunteers than its capacity, and only eligible volunteers can be assigned.

Quick Sort arranges tasks by increasing capacity. Ford-Fulkerson with BFS computes maximum flow and uses residual reverse edges to reconsider earlier assignments. The BFS-based variant is **Edmonds-Karp**.

> Sorting organizes the task order. Maximum flow determines the maximum assignment count. The program does not optimize fairness or personal preferences.

## Features

- Enter or replace tasks and their capacities.
- Enter or replace volunteers and their task eligibility.
- View the data entered during the current session.
- Display tasks sorted from smallest to largest capacity.
- Compute maximum valid volunteer assignments.
- Show each volunteer's assigned task or `UNASSIGNED` status.
- Report total volunteers, maximum assigned, unassigned volunteers, and vacancies for every task.
- Check numeric input ranges and require task data before volunteer entry.

## Technology

| Component | Implementation |
| --- | --- |
| Language | C, compiled in C11 mode |
| Editor used | Visual Studio Code |
| Compiler | GCC |
| Interface | Interactive console menu |
| Sorting | Quick Sort with a last-element pivot |
| Matching | Ford-Fulkerson with BFS (Edmonds-Karp) |
| Data structures | Arrays, eligibility matrix, residual-capacity matrix, queue, parent array |
| External libraries | None; the program uses `stdio.h` |

## Run the Project

### Requirements

- Git to clone the repository, or download its ZIP from GitHub.
- GCC available in your terminal.
- A terminal that supports interactive input.

VS Code is optional. The program can run from a terminal without an editor.

### Clone

```bash
git clone https://github.com/nknilandu/smart-volunteer-task-matcher.git
cd smart-volunteer-task-matcher
```

### Windows — PowerShell

Check the compiler:

```powershell
gcc --version
```

Compile from the repository root:

```powershell
gcc -std=c11 -Wall -Wextra -pedantic .\src\main.c -o .\matcher.exe
```

If compilation succeeds, run:

```powershell
.\matcher.exe
```

If GCC is installed through MSYS2 UCRT64 at its default location but PowerShell cannot find it, add its directory to the current terminal session:

```powershell
$env:Path = "C:\msys64\ucrt64\bin;" + $env:Path
gcc --version
```

This PATH change lasts only for that terminal session. For a permanent setup, add `C:\msys64\ucrt64\bin` to your Windows user `Path` and restart VS Code.

### Linux / macOS with GCC available

```bash
gcc -std=c11 -Wall -Wextra -pedantic src/main.c -o matcher
./matcher
```

### Running in VS Code

1. Open the project folder.
2. Open **Terminal → New Terminal**.
3. Save `src/main.c`.
4. Run the compile and execution commands above for your terminal.
5. Enter menu choices and program input in the terminal.

After changing the source code, save and compile again before running the new executable.

## Console Menu

| Option | Action |
| --- | --- |
| `1` | Enter / Replace Tasks |
| `2` | Enter / Replace Volunteers |
| `3` | View Saved Data |
| `4` | Run Volunteer Matching |
| `0` | Exit |

**Typical session:** enter tasks with option `1`, enter volunteers with option `2`, review the data with option `3`, and run matching with option `4`.

For eligibility prompts, enter `1` for Yes and `0` for No. Use single-word names, such as `Registration` or `Jack`.

> Replacing tasks clears the previous volunteer data because eligibility depends on the task list. “Saved Data” means data held in memory during the current run; it is not saved to disk.

## Sample Run

Enter these tasks in the following order:

| Task | Capacity |
| --- | ---: |
| Registration | 2 |
| Photography | 1 |
| Logistics | 3 |
| **Total positions** | **6** |

Enter the volunteers in this order, using the eligibility values below:

| Volunteer | Registration | Photography | Logistics |
| --- | ---: | ---: | ---: |
| Jack | 1 | 1 | 0 |
| Samia | 1 | 0 | 1 |
| Bob | 0 | 1 | 1 |
| Nil | 0 | 0 | 1 |
| Tania | 0 | 1 | 0 |

Quick Sort displays the task order as **Photography (1), Registration (2), Logistics (3)**.

### Verified assignments

| Volunteer | Assigned task |
| --- | --- |
| Jack | Registration |
| Samia | Registration |
| Bob | Logistics |
| Nil | Logistics |
| Tania | Photography |

| Summary | Value |
| --- | ---: |
| Total volunteers | 5 |
| Maximum assigned | 5 |
| Unassigned volunteers | 0 |
| Remaining vacancies | 1 |

| Task | Filled | Capacity | Vacant |
| --- | ---: | ---: | ---: |
| Registration | 2 | 2 | 0 |
| Photography | 1 | 1 | 0 |
| Logistics | 2 | 3 | 1 |

The supplied source was compiled with `-std=c11 -Wall -Wextra -pedantic` and run with this input to verify the result. All five volunteers are assigned, so no solution can assign more volunteers for this dataset. The remaining logistics vacancy reflects six positions but only five volunteers.

Other inputs may have multiple equally valid maximum assignments. Input order and traversal order can affect the selected pairs without changing the maximum count.

## Algorithms

### 1. Quick Sort

`quickSort()` and `partition()` sort the `order[]` array using task capacity as the key. The task names, capacities, and eligibility columns retain their original indices.

- Sort direction: ascending task capacity.
- Pivot: capacity of the last indexed task in the current partition.
- Average time: `O(T log T)`.
- Worst-case time: `O(T²)`.
- Equal-capacity tasks are not guaranteed to retain their input order.

### 2. Flow-network construction

| Connection | Capacity | Purpose |
| --- | ---: | --- |
| Source to each volunteer | 1 | At most one assignment per volunteer |
| Volunteer to an eligible task | 1 | Allow only eligible assignments |
| Task to sink | Required volunteers | Enforce task capacity |

An ineligible volunteer-task pair has zero capacity.

### 3. Ford-Fulkerson with BFS

1. BFS searches the residual network for a source-to-sink path with positive remaining capacity.
2. The program reconstructs the path using `parent[]`.
3. It subtracts one unit of forward capacity and adds one unit of reverse capacity along the path.
4. It increases the flow count by one and repeats until no augmenting path exists.

One-unit updates are appropriate here because every source-to-volunteer edge has capacity one. Reverse edges allow an earlier assignment to be undone and replaced as part of a better overall assignment.

### Complexity of this implementation

Let `V` be the volunteer count, `T` the task count, `N = V + T + 2` the network node count, and `F` the maximum flow.

| Operation | Bound |
| --- | --- |
| Quick Sort | Average `O(T log T)`, worst `O(T²)` |
| One BFS with the residual matrix | `O(N²)` |
| Maximum-flow loop, including the final unsuccessful BFS | `O((F + 1)N²)`, where `F ≤ V` |
| Residual-matrix space | `O(N²)` |

These bounds describe the matrix scans and unit augmentations in this source. The program allocates fixed-size arrays, with `MAX_N = 17`.

## Source Guide

| Function | Responsibility |
| --- | --- |
| `clearLine()` | Discard remaining characters on the input line |
| `readNumber()` | Read an integer within a requested range |
| `partition()` | Partition task indices by capacity |
| `quickSort()` | Recursively sort task indices |
| `bfs()` | Find reachable nodes and record the augmenting path |
| `fordFulkerson()` | Update residual capacities and calculate maximum flow |
| `enterTasks()` | Enter task names and capacities |
| `enterVolunteers()` | Enter volunteer names and eligibility |
| `viewData()` | Display current session data |
| `runMatching()` | Sort tasks, build the network, match, and report results |
| `main()` | Run the interactive menu |

## Applications

- Assign volunteers to registration, photography, and logistics at university events.
- Allocate eligible members to roles in club programs.
- Organize volunteer roles for community activities.
- Demonstrate sorting, residual networks, and maximum flow in an Algorithms Lab.

This is an educational prototype for small, clearly defined assignment problems.

## Limitations

- At most **10 volunteers** and **5 tasks**.
- Task capacities range from **0 to 10**.
- Names are single words, read with a limit of 24 characters.
- Each volunteer receives at most one task.
- Eligibility is entered manually as Yes/No; the program does not infer skills.
- No scheduling, preference scores, fairness objective, or weighted matching.
- No graphical interface, file persistence, or database.
- Closing the program loses the current session data.

## Future Improvements

- Add file-based storage or a database.
- Support full names and dynamically sized datasets.
- Add a graphical or web interface.
- Introduce time-slot constraints and preference-based assignment.
- Add automated regression tests for zero capacities, ineligible volunteers, and reassignment cases.

These are planned improvements, not implemented features.

## Documentation

The links below use the agreed `documentation/` folder structure. Add the corresponding files with these exact names for the links to work.

| Document | Format | File |
| --- | --- | --- |
| Project proposal | PDF | [Read proposal](documentation/proposal/Smart_Volunteer_Task_Matcher_Proposal.pdf) |
| Presentation | PDF | [View slides](documentation/slides/Smart_Volunteer_Task_Matcher_Presentation.pdf) |
| Presentation | PPTX | [Download PowerPoint](documentation/slides/Smart_Volunteer_Task_Matcher_Presentation.pptx) |
| Final report | PDF | [Read report](documentation/report/Smart_Volunteer_Task_Matcher_Report.pdf) |

[Open the Google Slides presentation](https://docs.google.com/presentation/d/1mbi3KXFKB0iQU8Iwywf3JiUkUoxzQgfITJSfBxMQdwk/edit?usp=sharing)

### Repository layout

| Path | Contents |
| --- | --- |
| `src/main.c` | Complete C implementation |
| `documentation/proposal/` | Project proposal PDF |
| `documentation/slides/` | Presentation PDF and PPTX |
| `documentation/report/` | Final report PDF |
| `.gitignore` | Excludes generated binaries and local files |
| `README.md` | Project overview and usage instructions |

## Author

**Nilandu Kumar Das Vapon**  
Student ID: **231902031**  
Department of Computer Science and Engineering  
Green University of Bangladesh  
Course: **Algorithms Lab (CSE 208)**

[GitHub Profile](https://github.com/nknilandu) · [Project Repository](https://github.com/nknilandu/smart-volunteer-task-matcher)
