
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h> // For _kbhit() and _getch()

#define MAX_MOVES 20
#define INITIAL_HEALTH 3

char grid[10][10]; // Maximum grid size for larger difficulties
int gridSize = 5;  // Default grid size
int numTreasures = 3, numTraps = 3, numPowerUps = 2;
int playerX = 0, playerY = 0;
int score = 0, health = INITIAL_HEALTH, movesLeft = MAX_MOVES;

// Function prototypes
void chooseDifficulty();
void initializeGrid();
void displayGrid();
void placeItems(char item, int count);
void movePlayer(char direction);
void updateDynamicTraps();
void checkCell();
int isValidMove(int x, int y);
void saveScore(int score);
void showLeaderboard();
void revealHint();

int main() {
    char move;
    srand(time(0)); // Seed for randomization

    printf("=== Welcome to Treasure Hunt with a Twist! ===
");
    printf("Collect treasures, avoid traps, and use power-ups wisely!
");

    // Show leaderboard
    printf("Do you want to see the leaderboard? (y/n): ");
    char choice = getchar();
    getchar(); // Consume newline character
    if (choice == 'y' || choice == 'Y') {
        showLeaderboard();
    }

    // Choose difficulty level
    chooseDifficulty();
    printf("You have %d moves and %d health.
", movesLeft, INITIAL_HEALTH);
    printf("Press any key to start...
");
    getchar();

    initializeGrid();
    displayGrid();

    while (movesLeft > 0 && health > 0) {
        printf("\nYour Move (W/A/S/D): ");
        move = _getch(); // Get input without pressing Enter
        printf("%c\n", move);

        if (move == 'w' || move == 'a' || move == 's' || move == 'd') {
            movePlayer(move);
            checkCell();
            updateDynamicTraps();
            displayGrid();

            if (score >= numTreasures) {
                printf("Congratulations! You collected enough treasures to win!\n");
                break;
            }
        } else {
            printf("Invalid move! Use W/A/S/D to move.\n");
        }
    }

    if (health <= 0) {
        printf("Game Over! You ran out of health.\n");
    } else if (movesLeft <= 0) {
        printf("Game Over! You ran out of moves.\n");
    }

    // Save score
    if (score > 0) {
        saveScore(score);
    }

    return 0;
}

// Choose difficulty level
void chooseDifficulty() {
    int choice;
    printf("Choose Difficulty Level:\n");
    printf("1. Easy (5x5 grid, few traps)\n");
    printf("2. Medium (7x7 grid, more traps)\n");
    printf("3. Hard (10x10 grid, many traps)\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            gridSize = 5;
            numTreasures = 3;
            numTraps = 3;
            numPowerUps = 2;
            break;
        case 2:
            gridSize = 7;
            numTreasures = 5;
            numTraps = 7;
            numPowerUps = 3;
            break;
        case 3:
            gridSize = 10;
            numTreasures = 7;
            numTraps = 10;
            numPowerUps = 4;
            break;
        default:
            printf("Invalid choice! Defaulting to Easy.\n");
            gridSize = 5;
            numTreasures = 3;
            numTraps = 3;
            numPowerUps = 2;
    }
}

// Initialize the grid with random items
void initializeGrid() {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            grid[i][j] = '?';
        }
    }
    grid[playerX][playerY] = 'P'; // Place player
    placeItems('T', numTreasures); // Place treasures
    placeItems('X', numTraps);     // Place traps
    placeItems('+', numPowerUps);  // Place power-ups
}

// Display the grid
void displayGrid() {
    printf("\nGrid:\n");
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
    printf("Score: %d | Health: %d | Moves Left: %d\n", score, health, movesLeft);
}

// Randomly place items on the grid
void placeItems(char item, int count) {
    for (int i = 0; i < count; i++) {
        int x, y;
        do {
            x = rand() % gridSize;
            y = rand() % gridSize;
        } while (grid[x][y] != '?');
        grid[x][y] = item;
    }
}

// Move the player
void movePlayer(char direction) {
    int newX = playerX, newY = playerY;

    if (direction == 'w') newX--;
    if (direction == 'a') newY--;
    if (direction == 's') newX++;
    if (direction == 'd') newY++;

    if (isValidMove(newX, newY)) {
        grid[playerX][playerY] = '.'; // Mark previous cell as explored
        playerX = newX;
        playerY = newY;
        movesLeft--;
    } else {
        printf("Invalid move! You hit a wall.\n");
    }
}

// Check the cell contents
void checkCell() {
    char cell = grid[playerX][playerY];
    if (cell == 'T') {
        printf("You found a treasure!\n");
        score++;
    } else if (cell == 'X') {
        printf("You hit a trap!\n");
        health--;
    } else if (cell == '+') {
        printf("You found a power-up! Revealing part of the grid...\n");
        movesLeft += 5; // Example benefit
        revealHint();
    }
    grid[playerX][playerY] = 'P'; // Mark player position
}

// Update dynamic traps
void updateDynamicTraps() {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (grid[i][j] == 'X') {
                grid[i][j] = '?'; // Remove old trap
                placeItems('X', 1); // Place new trap
            }
        }
    }
}

// Validate moves
int isValidMove(int x, int y) {
    return x >= 0 && x < gridSize && y >= 0 && y < gridSize;
}

// Save score to file
void saveScore(int score) {
    FILE *file = fopen("scores.txt", "a");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }
    fprintf(file, "Score: %d\n", score);
    fclose(file);
    printf("Your score has been saved!\n");
}

// Show leaderboard
void showLeaderboard() {
    char line[100];
    FILE *file = fopen("scores.txt", "r");
    if (file == NULL) {
        printf("No scores available yet!\n");
        return;
    }
    printf("\n=== Leaderboard ===\n");
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
}

// Reveal hint
void revealHint() {
    int revealed = 0;
    for (int i = 0; i < gridSize && revealed < 5; i++) {
        for (int j = 0; j < gridSize && revealed < 5; j++) {
            if (grid[i][j] == 'T' || grid[i][j] == 'X' || grid[i][j] == '+') {
                printf("Hint: (%d, %d) contains %c\n", i, j, grid[i][j]);
                revealed++;
            }
        }
    }
}
