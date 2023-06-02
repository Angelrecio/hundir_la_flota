#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void revisarResultadoDisparo(const char* archivoDisparos, const char* pidPropio) {
    FILE* archivo = fopen(archivoDisparos, "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo de disparos");
        exit(EXIT_FAILURE);
    }

    char linea[256];
    char resultadoDisparo[10] = "";
    size_t longitudPid = strlen(pidPropio);

    // Buscar el último disparo realizado por el proceso propio
    while (fgets(linea, sizeof(linea), archivo)) {
        if (strncmp(linea, pidPropio, longitudPid) == 0) {
            strncpy(resultadoDisparo, strstr(linea, ":") + 1, sizeof(resultadoDisparo) - 1);
        }
    }

    fclose(archivo);

    // Realizar las verificaciones necesarias y decidir el objetivo del siguiente disparo
    if (strcmp(resultadoDisparo, "AGUA") == 0) {
        // El disparo anterior dio en agua, realizar disparo aleatorio
    } else if (strcmp(resultadoDisparo, "TOCADO") == 0) {
        // El disparo anterior dio en un barco, realizar disparo hacia la derecha, si es posible
    } else if (strcmp(resultadoDisparo, "HUNDIDO") == 0) {
        // El barco contrincante fue hundido, realizar disparo aleatorio
    } else {
        // No se ha realizado un disparo previo o el resultado es desconocido
        // Realizar disparo aleatorio
    }
}

int main() {
    // Obtener el PID del proceso actual
    pid_t pid = getpid();

    // Obtener el PID del contrincante
    pid_t pidContrincante = (pid == pidHijo1) ? pidHijo2 : pidHijo1;

    // Obtener el resultado del último disparo propio (supongamos que lo almacenamos en la variable 'resultadoDisparoPropio')

    // Revisar el resultado del disparo propio y decidir el objetivo del siguiente disparo
    revisarResultadoDisparo("archivo_disparos.txt", pid);

    return 0;
}
