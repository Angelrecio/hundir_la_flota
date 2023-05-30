#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#define MAX_DIMENSION 10
#define MAX_SHOT_DELAY 10

typedef struct {
    int x;
    int y;
    char result[10];
} Shot;

typedef struct {
    int size;
    char type[20];
} Ship;

int random_number(int min, int max) {
    return min + rand() % (max - min + 1);
}

void write_shot(int fd, pid_t pid, int x, int y, char* result) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "%d:%d,%d:%s\n", pid, x, y, result);
    write(fd, buffer, strlen(buffer));
}

void read_shots(int fd, pid_t pid, Shot* shots, int* num_shots) {
    lseek(fd, 0, SEEK_SET);
    
    char buffer[100];
    int pid_len = snprintf(NULL, 0, "%d", pid);
    
    *num_shots = 0;
    while (read(fd, buffer, sizeof(buffer)) > 0) {
        int shot_pid;
        sscanf(buffer, "%d:", &shot_pid);
        
        if (shot_pid == pid) {
            Shot shot;
            sscanf(buffer, "%*d:%d,%d:%[^:]", &shot.x, &shot.y, shot.result);
            shots[*num_shots] = shot;
            (*num_shots)++;
        }
    }
}

void determine_target(Shot* shots, int num_shots, int last_x, int last_y, int* target_x, int* target_y) {
    int i;
    for (i = num_shots - 1; i >= 0; i--) {
        if (strcmp(shots[i].result, "TOCADO") == 0) {
            *target_x = shots[i].x;
            *target_y = shots[i].y;
            break;
        }
    }
    
    if (i < 0) {
        *target_x = random_number(0, MAX_DIMENSION - 1);
        *target_y = random_number(0, MAX_DIMENSION - 1);
    }
    
    if (last_x != -1 && last_y != -1) {
        if (*target_x == last_x) {
            if (*target_y < last_y && *target_y > 0)
                (*target_y)--;
            else if (*target_y > last_y && *target_y < MAX_DIMENSION - 1)
                (*target_y)++;
        } else if (*target_y == last_y) {
            if (*target_x < last_x && *target_x > 0)
                (*target_x)--;
            else if (*target_x > last_x && *target_x < MAX_DIMENSION - 1)
                (*target_x)++;
        }
    }
}

