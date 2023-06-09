#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <stdbool.h>

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
    Coordenada posiciones_intermedias[MAX_BARCOS][MAX_DIMENSION];
    Coordenada posiciones_finales[MAX_BARCOS];
    int num_barcos; 
    int num_barcos_hundidos; 
    int num_intermedias[MAX_BARCOS]; 
    int estado[MAX_BARCOS];
    int aciertos; 
    char nombre;
    int hundido;
} TipoBarco;

typedef struct {
    TipoBarco tipos[MAX_BARCOS];
    int num_tipos;
    int dimensionX;
    int dimensionY;
    int num_disparos;
    
} Tablero;

typedef struct {
    int pid;
    Coordenada coordenada;
} Disparo;

typedef struct {
    int estado[MAX_DIMENSION][MAX_DIMENSION];
    Coordenada coordenadas[MAX_DIMENSION][MAX_DIMENSION];
} Oponente;


typedef struct {
    Coordenada coordenadas[MAX_DIMENSION][MAX_DIMENSION];  
    int num_aciertos[MAX_DIMENSION];  
} EstadoBarcos;
typedef struct {
    Coordenada* elementos;
    int capacidad;
    int tamano;
} ArrayCoordenadas;

void cargarTablero(const char* archivo, Tablero* tablero) {
    FILE* file = fopen(archivo, "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return;
    }

    int dimensionX, dimensionY;
    fscanf(file, "%dx%d", &dimensionX, &dimensionY);
    tablero->num_tipos = 0;
    tablero->dimensionX = dimensionX;
    tablero->dimensionY = dimensionY;

    while (!feof(file)) {
        TipoBarco tipoBarco;
        int num_barcos;
        if (fscanf(file, "%s %d", tipoBarco.tipo, &num_barcos) != 2)
            break;

        if (num_barcos > 0) {
            tipoBarco.num_barcos = num_barcos;
            tipoBarco.num_barcos_hundidos = 0;
            for (int j = 0; j < num_barcos; j++) {
                int x, y;
                fscanf(file, "%d %d", &x, &y);
                tipoBarco.posiciones[j].x = x;
                tipoBarco.posiciones[j].y = y;

                // Leer y almacenar las coordenadas intermedias
                int num_intermedias = 0;
                while (fscanf(file, ", %d %d", &x, &y) == 2) {
                    tipoBarco.posiciones_intermedias[j][num_intermedias].x = x;
                    tipoBarco.posiciones_intermedias[j][num_intermedias].y = y;
                    num_intermedias++;
                }

                // Leer la coordenada final
                fscanf(file, ", %d %d", &x, &y);
                tipoBarco.posiciones_finales[j].x = x;
                tipoBarco.posiciones_finales[j].y = y;

                tipoBarco.num_intermedias[j] = num_intermedias;
            }
            tablero->tipos[tablero->num_tipos] = tipoBarco;
            tablero->num_tipos++;
        }
    }

    fclose(file);
}

void imprimirTablero(const Tablero* tablero) {
    printf("Dimensiones del tablero: %d x %d\n", tablero->dimensionX, tablero->dimensionY);

    for (int i = 0; i < tablero->num_tipos; i++) {
        TipoBarco tipoBarco = tablero->tipos[i];
        printf("Barco: %s\n", tipoBarco.tipo);
        for (int j = 0; j < tipoBarco.num_barcos; j++) {
            Coordenada coordenada = tipoBarco.posiciones[j];
            printf("Posición inicial: %d, %d\n", coordenada.x, coordenada.y);

            // Imprimir las coordenadas intermedias
            for (int k = 0; k < tipoBarco.num_intermedias[j]-1; k++) {
                Coordenada intermedia = tipoBarco.posiciones_intermedias[j][k];
                printf("Coordenada intermedia: %d, %d\n", intermedia.x, intermedia.y);
            }

            Coordenada coordenada_final = tipoBarco.posiciones_finales[j];
            printf("Posición final: %d, %d\n", coordenada_final.x, coordenada_final.y);
        }
        printf("\n");
    }
}
//Función para reiniciar el archivo de disparos
void reiniciarArchivo(const char* archivo) {
    FILE* file = fopen(archivo, "w");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return;
    }
    fclose(file);
}

