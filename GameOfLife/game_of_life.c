#include <math.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define WORLD_HEIGHT 25
#define WORLD_WIDTH 80
void output(int **world);
int soSed(int **world, int m, int n);
void update(int **newWorld, int **world);
void liveOrDie(int **world);
int noChange(int **world, int **newWorld);
void fillMatrix(int **world);
void changeSpeed(int *speed, char *c, int *t, int *transm);
void gameOver(int **world, int *t);
void gameStart();
int main() {
    int **worldData = malloc(WORLD_HEIGHT * WORLD_WIDTH * sizeof(int) + WORLD_HEIGHT * sizeof(int *));
    int *worldP = (int *)(worldData + WORLD_HEIGHT);
    for (int i = 0; i < WORLD_HEIGHT; i++) worldData[i] = worldP + WORLD_WIDTH * i;
    fillMatrix(worldData);
    if (freopen("/dev/tty", "r", stdin) != NULL) {
        gameStart();
        output(worldData);
        int speed = 90000;
        int transm = 13;
        printw("\nSpeed = %d", speed);
        int t = 1, f = 1;
        char c;
        refresh();
        while (t && f) {
            clear();
            liveOrDie(worldData);
            printw("Pressed 'q' to Exit\n");
            printw("Pressed '-' to reduce the speed\n");
            printw("Pressed '+' to increase the speed\n");
            output(worldData);
            if (speed >= 150000) {
                printw("\nSpeed = MIN");
            } else if (speed <= 40000) {
                printw("\nSpeed = MAX");
            } else
                printw("\nSpeed = %d", transm);
            changeSpeed(&speed, &c, &f, &transm);
            usleep(speed);
            gameOver(worldData, &t);
        }
        clear();
        printw("THEY ARE ALL DEAD\n");
        printw("GAME OVER");
        refresh();
        usleep(1000000);
        endwin();
        free(worldData);
        return 0;
    } else {
        printf("File not found");
        free(worldData);
        return 0;
    }
}
void gameStart() {
    nodelay(initscr(), 1);
    noecho();
}
void changeSpeed(int *speed, char *c, int *t, int *transm) {
    *c = getch();
    if (*c == 'q')
        *t = 0;
    else if (*c == '-') {
        if (*speed >= 150000) {
            *speed = 150000;
            *transm = 1;
        } else {
            *speed += 5000;
            *transm -= 1;
        }

    } else if (*c == '+') {
        if (*speed <= 40000) {
            *speed = 40000;
            *transm = 23;
        } else {
            *speed -= 5000;
            *transm += 1;
        }
    }
}
void gameOver(int **world, int *t) {
    int count = 0;
    for (int i = 0; i < WORLD_HEIGHT; i++)
        for (int j = 0; j < WORLD_WIDTH; j++) {
            if (world[i][j] == 1) count++;
        }
    if (count == 0)
        *t = 0;
    else
        *t = 1;
}
void fillMatrix(int **world) {
    for (int i = 0; i < WORLD_HEIGHT; i++) {
        for (int j = 0; j < WORLD_WIDTH; j++) {
            scanf("%d", &world[i][j]);
        }
    }
}
void output(int **world) {
    for (int i = 0; i < WORLD_HEIGHT; i++) {
        for (int j = 0; j < WORLD_WIDTH; j++) {
            if (j < WORLD_WIDTH - 1) {
                if (world[i][j] == 1)
                    printw("@");
                else if (world[i][j] == 0)
                    printw(" ");
            }
        }
        if (i < WORLD_HEIGHT - 1) printw("\n");
    }
}
int soSed(int **world, int m, int n) {
    int count = 0;
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++) {
            if (world[(m + i + WORLD_HEIGHT) % WORLD_HEIGHT][(n + j + WORLD_WIDTH) % WORLD_WIDTH] == 1)
                count++;
        }
    if (world[m][n] == 1) count--;
    return count;
}
void liveOrDie(int **world) {
    int **newGen = malloc(WORLD_HEIGHT * WORLD_WIDTH * sizeof(int) + WORLD_HEIGHT * sizeof(int *));
    int *newGenP = (int *)(newGen + WORLD_HEIGHT);
    for (int i = 0; i < WORLD_HEIGHT; i++) newGen[i] = newGenP + WORLD_WIDTH * i;

    for (int i = 0; i < WORLD_HEIGHT; i++) {
        for (int j = 0; j < WORLD_WIDTH; j++) {
            if (!world[i][j] && soSed(world, i, j) == 3) newGen[i][j] = 1;
            if (world[i][j] && (soSed(world, i, j) < 2 || soSed(world, i, j) > 3)) newGen[i][j] = 0;
            if (world[i][j] && (soSed(world, i, j) == 2 || soSed(world, i, j) == 3)) newGen[i][j] = 1;
            if (!world[i][j] && soSed(world, i, j) < 3) newGen[i][j] = 0;
        }
    }
    update(newGen, world);
    free(newGen);
}
void update(int **newWorld, int **world) {
    for (int i = 0; i < WORLD_HEIGHT; i++)
        for (int j = 0; j < WORLD_WIDTH; j++) world[i][j] = newWorld[i][j];
}
