#include <stdio.h>
#include <stdbool.h>

#define MIN_DIMENSION 5
#define MAX_BARCOS 5

typedef struct {
    int x;
    int y;
    int tamBarco; // Agregamos el tamaño del barco en la estructura Coordenada

} Coordenada;


bool verificarSuperposicionBarcos(Coordenada* coordenadas, int numCoordenadas, Coordenada* barcosColocados, int numBarcosColocados) {
    for (int i = 0; i < numCoordenadas; i++) {
        for (int j = 0; j < numBarcosColocados; j++) {
            for (int k = 0; k < barcosColocados[j].tamBarco; k++) {
                if (barcosColocados[j].orientacion == 'H') {
                    if (coordenadas[i].x == barcosColocados[j].x + k && coordenadas[i].y == barcosColocados[j].y) {
                        return true;
                    }
                } else if (barcosColocados[j].orientacion == 'V') {
                    if (coordenadas[i].x == barcosColocados[j].x && coordenadas[i].y == barcosColocados[j].y + k) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}







void crearTablero(const char* archivo, int tableroNum) {
    char nombreArchivo[20];
    sprintf(nombreArchivo, "tablero%d.txt", tableroNum);

    FILE* file = fopen(nombreArchivo, "w");
    if (file == NULL) {
        perror("Error al crear el archivo");
        return;
    }

    int dimensionX, dimensionY;
    printf("Introduce las dimensiones del tablero %d (formato: X Y, mínimo %d): ", tableroNum, MIN_DIMENSION);
    scanf("%d %d", &dimensionX, &dimensionY);

    if (dimensionX < MIN_DIMENSION || dimensionY < MIN_DIMENSION) {
        printf("Las dimensiones mínimas del tablero son %d x %d\n", MIN_DIMENSION, MIN_DIMENSION);
        fclose(file);
        return;
    }

    fprintf(file, "%d %d\n", dimensionX, dimensionY);

     Coordenada barcosColocados[MAX_BARCOS * 3];

    for (int i = 1; i <= MAX_BARCOS; i++) {
        int num_barcos;
        switch (i) {
            case 1:
                printf("Introduce el número de Portaviones (1 unidad) para el tablero %d: ", tableroNum);
                scanf("%d", &num_barcos);
                fprintf(file, "Portaviones %d\n", num_barcos);

                for (int j = 0; j < num_barcos; j++) {
                    Coordenada coordenadas[4];
                    int posX, posY;
                    char orientacion;
                    int tamBarco = 4;
                    bool superposicion = false;


                    do {
                        printf("Introduce las coordenadas iniciales del Portaviones %d (formato: X Y) para el tablero %d: ", j+1, tableroNum);
                        scanf("%d %d", &posX, &posY);
                        printf("Introduce la orientación del Portaviones %d (H para horizontal, V para vertical) para el tablero %d: ", j+1, tableroNum);
                        scanf(" %c", &orientacion);

                        if (orientacion == 'H' || orientacion == 'h') {
                            if (posX + 3 >= dimensionX) {
                                printf("El Portaviones se sale del tablero. Introduce coordenadas válidas.\n");
                                continue;
                            }

                            for (int k = 0; k < 4; k++) {
                                if (posY >= dimensionY) {
                                    printf("El Portaviones se sale del tablero. Introduce coordenadas válidas.\n");
                                    superposicion = true;
                                    break;
                                }

                                 if (verificarSuperposicionBarcos(coordenadas, k, posX + k, posY, tamBarco, barcosColocados, j)) {
                                    printf("Superposición de barcos. Introduce coordenadas válidas.\n");
                                    superposicion = true;
                                    break;
                                }

                                coordenadas[k].x = posX + k;
                                coordenadas[k].y = posY;
                            }
                        } else if (orientacion == 'V' || orientacion == 'v') {
                            if (posY + 3 >= dimensionY) {
                                printf("El Portaviones se sale del tablero. Introduce coordenadas válidas.\n");
                                continue;
                            }

                            for (int k = 0; k < 4; k++) {
                                if (posX >= dimensionX) {
                                    printf("El Portaviones se sale del tablero. Introduce coordenadas válidas.\n");
                                    superposicion = true;
                                    break;
                                }

                                if (verificarSuperposicionBarcos(coordenadas, k, posX + k, posY, tamBarco, barcosColocados, j)) {
                                    printf("Superposición de barcos. Introduce coordenadas válidas.\n");
                                    superposicion = true;
                                    break;
                                }

                                coordenadas[k].x = posX;
                                coordenadas[k].y = posY + k;
                            }
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
                printf("Introduce el número de Bombarderos (1 unidad) para el tablero %d: ", tableroNum);
                scanf("%d", &num_barcos);
                fprintf(file, "Bombarderos %d\n", num_barcos);

                for (int j = 0; j < num_barcos; j++) {
                    Coordenada coordenadas[3];
                    int posX, posY;
                    char orientacion;
                    int tamBarco = 3;
                    bool superposicion = false;
                    do {
                        
                        printf("Introduce las coordenadas iniciales del Bombardero %d (formato: X Y) para el tablero %d: ", j+1, tableroNum);
                        scanf("%d %d", &posX, &posY);
                        printf("Introduce la orientación del Bombardero %d (H para horizontal, V para vertical) para el tablero %d: ", j+1, tableroNum);
                        scanf(" %c", &orientacion);

                        if (orientacion == 'H' || orientacion == 'h') {
                            if (posX + 2 >= dimensionX) {
                                printf("El Bombardero se sale del tablero. Introduce coordenadas válidas.\n");
                                continue;
                            }

                            for (int k = 0; k < 3; k++) {
                                if (posY >= dimensionY) {
                                    printf("El Bombardero se sale del tablero. Introduce coordenadas válidas.\n");
                                    superposicion = true;
                                    break;
                                }

                                if (verificarSuperposicionBarcos(coordenadas, k, posX + k, posY, tamBarco, barcosColocados, j)) {
                                    printf("Superposición de barcos. Introduce coordenadas válidas.\n");
                                    superposicion = true;
                                    break;
                                }

                                coordenadas[k].x = posX + k;
                                coordenadas[k].y = posY;
                            }
                        } else if (orientacion == 'V' || orientacion == 'v') {
                            if (posY + 2 >= dimensionY) {
                                printf("El Bombardero se sale del tablero. Introduce coordenadas válidas.\n");
                                continue;
                            }

                            for (int k = 0; k < 3; k++) {
                                if (posX >= dimensionX) {
                                    printf("El Bombardero se sale del tablero. Introduce coordenadas válidas.\n");
                                    superposicion = true;
                                    break;
                                }

                                if (verificarSuperposicionBarcos(coordenadas, k, posX + k, posY, tamBarco, barcosColocados, j)) {
                                    printf("Superposición de barcos. Introduce coordenadas válidas.\n");
                                    superposicion = true;
                                    break;
                                }

                                coordenadas[k].x = posX;
                                coordenadas[k].y = posY + k;
                            }
                        }
                    } while (superposicion);
                    for (int k = 0; k < tamBarco; k++) {
                        barcosColocados[j * tamBarco + k].x = coordenadas[k].x;
                        barcosColocados[j * tamBarco + k].y = coordenadas[k].y;
                    }

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
                printf("Introduce el número de Fragatas (1 unidad) para el tablero %d: ", tableroNum);
                scanf("%d", &num_barcos);
                fprintf(file, "Fragatas %d\n", num_barcos);

                for (int j = 0; j < num_barcos; j++) {
                    Coordenada coordenadas[2];
                    int posX, posY;
                    char orientacion;
                    int tamBarco = 2;
                    bool superposicion = false;

                    do {
                        printf("Introduce las coordenadas iniciales de la Fragata %d (formato: X Y) para el tablero %d: ", j+1, tableroNum);
                        scanf("%d %d", &posX, &posY);
                        printf("Introduce la orientación de la Fragata %d (H para horizontal, V para vertical) para el tablero %d: ", j+1, tableroNum);
                        scanf(" %c", &orientacion);

                        if (orientacion == 'H' || orientacion == 'h') {
                            if (posX + 1 >= dimensionX) {
                                printf("La Fragata se sale del tablero. Introduce coordenadas válidas.\n");
                                continue;
                            }

                            for (int k = 0; k < 2; k++) {
                                if (posY >= dimensionY) {
                                    printf("La Fragata se sale del tablero. Introduce coordenadas válidas.\n");
                                    superposicion = true;
                                    break;
                                }

                                if (verificarSuperposicionBarcos(coordenadas, k, posX + k, posY, tamBarco, barcosColocados, j)) {
                                    printf("Superposición de barcos. Introduce coordenadas válidas.\n");
                                    superposicion = true;
                                    break;
                                }

                                coordenadas[k].x = posX + k;
                                coordenadas[k].y = posY;
                            }
                        } else if (orientacion == 'V' || orientacion == 'v') {
                            if (posY + 1 >= dimensionY) {
                                printf("La Fragata se sale del tablero. Introduce coordenadas válidas.\n");
                                continue;
                            }

                            for (int k = 0; k < 2; k++) {
                                if (posX >= dimensionX) {
                                    printf("La Fragata se sale del tablero. Introduce coordenadas válidas.\n");
                                    superposicion = true;
                                    break;
                                }

                                if (verificarSuperposicionBarcos(coordenadas, k, posX + k, posY, tamBarco, barcosColocados, j)) {
                                    printf("Superposición de barcos. Introduce coordenadas válidas.\n");
                                    superposicion = true;
                                    break;
                                }

                                coordenadas[k].x = posX;
                                coordenadas[k].y = posY + k;
                            }
                        }
                    } while (superposicion);
                    for (int k = 0; k < tamBarco; k++) {
                        barcosColocados[j * tamBarco + k].x = coordenadas[k].x;
                        barcosColocados[j * tamBarco + k].y = coordenadas[k].y;
                    }

                    for (int k = 0; k < 2; k++) {
                        fprintf(file, "%d %d", coordenadas[k].x, coordenadas[k].y);
                        if (k < 1) {
                            fprintf(file, ", ");
                        }
                    }
                    fprintf(file, "\n");
                }

                break;
            default:
                break;
        }
    }

    fclose(file);
    printf("El tablero %d ha sido creado exitosamente.\n", tableroNum);
}


int main() {
    crearTablero("tablero1.txt", 1);
    crearTablero("tablero2.txt", 2);
    return 0;
}