// Función para verificar si una coordenada ya ha sido disparada
int coordenadaYaDisparada(const Tablero* tablero, Coordenada coordenada) {
    for (int i = 0; i < tablero->num_tipos; i++) {
        TipoBarco* tipo = &tablero->tipos[i];
        for (int j = 0; j < tipo->num_barcos; j++) {
            Coordenada coordenada_inicial = tipo->posiciones[j];
            Coordenada coordenada_final = tipo->posiciones_finales[j];

            // Comprobar si la coordenada coincide con alguna coordenada ya disparada
            if ((coordenada.x >= coordenada_inicial.x && coordenada.x <= coordenada_final.x) &&
                (coordenada.y >= coordenada_inicial.y && coordenada.y <= coordenada_final.y)) {
                return 1;  // La coordenada ya ha sido disparada
            }
        }
    }
    return 0;  // La coordenada no ha sido disparada
}




void agregarCoordenada(ArrayCoordenadas* array, Coordenada coordenada) {
    if (array->tamano == array->capacidad) {
        // Si el array está lleno, redimensionar el array
        int nuevaCapacidad = array->capacidad * 2;
        Coordenada* nuevosElementos = (Coordenada*)realloc(array->elementos, nuevaCapacidad * sizeof(Coordenada));
        
        if (nuevosElementos == NULL) {
            printf("Error al asignar memoria.\n");
            return;
        }
        
        array->elementos = nuevosElementos;
        array->capacidad = nuevaCapacidad;
    }
    
    // Agregar la coordenada al final del array
    array->elementos[array->tamano] = coordenada;
    array->tamano++;
}

ArrayCoordenadas* crearArrayCoordenadas(int capacidadInicial) {
    ArrayCoordenadas* array = (ArrayCoordenadas*)malloc(sizeof(ArrayCoordenadas));
    if (array == NULL) {
        printf("Error al asignar memoria.\n");
        return NULL;
    }
    
    array->elementos = (Coordenada*)malloc(capacidadInicial * sizeof(Coordenada));
    if (array->elementos == NULL) {
        printf("Error al asignar memoria.\n");
        free(array);
        return NULL;
    }
    
    array->capacidad = capacidadInicial;
    array->tamano = 0;
    
    return array;
}

void liberarArrayCoordenadas(ArrayCoordenadas* array) {
    if (array != NULL) {
        free(array->elementos);
        free(array);
    }
}

void borrar_coordenada(int x, int y, const char* archivo_n, int jugador) {
    FILE* archivo = fopen(archivo_n, "r");
    if (archivo == NULL) {
        printf("Error al abrir el archivo.\n");
        return;
    }

    FILE* archivo_temporal = fopen("temp.txt", "w");
    if (archivo_temporal == NULL) {
        printf("Error al abrir el archivo temporal.\n");
        fclose(archivo);
        return;
    }

    FILE* archivo_elim_temporal = fopen("archivo_elim.txt", "a");
    if (archivo_elim_temporal == NULL) {
        printf("Error al abrir el archivo de coordenadas eliminadas.\n");
        fclose(archivo);
        fclose(archivo_temporal);
        return;
    }

    char linea[100];
    int modificar_coordenadas = 0;
    int borrado = 0;
    int barco_hundido = 0; // Variable para controlar si el barco está hundido

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        int coordenada_x, coordenada_y;
        if (strstr(linea, ",") != NULL || sscanf(linea, "%*s %*s %d %d", &coordenada_x, &coordenada_y) >= 1) {
            // Coordenadas de un barco
            modificar_coordenadas = 1;
            char* token = strtok(linea, ",");
            int primera_coordenada = 1; 
            int todas_coordenadas_eliminadas = 1; 
            while (token != NULL) {
                sscanf(token, "%d %d", &coordenada_x, &coordenada_y);
                if (coordenada_x == x && coordenada_y == y) {
                    fprintf(archivo_elim_temporal, "%d %d, ", coordenada_x, coordenada_y);
                    borrado = 1;
                } else {
                    if (!primera_coordenada) {
                        fprintf(archivo_temporal, ", "); 
                    } else {
                        primera_coordenada = 0; 
                    }
                    fprintf(archivo_temporal, "%d %d", coordenada_x, coordenada_y);
                    todas_coordenadas_eliminadas = 0; 
                }
                token = strtok(NULL, ",");
            }
            fprintf(archivo_temporal, "\n");

            if (todas_coordenadas_eliminadas && !barco_hundido) {
                barco_hundido = 1; 
            }
        } else if (sscanf(linea, "%d %d", &coordenada_x, &coordenada_y) == 2) {
            modificar_coordenadas = 1;
            if (coordenada_x == x && coordenada_y == y) {
                fprintf(archivo_elim_temporal, "%d %d\n", coordenada_x, coordenada_y);
                borrado = 1;
                barco_hundido = 1; 
            } else {
                fputs(linea, archivo_temporal);
            }
        } else {
            
            fputs(linea, archivo_temporal);
        }
    }

    fclose(archivo);
    fclose(archivo_temporal);
    fclose(archivo_elim_temporal);

    remove(archivo_n);
    rename("temp.txt", archivo_n);

    if (modificar_coordenadas) {
        if (borrado) {
            printf("Coordenada eliminada exitosamente.\n");
            if (barco_hundido) {
                printf("¡El jugador %d ha HUNDIDO un barco del oponente!\n", jugador);
            }
        } else {
            printf("No se encontraron coordenadas para eliminar.\n");
        }
    } else {
        printf("No se encontraron coordenadas para eliminar.\n");
    }
}


