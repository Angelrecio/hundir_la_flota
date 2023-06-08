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

#define MAX_DIMENSION 10
#define MIN_DIMENSION 5
#define MAX_BARCOS 5

#define AGUA 0
#define TOCADO 1
#define HUNDIDO 2

#define DISPARO_INVALIDO -1
#define DISPARO_AGUA 0
#define DISPARO_ACERTADO 1
#define DISPARO_REPETIDO 2
#define DISPARO_HUNDIDO 3

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
    int num_barcos_hundidos; // Número de barcos hundidos
    int num_intermedias[MAX_BARCOS]; // Número de coordenadas intermedias para cada barco
    int estado[MAX_BARCOS];
    int aciertos; // Nuevo campo para contar los aciertos en el barco
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
    Coordenada coordenadas[MAX_DIMENSION][MAX_DIMENSION];  // Coordenadas acertadas de los barcos
    int num_aciertos[MAX_DIMENSION];  // Número de aciertos por cada barco
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
    fscanf(file, "%d %d", &dimensionX, &dimensionY);
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

void reiniciarArchivo(const char* archivo) {
    FILE* file = fopen(archivo, "w");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return;
    }
    fclose(file);
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

    if (sem_wait(semaforo) == -1) { // Bloquear el semáforo
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
        sem_post(semaforo); // Desbloquear el semáforo
        sem_close(semaforo);
        close(fd);
        return;
    }

    sem_post(semaforo); // Desbloquear el semáforo
    sem_close(semaforo);
    close(fd);
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

void imprimirEstadoBarcos(const Tablero* tablero, const EstadoBarcos* estadoBarcos) {
for (int i = 0; i < tablero->num_tipos; i++) {
printf("Barco %d (%s):\n", i + 1, tablero->tipos[i].nombre);
for (int j = 0; j < tablero->tipos[i].num_barcos; j++) {
Coordenada posicion = tablero->tipos[i].posiciones[j];
printf(" Posición %d: (%d, %d)\n", j + 1, posicion.x, posicion.y);
}
printf("\n");
}
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
    // Abre el archivo de texto en modo lectura
    if (archivo == NULL) {
        printf("Error al abrir el archivo.\n");
        return;
    }

    // Crea un archivo temporal en modo escritura
    FILE *archivo_temporal = fopen("temp.txt", "w");
    if (archivo_temporal == NULL) {
        printf("Error al abrir el archivo temporal.\n");
        fclose(archivo);
        return;
    }

    // Abre el archivo de coordenadas eliminadas en modo escritura
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

    // Lee cada línea del archivo original
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        // Verifica si la línea contiene las coordenadas
        int coordenada_x, coordenada_y;
        if (sscanf(linea, "%d %d", &coordenada_x, &coordenada_y) == 2) {
            // Verifica si las coordenadas coinciden con las especificadas para borrar
            if (coordenada_x == x && coordenada_y == y) {
                // Escribe las coordenadas eliminadas en el archivo de coordenadas eliminadas
                fprintf(archivo_elim_temporal, "%d %d\n", coordenada_x, coordenada_y);
                borrado = 1;
                continue;  // Ignora esta línea y pasa a la siguiente
            }
        }

        // Copia las líneas que no contienen las coordenadas al archivo temporal
        fputs(linea, archivo_temporal);
    }

    fclose(archivo);
    fclose(archivo_temporal);
    fclose(archivo_elim_temporal);

    if (borrado) {
        // Reemplaza el archivo original con el archivo temporal
        remove(archivo_n);
        rename("temp.txt", archivo_n);
        printf("Coordenada eliminada exitosamente.\n");
    } else {
        // Elimina el archivo temporal si no se encontraron las coordenadas
        remove("temp.txt");
        printf("No se encontraron coordenadas para eliminar.\n");
    }
}

int verificarCoordenadasEliminadas(const Tablero* tablero, const TipoBarco* tipoBarco, const char* archivo_n) {
    FILE* archivo = fopen(archivo_n, "r");
    if (archivo == NULL) {
        printf("Error al abrir el archivo del tablero.\n");
        return 0;
    }

    char linea[100];

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        if (strstr(linea, tipoBarco->nombre) != NULL) {
            char* token = strtok(linea, ",");
            while (token != NULL) {
                int coordenada_x, coordenada_y;
                sscanf(token, "%d %d", &coordenada_x, &coordenada_y);
                // Verificar si alguna coordenada del barco aún está presente en el archivo
                for (int i = 0; i < tipoBarco->num_intermedias + 2; i++) {
                    if (coordenada_x == tipoBarco->posiciones[i].x && coordenada_y == tipoBarco->posiciones[i].y) {
                        fclose(archivo);
                        return 0;  // Todavía hay una coordenada presente en el archivo
                    }
                }
                token = strtok(NULL, ",");
            }
        }
    }

    fclose(archivo);
    return 1;  // Todas las coordenadas del barco han sido eliminadas
}


