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

    int* fila = (int*)malloc(*columnas * sizeof(int));

    char* token = strtok(linea, " ");
    while (token != NULL) {
        fila[j] = atoi(token);
        j++;
        token = strtok(NULL, " ");
    }

    if (mapa == 0) {
        *mapa1 = (int**)realloc(*mapa1, (*filas_mapa1 + 1) * sizeof(int*));
        (*mapa1)[*filas_mapa1] = fila;
        (*filas_mapa1)++;
    } else {
        *mapa2 = (int**)realloc(*mapa2, (*filas_mapa2 + 1) * sizeof(int*));
        (*mapa2)[*filas_mapa2] = fila;
        (*filas_mapa2)++;
    }

    j = 0;
    i++;
}





    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        if (strncmp(linea, "Mapa 2:", 7) == 0) {
            mapa = 1;
            continue;
        }

        int* fila = (int*)malloc(*columnas * sizeof(int));

        char* token = strtok(linea, " ");
        while (token != NULL) {
            fila[j] = atoi(token);
            j++;
            token = strtok(NULL, " ");
        }

        if (mapa == 0) {
            *mapa1 = (int**)realloc(*mapa1, (*filas_mapa1 + 1) * sizeof(int*));
            (*mapa1)[*filas_mapa1] = fila;
            (*filas_mapa1)++;
        } else {
            *mapa2 = (int**)realloc(*mapa2, (*filas_mapa2 + 1) * sizeof(int*));
            (*mapa2)[*filas_mapa2] = fila;
            (*filas_mapa2)++;
        }

        j = 0;
        i++;
    }

    fclose(archivo);
}

int main() {
    const char* nombre_archivo = "mapas.txt";
    int filas_mapa1 = 0;
    int filas_mapa2 = 0;
    int columnas = 6;
    int** mapa1 = NULL;
    int** mapa2 = NULL;

    leer_mapas(nombre_archivo, &mapa1, &mapa2, &filas_mapa1, &filas_mapa2, &columnas);

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

        pid_t pid1, pid2;

    // Crear el primer hijo
    pid1 = fork();

    if (pid1 == 0) {
        // Código para el primer hijo
        printf("Proceso hijo 1. PID: %d\n", getpid());
        // Realizar tareas del primer hijo
    } else if (pid1 > 0) {
        // Proceso padre
        // Crear el segundo hijo
        pid2 = fork();

        if (pid2 == 0) {
            // Código para el segundo hijo
            printf("Proceso hijo 2. PID: %d\n", getpid());
            // Realizar tareas del segundo hijo
        } else if (pid2 > 0) {
            // Proceso padre
            printf("Proceso padre. PID: %d\n", getpid());
            // Realizar tareas del proceso padre
        } else {
            // Error al crear el segundo hijo
            printf("Error al crear el segundo hijo.\n");
        }
    } else {
        // Error al crear el primer hijo
        printf("Error al crear el primer hijo.\n");
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