int cuantasCoordenadasQuedan(const char* nombreArchivo) {
    FILE* archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        printf("Error al abrir el archivo.\n");
        return -1;
    }
    int contador = 0;
    char linea[100];
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        int coordenada_x, coordenada_y;
        if (strstr(linea, ",") != NULL || sscanf(linea, "%*s %*s %d %d", &coordenada_x, &coordenada_y) >= 1) {
            char* token = strtok(linea, ",");
            while (token != NULL) {
                contador++;
                token = strtok(NULL, ",");
            }
        } else if (sscanf(linea, "%d %d", &coordenada_x, &coordenada_y) == 2) {
            contador++;
        }
    }
    printf("Quedan %d coordenadas.\n", contador);
    fclose(archivo);
    return contador;
    
}
    
void crearArchivoBatalla(const char* nombreArchivo, int pidHijo1, int pidHijo2) {
    FILE* archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        printf("Error al abrir el archivo.\n");
        return;
    }
    char nombreArchivoCopia[100];
    char fecha[100];
    time_t tiempo = time(NULL);
    struct tm* fechaHora = localtime(&tiempo);
    strftime(fecha, sizeof(fecha), "%Y%m%d_%H%M", fechaHora);
    sprintf(nombreArchivoCopia, "batalla_%d_vs_%d.%s.txt", getpid(), getppid(), fecha);
    FILE* archivoCopia = fopen(nombreArchivoCopia, "w");
    if (archivoCopia == NULL) {
        printf("Error al crear el archivo copia.\n");
        return;
    }
    char linea[100];
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        fputs(linea, archivoCopia);
    }
    fclose(archivo);
    fclose(archivoCopia);
    printf("Se creó el archivo copia %s.\n", nombreArchivoCopia);
}

bool buscarWinsEnArchivo(const char* nombreArchivo) {
    FILE* archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return false;
    }

    char linea[256];
    while (fgets(linea, sizeof(linea), archivo)) {
        if (strstr(linea, "WINS!") != NULL) {
            fclose(archivo);
            
            return true;
        }
    }
    

    fclose(archivo);
        return false;
}

void escribirDisparo(const char* archivo, const char* mensaje, int pid, int jugador) {
    sem_t* semaforo = sem_open("/disparos_semaphore", O_CREAT, 0666, 1);
    if (semaforo == SEM_FAILED) {
        perror("Error al abrir el semáforo");
        return;
    }

    int fd = open(archivo, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd == -1) {
        perror("Error al abrir el archivo de disparos");
        sem_close(semaforo);
        return;
    }

    if (sem_wait(semaforo) == -1) { 
        perror("Error al bloquear el semáforo");
        close(fd);
        sem_close(semaforo);
        return;
    }

    char resultado_disparo[100];
    sprintf(resultado_disparo, "PID %d Jugador %d: %s\n", pid, jugador, mensaje);

    ssize_t bytesWritten = write(fd, resultado_disparo, strlen(resultado_disparo));
    if (bytesWritten == -1) {
        perror("Error al escribir en el archivo de disparos");
        sem_post(semaforo); 
        sem_close(semaforo);
        close(fd);
        return;
    }

    sem_post(semaforo); 
    sem_close(semaforo);
    close(fd);
}

