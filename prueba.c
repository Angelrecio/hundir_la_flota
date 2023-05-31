#include <stdio.h>
#include <string.h>

void mostrar_mapas(char mapa1[][100], char mapa2[][100]) {
    FILE *archivo = fopen("mapas.txt", "rb");

    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }

    char linea[100];
    int i1 = 0;  // Variable para el mapa 1
    int i2 = 0;  // Variable para el mapa 2
    int mapa = 0;  // Variable para indicar en qué mapa se está guardando el contenido

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        if (strcmp(linea, "Mapa 1:\n") == 0) {
            mapa = 1;  // Se encuentra la etiqueta del mapa 1
            continue;  // Se omite la línea de la etiqueta y se pasa a la siguiente línea
        }

        if (strcmp(linea, "Mapa 2:\n") == 0) {
            mapa = 2;  // Se encuentra la etiqueta del mapa 2
            continue;  // Se omite la línea de la etiqueta y se pasa a la siguiente línea
        }

        if (mapa == 1) {
            strcpy(mapa1[i1], linea);  // Copiar la línea en el mapa 1
            i1++;
        } else if (mapa == 2) {
            strcpy(mapa2[i2], linea);  // Copiar la línea en el mapa 2
            i2++;
        }
    }

    fclose(archivo);
}

int main() {
    char mapa1[10][100];  // Definir el array para almacenar el contenido del mapa 1
    char mapa2[10][100];  // Definir el array para almacenar el contenido del mapa 2

    mostrar_mapas(mapa1, mapa2);

    for (int i = 0; i < 10; i++) {
        if (strcmp(mapa1[i], "") == 0) {
            break;  // Si se encuentra una línea vacía, se sale del bucle
        }
        printf("%s", (mapa1[i]));
    }

    for (int i = 0; i < 10; i++) {
        if (strcmp(mapa2[i], "") == 0) {
            break;  // Si se encuentra una línea vacía, se sale del bucle
        }
        printf("%s", mapa2[i]);
    }

    return 0;
}