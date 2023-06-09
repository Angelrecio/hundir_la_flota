#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


#define MAX_BARCOS 5
#define MAX_COORDENADAS 50

typedef struct {
    int x;
    int y;
    int tamBarco;
} Coordenada;
Coordenada *barcosColocados = NULL; // Variable global

int numBarcosColocados = 0; // Número actual de barcos colocados
int capacidadBarcosColocados = 10; // Capacidad inicial del array



void limpiarCoordenadas() {
    numBarcosColocados = 0;
    capacidadBarcosColocados = 10;
    barcosColocados = realloc(barcosColocados, capacidadBarcosColocados * sizeof(Coordenada));
    if (barcosColocados == NULL) {
        printf("Error al redimensionar el array\n");
        exit(1);
    }
}

void agregarBarcoColocado(int x, int y) {
    if (barcosColocados == NULL) {
        capacidadBarcosColocados = 10; // Capacidad inicial
        barcosColocados = malloc(capacidadBarcosColocados * sizeof(Coordenada));
        if (barcosColocados == NULL) {
            printf("Error al asignar memoria\n");
            exit(1);
        }
    } else if (numBarcosColocados >= capacidadBarcosColocados) {
        capacidadBarcosColocados *= 2; // Duplicar la capacidad
        barcosColocados = realloc(barcosColocados, capacidadBarcosColocados * sizeof(Coordenada));
        if (barcosColocados == NULL) {
            printf("Error al redimensionar el array\n");
            exit(1);
        }
    }
    
    barcosColocados[numBarcosColocados].x = x;
    barcosColocados[numBarcosColocados].y = y;
    numBarcosColocados++;
}





bool verificarSuperposicionBarcos(int posX, int posY) {
    for (int i = 0; i < numBarcosColocados; i++) {
        int x = barcosColocados[i].x;
        int y = barcosColocados[i].y;

        if (posX == x && posY == y) {
            return true;
        }
    }

    return false;
}



