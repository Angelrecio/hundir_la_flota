#include <stdio.h>
#include <stdlib.h>

void imprimirMapa(int** mapa, int filas, int columnas) {
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            printf("%d ", mapa[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main() {
    FILE* archivo;
    archivo = fopen("mapas.txt", "r");

    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return 1;
    }

    int numMapas;
    fscanf(archivo, "%d", &numMapas);

    int*** mapas = (int***)malloc(numMapas * sizeof(int**));
    int* filasMapas = (int*)malloc(numMapas * sizeof(int));
    int* columnasMapas = (int*)malloc(numMapas * sizeof(int));

    for (int m = 0; m < numMapas; m++) {
        fscanf(archivo, "%d", &filasMapas[m]);
        fscanf(archivo, "%d", &columnasMapas[m]);
        printf("Mapa %d:\n", m + 1);

        // Asignar memoria para el mapa
        int filas = filasMapas[m];
        int columnas = columnasMapas[m];
        mapas[m] = (int**)malloc(filas * sizeof(int*));
        for (int i = 0; i < filas; i++) {
            mapas[m][i] = (int*)malloc(columnas * sizeof(int));
            for (int j = 0; j < columnas; j++) {
                fscanf(archivo, "%d", &mapas[m][i][j]);
            }
        }

        // Imprimir el mapa
        imprimirMapa(mapas[m], filas, columnas);
    }

    // Liberar memoria
    for (int m = 0; m < numMapas; m++) {
        int filas = filasMapas[m];
        for (int i = 0; i < filas; i++) {
            free(mapas[m][i]);
        }
        free(mapas[m]);
    }
    free(mapas);
    free(filasMapas);
    free(columnasMapas);

    fclose(archivo);
    return 0;
}
