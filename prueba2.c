#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define BOARD_SIZE_MIN 5
#define MAX_SHIP_TYPES 3
#define MAX_SHIP_SIZE 4
#define MAX_BUFFER_SIZE 100

// Estructura para representar un barco
typedef struct {
    int type;
    int size;
} Ship;

// Estructura para representar el tablero de juego
typedef struct {
    int **board;
    int size_x;
    int size_y;
} GameBoard;

// Función para liberar la memoria utilizada por el tablero de juego
void free_board(GameBoard *gameBoard) {
    for (int i = 0; i < gameBoard->size_x; i++) {
        free(gameBoard->board[i]);
    }
    free(gameBoard->board);
}

// Función para crear un tablero de juego vacío
void create_empty_board(GameBoard *gameBoard) {
    gameBoard->board = (int **)malloc(gameBoard->size_x * sizeof(int *));
    for (int i = 0; i < gameBoard->size_x; i++) {
        gameBoard->board[i] = (int *)malloc(gameBoard->size_y * sizeof(int));
        memset(gameBoard->board[i], 0, gameBoard->size_y * sizeof(int));
    }
}

// Función para colocar un barco en el tablero de juego
void place_ship(GameBoard *gameBoard, Ship *ship, int start_x, int start_y, char orientation) {
    if (orientation == 'H') {
        for (int i = 0; i < ship->size; i++) {
            gameBoard->board[start_x][start_y + i] = ship->type;
        }
    } else if (orientation == 'V') {
        for (int i = 0; i < ship->size; i++) {
            gameBoard->board[start_x + i][start_y] = ship->type;
        }
    }
}

// Función para cargar los tableros de juego desde un archivo de configuración
void load_game_boards(GameBoard *player1Board, GameBoard *player2Board, const char *configFile) {
    FILE *file = fopen(configFile, "r");
    
    if (file == NULL) {
        perror("Error al abrir el archivo de configuración");
        exit(1);
    }

    fscanf(file, "%d %d", &(player1Board->size_x), &(player1Board->size_y));
    fscanf(file, "%d %d", &(player2Board->size_x), &(player2Board->size_y));

    create_empty_board(player1Board);
    create_empty_board(player2Board);

    int shipCount[MAX_SHIP_TYPES] = {0};

    int x, y;
    char orientation;
    int shipType;

    while (fscanf(file, "%d %d %c", &x, &y, &orientation) == 3) {
        if (shipCount[shipType] >= MAX_SHIP_SIZE) {
            printf("Se ha alcanzado el máximo número de barcos del tipo %d\n", shipType);
            continue;
        }

        Ship *ship = (Ship *)malloc(sizeof(Ship));
        ship->type = shipType;
        ship->size = shipCount[shipType] + 2;

        if (x >= 0 && x < player1Board->size_x && y >= 0 && y < player1Board->size_y) {
            place_ship(player1Board, ship, x, y, orientation);
            shipCount[shipType]++;
        }

        if (x >= 0 && x < player2Board->size_x && y >= 0 && y < player2Board->size_y) {
            place_ship(player2Board, ship, x, y, orientation);
            shipCount[shipType]++;
        }
    }

    fclose(file);
}

// Función para imprimir el tablero de juego
void print_board(GameBoard *gameBoard) {
    for (int i = 0; i < gameBoard->size_x; i++) {
        for (int j = 0; j < gameBoard->size_y; j++) {
            printf("%d ", gameBoard->board[i][j]);
        }
        printf("\n");
    }
}

// Función para realizar un disparo al tablero de juego
void make_shot(GameBoard *gameBoard, int x, int y) {
    int cell = gameBoard->board[x][y];

    if (cell == 0) {
        gameBoard->board[x][y] = -1; // Disparo en agua
    } else if (cell > 0) {
        gameBoard->board[x][y] = -2; // Disparo a un barco

        // Verificar si el barco fue hundido
        int shipType = cell;
        int isShipSunk = 1;

        for (int i = 0; i < gameBoard->size_x; i++) {
            for (int j = 0; j < gameBoard->size_y; j++) {
                if (gameBoard->board[i][j] == shipType) {
                    isShipSunk = 0;
                    break;
                }
            }
            if (!isShipSunk) {
                break;
            }
        }

        if (isShipSunk) {
            for (int i = 0; i < gameBoard->size_x; i++) {
                for (int j = 0; j < gameBoard->size_y; j++) {
                    if (gameBoard->board[i][j] == shipType) {
                        gameBoard->board[i][j] = -3; // Barco hundido
                    }
                }
            }
        }
    }
}