void crearTablero(const char* archivo, int tableroNum, int dimensionX, int dimensionY) {
    char nombreArchivo[20];
    sprintf(nombreArchivo, "tablero%d.txt", tableroNum);

    FILE* file = fopen(nombreArchivo, "w");
    if (file == NULL) {
        perror("Error al crear el archivo");
        return;
    }

   

    fprintf(file, "%dx%d\n", dimensionX, dimensionY);

    for (int i = 1; i <= MAX_BARCOS; i++) {
        int num_barcos;
        switch (i) {
            case 1:
                printf("Introduce el número de Portaviones (4 unidades) para el tablero %d: ", tableroNum);
                scanf("%d", &num_barcos);
                fprintf(file, "Portaviones %d\n", num_barcos);

                for (int j = 0; j < num_barcos; j++) {
                    Coordenada coordenadas[4];
                    int posX, posY;
                    char orientacion;
                    int tamBarco = 4;
                    bool superposicion = false;

                    do {
                        printf("Introduce las coordenadas iniciales del Portaviones %d (formato: X Y) para el tablero %d: ",
                               j + 1, tableroNum);
                        scanf("%d %d", &posX, &posY);
                        printf("Introduce la orientación del Portaviones %d (H para horizontal, V para vertical) para el tablero %d: ",
                               j + 1, tableroNum);
                        scanf(" %c", &orientacion);

                        if (orientacion == 'H' || orientacion == 'h') {
                            if (posX + 3 >= dimensionX) {
                                printf("Las coordenadas iniciales no son válidas. El Portaviones se sale del tablero.\n");
                                continue;
                            }

                            for (int k = 0; k < tamBarco; k++) {
                                if (verificarSuperposicionBarcos(posX + k, posY)) {
                                    printf("Las coordenadas iniciales no son válidas. Hay una superposición de barcos.\n");
                                    superposicion = true;
                                    break;
                                } else {
                                    superposicion = false;
                                    coordenadas[k].x = posX + k;
                                    coordenadas[k].y = posY;
                                    coordenadas[k].tamBarco = tamBarco;
                                    agregarBarcoColocado(coordenadas[k].x, coordenadas[k].y);

                                }
                            }
                        } else if (orientacion == 'V' || orientacion == 'v') {
                            if (posY + 3 >= dimensionY) {
                                printf("Las coordenadas iniciales no son válidas. El Portaviones se sale del tablero.\n");
                                continue;
                            }

                            for (int k = 0; k < tamBarco; k++) {
                                if (verificarSuperposicionBarcos(posX, posY + k)) {
                                    printf("Las coordenadas iniciales no son válidas. Hay una superposición de barcos.\n");
                                    superposicion = true;
                                    break;
                                } else {
                                    superposicion = false;
                                    coordenadas[k].x = posX;
                                    coordenadas[k].y = posY + k;
                                    coordenadas[k].tamBarco = tamBarco;
                                    agregarBarcoColocado(coordenadas[k].x, coordenadas[k].y);

                                }
                            }
                        } else {
                            printf("La orientación no es válida. Introduce H para horizontal o V para vertical.\n");
                        }
                    } while (superposicion);
                    for (int k = 0; k < 4; k++) {
                        fprintf(file, "%d %d", coordenadas[k].x, coordenadas[k].y);
                        if (k < 3) {
                            fprintf(file, ", ");
                        }
                    }
                    fprintf(file, "\n");
                }

                break;
            case 2:
                printf("Introduce el número de Bombarderos (3 unidades) para el tablero %d: ", tableroNum);
                scanf("%d", &num_barcos);
                fprintf(file, "Bombarderos %d\n", num_barcos);

                for (int j = 0; j < num_barcos; j++) {
                    Coordenada coordenadas[3];
                    int posX, posY;
                    char orientacion;
                    int tamBarco = 3;
                    bool superposicion = false;

                    do {
                        printf("Introduce las coordenadas iniciales del Bombardero %d (formato: X Y) para el tablero %d: ",
                               j + 1, tableroNum);
                        scanf("%d %d", &posX, &posY);
                        printf("Introduce la orientación del Bombardero %d (H para horizontal, V para vertical) para el tablero %d: ",
                               j + 1, tableroNum);
                        scanf(" %c", &orientacion);

                        if (orientacion == 'H' || orientacion == 'h') {
                            if (posX + 2 >= dimensionX) {
                                printf("Las coordenadas iniciales no son válidas. El Bombardero se sale del tablero.\n");
                                continue;
                            }

                            for (int k = 0; k < tamBarco; k++) {
                                if (verificarSuperposicionBarcos( posX + k, posY)) {
                                    printf("Las coordenadas iniciales no son válidas. Hay una superposición de barcos.\n");
                                    superposicion = true;
                                    break;
                                } else {
                                    superposicion = false;
                                    coordenadas[k].x = posX + k;
                                    coordenadas[k].y = posY;
                                    coordenadas[k].tamBarco = tamBarco;
                                    agregarBarcoColocado(coordenadas[k].x, coordenadas[k].y);

                                }
                            }
                        } else if (orientacion == 'V' || orientacion == 'v') {
                            if (posY + 2 >= dimensionY) {
                                printf("Las coordenadas iniciales no son válidas. El Bombardero se sale del tablero.\n");
                                continue;
                            }

                            for (int k = 0; k < tamBarco; k++) {
                                if (verificarSuperposicionBarcos(posX, posY + k)) {
                                    printf("Las coordenadas iniciales no son válidas. Hay una superposición de barcos.\n");
                                    superposicion = true;
                                    break;
                                } else {
                                    superposicion = false;
                                    coordenadas[k].x = posX;
                                    coordenadas[k].y = posY + k;
                                    coordenadas[k].tamBarco = tamBarco;
                                    agregarBarcoColocado(coordenadas[k].x, coordenadas[k].y);

                                }
                            }
                        } else {
                            printf("La orientación no es válida. Introduce H para horizontal o V para vertical.\n");
                        }
                    } while (superposicion);
                    for (int k = 0; k < 3; k++) {
                        fprintf(file, "%d %d", coordenadas[k].x, coordenadas[k].y);
                        if (k < 2) {
                            fprintf(file, ", ");
                        }
                    }
                    fprintf(file, "\n");
                }

                break;
            case 3:
                printf("Introduce el número de Fragatas (2 unidades) para el tablero %d: ", tableroNum);
                scanf("%d", &num_barcos);
                fprintf(file, "Fragatas %d\n", num_barcos);

                for (int j = 0; j < num_barcos; j++) {
                    Coordenada coordenadas[2];
                    int posX, posY;
                    char orientacion;
                    int tamBarco = 2;
                    bool superposicion = false;

                    do {
                        printf("Introduce las coordenadas iniciales de la Fragata %d (formato: X Y) para el tablero %d: ",
                               j + 1, tableroNum);
                        scanf("%d %d", &posX, &posY);
                        printf("Introduce la orientación de la Fragata %d (H para horizontal, V para vertical) para el tablero %d: ",
                               j + 1, tableroNum);
                        scanf(" %c", &orientacion);

                        if (orientacion == 'H' || orientacion == 'h') {
                            if (posX + 1 >= dimensionX) {
                                printf("Las coordenadas iniciales no son válidas. La Fragata se sale del tablero.\n");
                                continue;
                            }

                            for (int k = 0; k < tamBarco; k++) {
                                if (verificarSuperposicionBarcos(posX + k, posY)) {
                                    printf("Las coordenadas iniciales no son válidas. Hay una superposición de barcos.\n");
                                    superposicion = true;
                                    break;
                                } else {
                                    superposicion = false;
                                    coordenadas[k].x = posX + k;
                                    coordenadas[k].y = posY;
                                    coordenadas[k].tamBarco = tamBarco;
                                    agregarBarcoColocado(coordenadas[k].x, coordenadas[k].y);

                                }
                            }
                        } else if (orientacion == 'V' || orientacion == 'v') {
                            if (posY + 1 >= dimensionY) {
                                printf("Las coordenadas iniciales no son válidas. La Fragata se sale del tablero.\n");
                                continue;
                            }

                            for (int k = 0; k < tamBarco; k++) {
                                if (verificarSuperposicionBarcos( posX, posY + k)) {
                                    printf("Las coordenadas iniciales no son válidas. Hay una superposición de barcos.\n");
                                    superposicion = true;
                                    break;
                                } else {
                                    superposicion = false;
                                    coordenadas[k].x = posX;
                                    coordenadas[k].y = posY + k;
                                    coordenadas[k].tamBarco = tamBarco;
                                    agregarBarcoColocado(coordenadas[k].x, coordenadas[k].y);

                                }
                            }
                        } else {
                            printf("La orientación no es válida. Introduce H para horizontal o V para vertical.\n");
                        }
                    } while (superposicion);
                    for (int k = 0; k < 2; k++) {
                        fprintf(file, "%d %d", coordenadas[k].x, coordenadas[k].y);
                        if (k < 1) {
                            fprintf(file, ", ");
                        }
                    }
                    fprintf(file, "\n");
                }

                break;
            
        }
    }

    fclose(file);
    printf("El tablero %d se ha creado correctamente.\n", tableroNum);
}

int main() {
    int dimensionX, dimensionY;
    int verificado = 1;
    while (verificado == 1) {
        printf("Introduce las dimensiones de los tableros (formato: X Y, mínimo 5): ");
        scanf("%d %d", &dimensionX, &dimensionY);

        if (dimensionX < 5 || dimensionY < 5) {
            printf("Las dimensiones mínimas del tablero son 5 x 5\n");
            printf("Por favor, reintroduzca las dimensiones correctas\n");
        } else {
            verificado = 3;
        }
    }
        crearTablero("tableros.txt", 1, dimensionX, dimensionY);
        limpiarCoordenadas();
        crearTablero("tableros.txt", 2, dimensionX, dimensionY);

    return 0;
}