void atacante(int jugador, const Tablero* miTablero, const Tablero* tableroOponente) {
    srand(time(NULL) + jugador);  // Inicializar la semilla para generar números aleatorios
    ArrayCoordenadas* array = crearArrayCoordenadas(1);

    int oponente = (jugador == 1) ? 2 : 1;  // Determinar el número del oponente

    int ultima_direccion = 0;
    Coordenada ultima_coordenada;
    ultima_coordenada.x = 0;
    ultima_coordenada.y = 0;
    int pid = getpid();
    while (1) {
        int x, y;
        int disparo_aleatorio = 1;
        int disparidad = 1;
        int barco_hundido = 0;
        while(disparidad == 1){
        disparidad = 0;

        if (ultima_direccion == 0 || barco_hundido) {
            // Generar coordenadas de disparo aleatorias
            x = rand() % tableroOponente->dimensionX;
            y = rand() % tableroOponente->dimensionY;
            Coordenada coordenada_provisional;
            coordenada_provisional.x = x;
            coordenada_provisional.y = y;
            // Recorrer y añadir elementos al array
            for (int i = 0; i < array->tamano; i++) {
                Coordenada coordenada_array = array->elementos[i];
                if(coordenada_provisional.x == coordenada_array.x && coordenada_provisional.y == coordenada_array.y){
                    disparidad = 1;
                }

            }
        } else {
            // Disparo en la dirección anterior
            x = ultima_coordenada.x;
            y = ultima_coordenada.y;

            // Determinar la siguiente coordenada según la dirección
            if (ultima_direccion == 1 && x < tableroOponente->dimensionX - 1) {
                x++;
            } else if (ultima_direccion == 2 && y < tableroOponente->dimensionY - 1) {
                y++;
            } else if (ultima_direccion == 3 && x > 0) {
                x--;
            } else if (ultima_direccion == 4 && y > 0) {
                y--;
            } else {
                // Si no es posible seguir en la dirección, disparar aleatoriamente
                x = rand() % tableroOponente->dimensionX;
                y = rand() % tableroOponente->dimensionY;
            }
        }}

        Coordenada coordenada;
        coordenada.x = x;
        coordenada.y = y;
        // Agregar coordenadas al array
        agregarCoordenada(array, coordenada);

        TipoBarco* tipoBarco = NULL;

        for (int i = 0; i < tableroOponente->num_tipos; i++) {
            TipoBarco* tipo = &tableroOponente->tipos[i];
            for (int j = 0; j < tipo->num_barcos; j++) {
                Coordenada coordenada_inicial = tipo->posiciones[j];
                Coordenada coordenada_final = tipo->posiciones_finales[j];

                // Verificar si la coordenada está en las posiciones intermedias
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
            ultima_direccion = 0; // Reiniciar la dirección
            char mensaje[50];
            sprintf(mensaje, "%d, %d : TOCADO", coordenada.x, coordenada.y);
            escribirDisparo("disparos.txt", mensaje, pid, jugador);
            if(jugador == 1){
                borrar_coordenada(coordenada.x, coordenada.y,"tablero2.txt", jugador);
            }
            if(jugador == 2){
                borrar_coordenada(coordenada.x, coordenada.y,"tablero1.txt", jugador);
            }
            
            
            ultima_coordenada = coordenada;
            // Incrementar el contador de aciertos del barco
            tipoBarco->aciertos++;

            // Verificar si el barco ha sido hundido
             if (tipoBarco->aciertos == tipoBarco->num_intermedias + 2) {
                    // Verificar si todas las coordenadas del barco han sido eliminadas
                    if(jugador == 1){
                         int todas_coordenadas_eliminadas = verificarCoordenadasEliminadas(tableroOponente, tipoBarco, "tablero1.txt");
                    if (todas_coordenadas_eliminadas) {
                       printf("¡El jugador %d ha HUNDIDO un barco del jugador %d!\n", jugador, oponente);
                        // Aquí puedes realizar las acciones necesarias cuando se hunde un barco, como actualizar la información del tablero, etc.
                    }
                    }

                    if(jugador == 2){
                         int todas_coordenadas_eliminadas = verificarCoordenadasEliminadas(tableroOponente, tipoBarco, "tablero2.txt");
                    if (todas_coordenadas_eliminadas) {
                       printf("¡El jugador %d ha HUNDIDO un barco del jugador %d!\n", jugador, oponente);
                        // Aquí puedes realizar las acciones necesarias cuando se hunde un barco, como actualizar la información del tablero, etc.
                    }
                    }
                   
             }

            // Crear un objeto Disparo y almacenar la información del disparo
            Disparo disparo;
            disparo.pid = pid;
            disparo.coordenada = coordenada;

            // Realizar las acciones necesarias con el disparo, como registrar el disparo en una lista de disparos realizados
            // y compartir la información del disparo con el oponente, si es necesario.

        } else {
            printf("El jugador %d ha disparado al AGUA. Coordenadas: %d, %d\n", jugador, coordenada.x, coordenada.y);
            if (disparo_aleatorio) {
                ultima_direccion = 0; // Reiniciar la dirección si el disparo fue aleatorio
                char mensaje[50];
                sprintf(mensaje, "%d, %d : AGUA", coordenada.x, coordenada.y);
                escribirDisparo("disparos.txt", mensaje, pid, jugador);

                // Crear un objeto Disparo y almacenar la información del disparo
                Disparo disparo;
                disparo.pid = pid;
                disparo.coordenada = coordenada;

                // Realizar las acciones necesarias con el disparo, como registrar el disparo en una lista de disparos realizados
                // y compartir la información del disparo con el oponente, si es necesario.
            }
        }

        ultima_coordenada = coordenada;

        // Esperar un tiempo aleatorio entre 0 y 10 segundos antes de realizar el siguiente disparo
        int tiempoEspera = rand() % 11;
        sleep(tiempoEspera);
    }
}



int main() {
    reiniciarArchivo("disparos.txt");
    
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
        } else {
            perror("Error al crear el proceso del jugador 2");
        }
    } else {
        perror("Error al crear el proceso del jugador 1");
    }

    return 0;
}