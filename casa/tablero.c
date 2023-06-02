#include <stdio.h>
#include <stdlib.h>

#define MAX_DIMENSION 10
#define MIN_DIMENSION 5
#define MAX_BARCOS 5

typedef struct {
    int x;
    int y;
} Coordenada;

typedef struct {
    char tipo[20];
    Coordenada posiciones[MAX_BARCOS];
    int num_barcos;
} Tablero;

void crearTablero(const char* archivo) {
    FILE* file = fopen(archivo, "w");
    if (file == NULL) {
        perror("Error al crear el archivo");
        return;
    }

    int dimensionX, dimensionY;
    printf("Introduce las dimensiones del tablero (formato: X Y, mínimo %d): ", MIN_DIMENSION);
    scanf("%d %d", &dimensionX, &dimensionY);

    if (dimensionX < MIN_DIMENSION || dimensionY < MIN_DIMENSION) {
        printf("Las dimensiones mínimas del tablero son %d x %d\n", MIN_DIMENSION, MIN_DIMENSION);
        fclose(file);
        return;
    }

    fprintf(file, "%d %d\n", dimensionX, dimensionY);

    for (int i = 1; i <= MAX_BARCOS; i++) {
        int num_barcos;
        switch (i) {
            case 1:
                printf("Introduce el número de Portaviones (4 unidades): ");
                scanf("%d", &num_barcos);
                fprintf(file, "Portaviones %d\n", num_barcos);
                break;
            case 2:
                printf("Introduce el número de Bombarderos (3 unidades): ");
                scanf("%d", &num_barcos);
                fprintf(file, "Bombarderos %d\n", num_barcos);
                break;
            case 3:
                printf("Introduce el número de Fragatas (2 unidades): ");
                scanf("%d", &num_barcos);
                fprintf(file, "Fragatas %d\n", num_barcos);
                break;
        }

        for (int j = 0; j < num_barcos; j++) {
            int x, y;
            printf("Introduce las coordenadas del barco %d (formato: X Y): ", j + 1);
            scanf("%d %d", &x, &y);
            fprintf(file, "%d %d\n", x, y);
        }
    }

    fclose(file);
    printf("El archivo \"%s\" se ha creado correctamente.\n", archivo);
}

int main() {
    crearTablero("tablero1.txt");
    crearTablero("tablero2.txt");

    return 0;
}
