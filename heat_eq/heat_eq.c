// Francisco José Ramírez Aldaco A01634262
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <omp.h>
#define ROOM_LENGTH 7
#define ROOM_WIDTH 6
#define N_CELLS ROOM_LENGTH*ROOM_WIDTH
#define N_CALCULATED ((ROOM_LENGTH-2)*(ROOM_WIDTH-2))
#define GAMMA 0.0625 // thermal diffusivity (low -> no temp change, high -> temp goes way up or down)

char char_mat[ROOM_LENGTH][ROOM_WIDTH];
int sensed_cells = 0;
int dir_mat[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
float u[11][ROOM_LENGTH][ROOM_WIDTH];
int time_70_avg = -1;
float avg_temps[10];

void fill_default_temps() {
    #pragma omp parallel for collapse(2)
    for (int i=0; i<ROOM_LENGTH; i++)
        for (int j=0; j<ROOM_WIDTH; j++)
            u[0][i][j] = -1;

    for (int i=0; i<10; i++) avg_temps[i] = 0;
}

// Generate random csv file for room with set dimensions
// (big matrices are where parallelization truly shines)

void generate_room() {
    FILE* fp;
    int r;
    int threshold = rand() % 101 + 1;
    fp = fopen("room.csv", "w+");
    for (int i=0; i<ROOM_LENGTH; i++) {
        for (int j=0; j<ROOM_WIDTH; j++) {
            r = rand() % 101 + 1;
            fprintf(fp, "%c", r <= threshold ? 'H' : 'C');
            if (j == ROOM_WIDTH-1) fprintf(fp, "\n");
            else fprintf(fp, ",");
        }
    }
    fclose(fp);
}

// Translate csv file to matrix

void room_csv_to_mat() {
    FILE* fp = fopen("room.csv", "r");
    char c;
    int i = 0, j = 0;
    while ((c = fgetc(fp)) != EOF) {
        if (c == ',') j++;
        else if (c == '\n') {
            i++;
            j = 0;
        }
        else char_mat[i][j] = c;
    }
    fclose(fp);
}

// Run temperature detection (we assume the heat in the room is "frozen" in time)

void* detect_temp() {
    int pos[2];
    int dir, nextX, nextY;
    pos[0] = rand() % ROOM_LENGTH;
    pos[1] = rand() % ROOM_WIDTH;
    while (1) {
        dir = rand() % 4;
        nextX = pos[0] + dir_mat[dir][0];
        nextY = pos[1] + dir_mat[dir][1];
        if (u[0][pos[0]][pos[1]] == -1) {
            u[0][pos[0]][pos[1]] = 
                char_mat[pos[0]][pos[1]] == 'H' ? rand() % (100 - 60) + 60 : rand() % 61;
            sensed_cells++;
        }
        if (nextX >= 0 && nextX < ROOM_LENGTH 
            && nextY >= 0 && nextY < ROOM_WIDTH) {
            pos[0] = nextX;
            pos[1] = nextY;
        }
    }
}

// Run heat equation for incoming 10 hours

void heat_eq_calc() {
    float calc;
    for (int t=0; t<10; t++) {
        #pragma omp parallel for private(calc) collapse(2)
        for (int i=0; i<ROOM_LENGTH; i++) {
            for (int j=0; j<ROOM_WIDTH; j++) {
                if (i != 0 && j != 0 && i != ROOM_LENGTH-1 && j != ROOM_WIDTH-1) {
                    calc = GAMMA*(u[t][i+1][j] + u[t][i-1][j] + u[t][i][j+1]  + u[t][i][j-1] - 4*u[t][i][j]) + u[t][i][j];
                    u[t+1][i][j] = calc;
                    avg_temps[t] += calc/N_CALCULATED;
                } else {
                   u[t+1][i][j] = u[t][i][j]; 
                }
            }
        }
    }
}

// CSVs that we will use to plot our results

void generate_result_csvs() {
    FILE *fp_st, *fp_end;
    fp_st = fopen("room_hour_0.csv", "w+");
    fp_end = fopen("room_hour_10.csv", "w+");
    for (int i=0; i<ROOM_LENGTH; i++) {
        for (int j=0; j<ROOM_WIDTH; j++) {
            fprintf(fp_st, "%.1f", u[0][i][j]);
            fprintf(fp_end, "%.1f", u[10][i][j]);
            if (j == ROOM_WIDTH-1) {
                fprintf(fp_st, "\n");
                fprintf(fp_end, "\n");
            } else {
                fprintf(fp_st, ",");
                fprintf(fp_end, ",");
            }
        }
    }
    fclose(fp_st);
    fclose(fp_end);
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    fill_default_temps();
    generate_room();
    room_csv_to_mat();
    const int NUM_THREADS = atoi(argv[1]);
    omp_set_num_threads(NUM_THREADS);
    int ret;
    pthread_t threads[NUM_THREADS];
    for (int i=0; i<NUM_THREADS; i++)
        ret = pthread_create(&threads[i], NULL, &detect_temp, NULL);
    while (sensed_cells < N_CELLS) {}
    for (int i=0; i<NUM_THREADS; i++)
        ret = pthread_cancel(threads[i]);
    heat_eq_calc();
    for (int i=0; i<10; i++) {
        if (avg_temps[i] >= 70) {
            time_70_avg = i+1;
            printf("Avg temp of 70°C was reached at %d hours\n", time_70_avg);
            break;
        }
    }
    if (time_70_avg == -1) printf("70°C will not be reached in the following 10 hours.\n");
    generate_result_csvs();
    return 0;
}