// Función para escribir el resultado del disparo en el archivo de intercambio
void write_shot_result(int fileDescriptor, pid_t attackerPID, int x, int y, int result) {
    char buffer[MAX_BUFFER_SIZE];
    sprintf(buffer, "%d:%d,%d:%d\n", attackerPID, x, y, result);

    if (lockf(fileDescriptor, F_LOCK, 0) == -1) {
        perror("Error al bloquear el archivo de disparos");
        exit(1);
    }

    if (write(fileDescriptor, buffer, strlen(buffer)) == -1) {
        perror("Error al escribir en el archivo de disparos");
        exit(1);
    }

    if (lockf(fileDescriptor, F_ULOCK, 0) == -1) {
        perror("Error al desbloquear el archivo de disparos");
        exit(1);
    }
}

// Función para leer el resultado del último disparo del contrincante
void read_last_shot_result(int fileDescriptor, pid_t opponentPID, GameBoard *gameBoard) {
    char buffer[MAX_BUFFER_SIZE];
    char *token;

    lseek(fileDescriptor, 0, SEEK_SET);

    while (read(fileDescriptor, buffer, MAX_BUFFER_SIZE) > 0) {
        token = strtok(buffer, ":\n");
        pid_t attackerPID = atoi(token);

        if (attackerPID == opponentPID) {
            token = strtok(NULL, ":\n");
            char *coord = strtok(token, ",");
            int x = atoi(coord);
            coord = strtok(NULL, ",");
            int y = atoi(coord);
            token = strtok(NULL, ":\n");
            int result = atoi(token);

            if (result == -1) {
                printf("El oponente ha disparado en (%d,%d) y ha dado en agua.\n", x, y);
            } else if (result == -2) {
                printf("El oponente ha disparado en (%d,%d) y ha tocado un barco.\n", x, y);
                make_shot(gameBoard, x, y);
            } else if (result == -3) {
                printf("El oponente ha disparado en (%d,%d) y ha hundido un barco.\n", x, y);
                make_shot(gameBoard, x, y);
            }
        }
    }
}

// Función para determinar las coordenadas del siguiente disparo
void determine_next_shot_coordinates(GameBoard *gameBoard, int lastX, int lastY, int *nextX, int *nextY) {
    // Verificar si el último disparo fue en agua
    if (gameBoard->board[lastX][lastY] == -1) {
        *nextX = rand() % gameBoard->size_x;
        *nextY = rand() % gameBoard->size_y;
        return;
    }

    // Verificar si el último disparo tocó un barco
    if (gameBoard->board[lastX][lastY] == -2) {
        // Verificar hacia la derecha
        if (lastY + 1 < gameBoard->size_y && gameBoard->board[lastX][lastY + 1] >= 0) {
            *nextX = lastX;
            *nextY = lastY + 1;
            return;
        }

        // Verificar hacia abajo
        if (lastX + 1 < gameBoard->size_x && gameBoard->board[lastX + 1][lastY] >= 0) {
            *nextX = lastX + 1;
            *nextY = lastY;
            return;
        }

        // Verificar hacia la izquierda
        if (lastY - 1 >= 0 && gameBoard->board[lastX][lastY - 1] >= 0) {
            *nextX = lastX;
            *nextY = lastY - 1;
            return;
        }

        // Verificar hacia arriba
        if (lastX - 1 >= 0 && gameBoard->board[lastX - 1][lastY] >= 0) {
            *nextX = lastX - 1;
            *nextY = lastY;
            return;
        }
    }

    // Si no se cumple ninguna de las condiciones anteriores, se dispara aleatoriamente
    *nextX = rand() % gameBoard->size_x;
    *nextY = rand() % gameBoard->size_y;
}

