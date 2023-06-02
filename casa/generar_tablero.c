#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_DIMENSION 10

void generar_tablero(char tablero[MAX_DIMENSION][MAX_DIMENSION], int tamano_x, int tamano_y) {
    // Inicializar el tablero con agua
    for (int i = 0; i < tamano_x; i++) {
        for (int j = 0; j < tamano_y; j++) {
            tablero[i][j] = 'A';
        }
    }

    // Generar posiciones aleatorias para los barcos
    srand(time(NULL));
    int x, y;
    
    // Colocar barcos de tamaño 4 (portaviones)
    for (int i = 0; i < 1; i++) {
        x = rand() % tamano_x;
        y = rand() % tamano_y;
        for (int j = 0; j < 4; j++) {
            tablero[x][y + j] = 'P';
        }
    }

    // Colocar barcos de tamaño 3 (bombarderos)
    for (int i = 0; i < 2; i++) {
        x = rand() % tamano_x;
        y = rand() % tamano_y;
        for (int j = 0; j < 3; j++) {
            tablero[x][y + j] = 'B';
        }
    }

    // Colocar barcos de tamaño 2 (fragatas)
    for (int i = 0; i < 3; i++) {
        x = rand() % tamano_x;
        y = rand() % tamano_y;
        for (int j = 0; j < 2; j++) {
            tablero[x][y + j] = 'F';
        }
    }
}

int main() {
    int tamano_x = 10;
    int tamano_y = 10;
    char tablero[MAX_DIMENSION][MAX_DIMENSION];

    // Generar el tablero con los barcos
    generar_tablero(tablero, tamano_x, tamano_y);

    // Mostrar el tablero
    printf("Tablero:\n");
    for (int i = 0; i < tamano_x; i++) {
        for (int j = 0; j < tamano_y; j++) {
            printf("%c ", tablero[i][j]);
        }
        printf("\n");
    }

    return 0;
}