int main() {
    srand(time(NULL));
    
    // Crear el archivo de intercambio de disparos
    int fd = open("disparos.txt", O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Error al crear el archivo de intercambio de disparos");
        exit(1);
    }
    
    // Configurar los tableros y barcos
    int dimension_x, dimension_y;
    printf("Introduce la dimensión X del tablero: ");
    scanf("%d", &dimension_x);

    printf("Introduce la dimensión Y del tablero: ");
    scanf("%d", &dimension_y);
    
    if (dimension_x < 5 || dimension_y < 5) {
        printf("Error: Las dimensiones del tablero deben ser al menos 5x5.\n");
        exit(1);
    }
    
    Ship carrier = {4, "Portaviones"};
    Ship bomber = {3, "Bombardero"};
    Ship frigate = {2, "Fragata"};
    
    // Cargar los tableros desde archivos de configuración predefinidos
    // (no se implementa en este código)
    
    // Crear los procesos hijos atacantes
    pid_t attacker1_pid, attacker2_pid;
    pid_t parent_pid = getpid();
    attacker1_pid = fork();
    
    if (attacker1_pid == -1) {
        perror("Error al crear el proceso atacante 1");
        exit(1);
    } else if (attacker1_pid == 0) {
        // Proceso atacante 1
        pid_t pid = getpid();
        sem_t* semaphore = sem_open("semaphore", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
        
        int last_x = -1, last_y = -1;
        int target_x, target_y;
        
        while (1) {
            usleep(random_number(0, MAX_SHOT_DELAY * 1000000));
            
            sem_wait(semaphore);
            
            // Leer los disparos realizados por el otro atacante
            Shot other_shots[MAX_DIMENSION * MAX_DIMENSION];
            int num_other_shots;
            read_shots(fd, attacker2_pid, other_shots, &num_other_shots);
            
            // Actualizar los resultados de los disparos en nuestra propia flota
            int i;
            for (i = 0; i < num_other_shots; i++) {
                if (other_shots[i].x >= 0 && other_shots[i].x < dimension_x &&
                    other_shots[i].y >= 0 && other_shots[i].y < dimension_y) {
                    // Actualizar el resultado del disparo en nuestra flota
                    // (aquí puedes implementar tu lógica para actualizar la flota)
                    printf("Resultado del disparo en nuestra flota: %s\n", other_shots[i].result);
                }
            }
            
            // Determinar el siguiente objetivo de disparo
            determine_target(other_shots, num_other_shots, last_x, last_y, &target_x, &target_y);
            
            // Realizar el disparo
            char result[10];
            snprintf(result, sizeof(result), "TOCADO");
            write_shot(fd, pid, target_x, target_y, result);
            
            // Actualizar las coordenadas del último disparo
            last_x = target_x;
            last_y = target_y;
            
            sem_post(semaphore);
        }
        
        sem_close(semaphore);
        sem_unlink("semaphore");
    } else {
        attacker2_pid = fork();
        
        if (attacker2_pid == -1) {
            perror("Error al crear el proceso atacante 2");
            exit(1);
        } else if (attacker2_pid == 0) {
            // Proceso atacante 2
            pid_t pid = getpid();
            sem_t* semaphore = sem_open("semaphore", O_RDWR);
            
            int last_x = -1, last_y = -1;
            int target_x, target_y;
            
            while (1) {
                usleep(random_number(0, MAX_SHOT_DELAY * 1000000));
                
                sem_wait(semaphore);
                
                // Leer los disparos realizados por el otro atacante
                Shot other_shots[MAX_DIMENSION * MAX_DIMENSION];
                int num_other_shots;
                read_shots(fd, attacker1_pid, other_shots, &num_other_shots);
                
                // Actualizar los resultados de los disparos en nuestra propia flota
                int i;
                for (i = 0; i < num_other_shots; i++) {
                    if (other_shots[i].x >= 0 && other_shots[i].x < dimension_x &&
                        other_shots[i].y >= 0 && other_shots[i].y < dimension_y) {
                        // Actualizar el resultado del disparo en nuestra flota
                        // (aquí puedes implementar tu lógica para actualizar la flota)
                        printf("Resultado del disparo en nuestra flota: %s\n", other_shots[i].result);
                    }
                }
                
                // Determinar el siguiente objetivo de disparo
                determine_target(other_shots, num_other_shots, last_x, last_y, &target_x, &target_y);
                
                // Realizar el disparo
                char result[10];
                snprintf(result, sizeof(result), "TOCADO");
                write_shot(fd, pid, target_x, target_y, result);
                
                // Actualizar las coordenadas del último disparo
                last_x = target_x;
                last_y = target_y;
                
                sem_post(semaphore);
            }
            
            sem_close(semaphore);
        } else {
            // Proceso padre
            int status1, status2;
            waitpid(attacker1_pid, &status1, 0);
            waitpid(attacker2_pid, &status2, 0);
            
            if (WIFEXITED(status1) && WIFEXITED(status2)) {
                char filename[100];
                time_t now = time(NULL);
                struct tm* tm_info = localtime(&now);
                strftime(filename, sizeof(filename), "batalla %d%m%Y_%H%M.txt", tm_info);
                
                int fd_copy = open(filename, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
                if (fd_copy == -1) {
                    perror("Error al crear el archivo de copia de la batalla");
                    exit(1);
                }
                
                char buffer[100];
                ssize_t num_bytes;
                lseek(fd, 0, SEEK_SET);
                while ((num_bytes = read(fd, buffer, sizeof(buffer))) > 0) {
                    write(fd_copy, buffer, num_bytes);
                }
                
                close(fd_copy);
            }
            
            close(fd);
            unlink("disparos.txt");
        }
    }
    
    return 0;
}