void escribirWins(const char* archivo, int pid, int jugador) {
    sem_t* semaforo = sem_open("/disparos_semaphore", O_CREAT, 0666, 1);
    if (semaforo == SEM_FAILED) {
        perror("Error al abrir el semáforo");
        return;
    }

    int fd = open(archivo, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd == -1) {
        perror("Error al abrir el archivo de disparos");
        sem_close(semaforo);
        return;
    }

    if (sem_wait(semaforo) == -1) { 
        perror("Error al bloquear el semáforo");
        close(fd);
        sem_close(semaforo);
        return;
    }

    char resultado_disparo[100];
    sprintf(resultado_disparo, "PID %d Jugador %d: WINS!\n", pid, jugador);
    printf("Jugador %d WINS!\n", jugador);

    ssize_t bytesWritten = write(fd, resultado_disparo, strlen(resultado_disparo));
    if (bytesWritten == -1) {
        perror("Error al escribir en el archivo de disparos");
        sem_post(semaforo); 
        sem_close(semaforo);
        close(fd);
        return;
    }

    sem_post(semaforo); 
    sem_close(semaforo);
    close(fd);
}

void escribirGameOver(const char* archivo, int pid, int jugador) {
    sem_t* semaforo = sem_open("/disparos_semaphore", O_CREAT, 0666, 1);
    if (semaforo == SEM_FAILED) {
        perror("Error al abrir el semáforo");
        return;
    }

    int fd = open(archivo, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd == -1) {
        perror("Error al abrir el archivo de disparos");
        sem_close(semaforo);
        return;
    }

    if (sem_wait(semaforo) == -1) { 
        perror("Error al bloquear el semáforo");
        close(fd);
        sem_close(semaforo);
        return;
    }

    char resultado_disparo[100];
    sprintf(resultado_disparo, "PID %d GAME OVER\n", pid);
    printf("GAME OVER\n");

    ssize_t bytesWritten = write(fd, resultado_disparo, strlen(resultado_disparo));
    if (bytesWritten == -1) {
        perror("Error al escribir en el archivo de disparos");
        sem_post(semaforo); 
        sem_close(semaforo);
        close(fd);
        return;
    }

    sem_post(semaforo);
    sem_close(semaforo);
    close(fd);
}



