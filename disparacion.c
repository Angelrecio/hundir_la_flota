#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

void leer_mapas(const char* nombre_archivo, int*** mapa1, int*** mapa2, int* filas_mapa1, int* filas_mapa2, int* columnas) {
    FILE* archivo = fopen(nombre_archivo, "r");
    if (archivo == NULL) {
        printf("Error al abrir el archivo.\n");
        exit(1);
    }

    char linea[100];
    int i = 0;
    int j = 0;
    int mapa = 0;
    int primera_linea = 1; // Variable para indicar si es la primera línea de un mapa

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        // Eliminar espacios en blanco al final de la línea
        int len = strlen(linea);
        while (len > 0 && isspace(linea[len - 1])) {
            linea[len - 1] = '\0';
            len--;
        }

        if (strncmp(linea, "Mapa 1:", 7) == 0) {
            mapa = 0;
            continue;
        } else if (strncmp(linea, "Mapa 2:", 7) == 0) {
            mapa = 1;
            continue;
        }

        int* fila = malloc((*columnas) * sizeof(int));

        char* token = strtok(linea, " ");
        while (token != NULL) {
            fila[j] = atoi(token);
            j++;
            token = strtok(NULL, " ");
        }

        if (primera_linea) {
            *columnas = j; // Asignar la cantidad de elementos de la primera línea a columnas
            primera_linea = 0;
        }

        if (mapa == 0) {
            *mapa1 = realloc(*mapa1, (*filas_mapa1 + 1) * sizeof(int*));
            (*mapa1)[*filas_mapa1] = fila;
            (*filas_mapa1)++;
        } else {
            *mapa2 = realloc(*mapa2, (*filas_mapa2 + 1) * sizeof(int*));
            (*mapa2)[*filas_mapa2] = fila;
            (*filas_mapa2)++;
        }

        j = 0;
        i++;
    }

    fclose(archivo);
}

void eliminar_linea_ceros_adicional(int*** mapa, int* filas_mapa, int columnas) {
    int filas_nuevas = *filas_mapa;
    // Verificar si la última fila es una línea de ceros
    if (*filas_mapa > 0) {
        int es_linea_ceros = 1;
        for (int j = 0; j < columnas; j++) {
            if ((*mapa)[*filas_mapa - 1][j] != 0) {
                es_linea_ceros = 0;
                break;
            }
        }
        // Si es una línea de ceros, eliminarla
        if (es_linea_ceros) {
            filas_nuevas--;
            free((*mapa)[*filas_mapa - 1]);
        }
    }

    // Redimensionar el arreglo de filas
    *mapa = (int**)realloc(*mapa, filas_nuevas * sizeof(int*));
    *filas_mapa = filas_nuevas;
}

int main() {
    const char* nombre_archivo = "mapas.txt";
    int filas_mapa1 = 0;
    int filas_mapa2 = 0;
    int columnas = 0;
    int** mapa1 = NULL;
    int** mapa2 = NULL;

    leer_mapas(nombre_archivo, &mapa1, &mapa2, &filas_mapa1, &filas_mapa2, &columnas);
    eliminar_linea_ceros_adicional(&mapa1, &filas_mapa1, columnas);

    printf("Mapa 1:\n");
    for (int i = 0; i < filas_mapa1; i++) {
        for (int j = 0; j < columnas; j++) {
            printf("%d ", mapa1[i][j]);
        }
        printf("\n");
    }

    printf("\nMapa 2:\n");
    for (int i = 0; i < filas_mapa2; i++) {
        for (int j = 0; j < columnas; j++) {
            printf("%d ", mapa2[i][j]);
        }
        printf("\n");
    }

    // Liberar memoria
    for (int i = 0; i < filas_mapa1; i++) {
        free(mapa1[i]);
    }
    free(mapa1);

    for (int i = 0; i < filas_mapa2; i++) {
        free(mapa2[i]);
    }
    free(mapa2);

    return 0;
}