// Función para ejecutar la estrategia de juego del proceso
void play_game(int fileDescriptor, int playerID, GameBoard *playerBoard, GameBoard *opponentBoard) {
    srand(time(NULL));

    int lastX = -1;
    int lastY = -1;

    while (1) {
        read_last_shot_result(fileDescriptor, getpid(), playerBoard);

        if (playerID == 1) {
            int nextX, nextY;
            determine_next_shot_coordinates(opponentBoard, lastX, lastY, &nextX, &nextY);

            printf("Jugador 1 dispara en (%d,%d)...\n", nextX, nextY);
            make_shot(opponentBoard, nextX, nextY);
            write_shot_result(fileDescriptor, getpid(), nextX, nextY, opponentBoard->board[nextX][nextY]);

            lastX = nextX;
            lastY = nextY;

            if (opponentBoard->board[nextX][nextY] == -3) {
                int isGameOver = 1;
                for (int i = 0; i < opponentBoard->size_x; i++) {
                    for (int j = 0; j < opponentBoard->size_y; j++) {
                        if (opponentBoard->board[i][j] > 0) {
                            isGameOver = 0;
                            break;
                        }
                    }
                    if (!isGameOver) {
                        break;
                    }
                }

                if (isGameOver) {
                    printf("¡Jugador 1 ha ganado el juego!\n");
                    break;
                }
            }
        } else if (playerID == 2) {
            int nextX, nextY;
            determine_next_shot_coordinates(opponentBoard, lastX, lastY, &nextX, &nextY);

            printf("Jugador 2 dispara en (%d,%d)...\n", nextX, nextY);
            make_shot(opponentBoard, nextX, nextY);
            write_shot_result(fileDescriptor, getpid(), nextX, nextY, opponentBoard->board[nextX][nextY]);

            lastX = nextX;
            lastY = nextY;

            if (opponentBoard->board[nextX][nextY] == -3) {
                int isGameOver = 1;
                for (int i = 0; i < opponentBoard->size_x; i++) {
                    for (int j = 0; j < opponentBoard->size_y; j++) {
                        if (opponentBoard->board[i][j] > 0) {
                            isGameOver = 0;
                            break;
                        }
                    }
                    if (!isGameOver) {
                        break;
                    }
                }

                if (isGameOver) {
                    printf("¡Jugador 2 ha ganado el juego!\n");
                    break;
                }
            }
        }

        sleep(1);
    }
}

void destroy_game_board(GameBoard *board) {
    for (int i = 0; i < board->size_x; i++) {
        free(board->board[i]);
    }
    free(board->board);
    free(board);
}


int main() {
    // Se crean los tableros de juego para cada jugador
    GameBoard player1Board, player2Board;

    // Se cargan los tableros de juego desde el archivo de configuración
    load_game_boards(&player1Board, &player2Board, "config.txt");

    // Se crea el archivo de intercambio para los disparos
    int fileDescriptor = open("shots.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fileDescriptor == -1) {
        perror("Error al crear el archivo de disparos");
        exit(1);
    }

    // Se crean los procesos para cada jugador
    pid_t player1PID = fork();
    if (player1PID == 0) {
        // Proceso del Jugador 1
        play_game(fileDescriptor, 1, &player1Board, &player2Board);
        exit(0);
    } else if (player1PID > 0) {
        pid_t player2PID = fork();
        if (player2PID == 0) {
            // Proceso del Jugador 2
            play_game(fileDescriptor, 2, &player2Board, &player1Board);
            exit(0);
        } else if (player2PID > 0) {
            // Proceso del Juez
            wait(NULL);
            wait(NULL);

            // Se cierra el archivo de intercambio
            close(fileDescriptor);

            // Se eliminan los tableros de juego
            destroy_game_board(&player1Board);
            destroy_game_board(&player2Board);
        } else {
            perror("Error al crear el proceso del Jugador 2");
            exit(1);
        }
    } else {
        perror("Error al crear el proceso del Jugador 1");
        exit(1);
    }

    return 0;
}