void atacante(int jugador, const Tablero* miTablero, const Tablero* tableroOponente) {
    srand(time(NULL) + jugador);  
    ArrayCoordenadas* array = crearArrayCoordenadas(1);

    int oponente = (jugador == 1) ? 2 : 1;  

    int ultima_direccion = 0;
    Coordenada ultima_coordenada;
    ultima_coordenada.x = 0;
    ultima_coordenada.y = 0;
    int pid = getpid();
    int finJuego = 0;
    
    while (!finJuego) {
        int x, y;
        int disparo_aleatorio = 1;
        int disparidad = 1;
        int barco_hundido = 0;
        if(buscarWinsEnArchivo("disparos.txt")){
            break;
        }
        else{

        }
        while (disparidad == 1) {
            disparidad = 0;

            if (ultima_direccion == 0 || barco_hundido) {
                x = rand() % tableroOponente->dimensionX;
                y = rand() % tableroOponente->dimensionY;
                Coordenada coordenada_provisional;
                coordenada_provisional.x = x;
                coordenada_provisional.y = y;
                for (int i = 0; i < array->tamano; i++) {
                    Coordenada coordenada_array = array->elementos[i];
                    if (coordenada_provisional.x == coordenada_array.x && coordenada_provisional.y == coordenada_array.y) {
                        disparidad = 1;
                        break;
                    }
                }
            } else {
                x = ultima_coordenada.x;
                y = ultima_coordenada.y;

                if (ultima_direccion == 1 && x < tableroOponente->dimensionX - 1) {
                    x++;
                } else if (ultima_direccion == 2 && y < tableroOponente->dimensionY - 1) {
                    y++;
                } else if (ultima_direccion == 3 && x > 0) {
                    x--;
                } else if (ultima_direccion == 4 && y > 0) {
                    y--;
                } else {
                    x = rand() % tableroOponente->dimensionX;
                    y = rand() % tableroOponente->dimensionY;
                }
            }
        }

        Coordenada coordenada;
        coordenada.x = x;
        coordenada.y = y;
        agregarCoordenada(array, coordenada);

        TipoBarco* tipoBarco = NULL;

        for (int i = 0; i < tableroOponente->num_tipos; i++) {
            TipoBarco* tipo = &tableroOponente->tipos[i];
            for (int j = 0; j < tipo->num_barcos; j++) {
                Coordenada coordenada_inicial = tipo->posiciones[j];
                Coordenada coordenada_final = tipo->posiciones_finales[j];

                int coordenada_encontrada = 0;
                for (int k = 0; k < tipo->num_intermedias[j]; k++) {
                    Coordenada coordenada_intermedia = tipo->posiciones_intermedias[j][k];
                    if (coordenada.x == coordenada_intermedia.x && coordenada.y == coordenada_intermedia.y) {
                        coordenada_encontrada = 1;
                        break;
                    }
                }

                if (coordenada.x == coordenada_inicial.x && coordenada.y == coordenada_inicial.y) {
                    coordenada_encontrada = 1;
                }

                if (coordenada.x == coordenada_final.x && coordenada.y == coordenada_final.y) {
                    coordenada_encontrada = 1;
                }

                if (coordenada_encontrada) {
                    tipoBarco = tipo;
                    break;
                }
            }

            if (tipoBarco) {
                break;
            }
        }

        if (tipoBarco) {
            printf("¡El jugador %d ha TOCADO en un barco del jugador %d! Coordenadas: %d, %d\n", jugador, oponente, coordenada.x, coordenada.y);
            ultima_direccion = 0; 
            char mensaje[50];
            sprintf(mensaje, "%d, %d : TOCADO", coordenada.x, coordenada.y);
            escribirDisparo("disparos.txt", mensaje, pid, jugador);
            if (jugador == 1) {
                borrar_coordenada(coordenada.x, coordenada.y, "tablero2.txt", jugador);
                if (cuantasCoordenadasQuedan("tablero2.txt")==0) {
                printf("GAME OVER. Todas las coordenadas del tablero del jugador %d han sido disparadas.\n", oponente);
                
                escribirGameOver("disparos.txt", getppid, oponente);
                escribirWins("disparos.txt", pid, jugador);
                

                  return;
                
                }
            } else if (jugador == 2) {
                borrar_coordenada(coordenada.x, coordenada.y, "tablero1.txt", jugador);
                if (cuantasCoordenadasQuedan("tablero1.txt")==0) {
                printf("GAME OVER. Todas las coordenadas del tablero del jugador %d han sido disparadas.\n", oponente);
                escribirGameOver("disparos.txt", getppid, oponente);
                escribirWins("disparos.txt", pid, jugador);
                
            
                
                  return;
                

                }
            }

            ultima_coordenada = coordenada;

            Disparo disparo;
            disparo.pid = pid;
            disparo.coordenada = coordenada;

            
        } else {
            printf("El jugador %d ha disparado al AGUA. Coordenadas: %d, %d\n", jugador, coordenada.x, coordenada.y);
            if (disparo_aleatorio) {
                ultima_direccion = 0; 
                char mensaje[50];
                sprintf(mensaje, "%d, %d : AGUA", coordenada.x, coordenada.y);
                escribirDisparo("disparos.txt", mensaje, pid, jugador);


                
                Disparo disparo;
                disparo.pid = pid;
                disparo.coordenada = coordenada;

                
            }
        }

        ultima_coordenada = coordenada;

        

        int tiempoEspera = rand() % 11;
        sleep(tiempoEspera);
    }
}



int main() {
    
    reiniciarArchivo("disparos.txt");
    reiniciarArchivo("archivo_elim.txt");

    Tablero tablero1;
    EstadoBarcos estadoBarcos1;
    cargarTablero("tablero1.txt", &tablero1);
    
    Tablero tablero2;
    EstadoBarcos estadoBarcos2;
    cargarTablero("tablero2.txt", &tablero2);
    

    printf("Tablero del jugador 1:\n");
    imprimirTablero(&tablero1);

    printf("Tablero del jugador 2:\n");
    imprimirTablero(&tablero2);

    pid_t pid1, pid2;
    int status1, status2;

    pid1 = fork();
    if (pid1 == 0) {
        // Proceso del jugador 1 (atacante)
        atacante(1, &tablero1, &tablero2);
        exit(0);
    } else if (pid1 > 0) {
        pid2 = fork();
        if (pid2 == 0) {
            // Proceso del jugador 2 (atacante)
            atacante(2, &tablero2, &tablero1);
            exit(0);
        } else if (pid2 > 0) {
            // Proceso del juego
            waitpid(pid1, &status1, 0);
            waitpid(pid2, &status2, 0);

            if (WIFEXITED(status1)) {
                
                printf("El jugador 1 ha finalizado\n");
            }
            if (WIFEXITED(status2)) {
                printf("El jugador 2 ha finalizado\n");
            }
            crearArchivoBatalla("disparos.txt",pid1, pid2);
        } else {
            perror("Error al crear el proceso del jugador 2");
        }
    } else {
        perror("Error al crear el proceso del jugador 1");
    }

    return 0;
}