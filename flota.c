#include<stdio.h>
#include <unistd.h>
#include <sys/types.h>
/* 
0 = agua sin tocar
1 = agua tocada
2 = portaviones 
3 = portaviones tocado
4 = bombardero
5 = bombardero tocado
6 = fragata
7 = fragata tocado
8 = hundido
*/


void generar_mapas(){
    int x;
    int y;
    int exigir = 1;
    while (exigir == 1){
    printf("Escribe la longitud del mapa en X \n");
    scanf("%d", &x);
    printf("Escribe la longitud del mapa en Y \n");
    scanf("%d", &y);

    if (x < 5 || y < 5){
        printf("ta mal");

    }else {
        exigir = 0;
        }
    }
    int mapa1[x][y];
    int mapa2[x][y];
    char direccion;
    int x2;
    int y2;

    printf("------------------------------------------------------------Turno de rellenar el jugador 1------------------------------\n");
    // bombardeen caracas
    printf("Escribe la coordenada del bombardero en X \n");
    scanf("%d", &x2);
    printf("Escribe la coordenada del bombardero en Y \n");
    scanf("%d", &y2);
    fflush(stdout); // Limpiar el buffer de salida
    printf("Escribe v si lo quieres colocar en vertical o h si es en horizontal \n");
    
    scanf(" %c", &direccion);
    switch (direccion)
    {
    case 'h':
        int i;
        for (i = 0; i < 3; i++) {
            if(mapa1[x2 + i][y2] == 2){
                printf("Mal hecho cruck");
                break;
            }
            else{
            mapa1[x2 + i][y2] = 4;}
        }
        break;
    case 'v':
         int i2;
        for (i2 = 0; i2 < 3; i2++) {
            if(mapa1[x2 + i2][y2] == 2){
                printf("Mal hecho cruck");
                break;
            }
            else{
            mapa1[x2][y+ i2]= 4;}
        }
        break;

    default:
        break;
    }
 // fragatas
    int cordx;
    int cordy;
    printf("Escribe la coordenada de la fragata en X \n");
    scanf("%d", &cordx);
    printf("Escribe la coordenada de la fragata en Y \n");
    scanf("%d", &cordy);
    fflush(stdout); // Limpiar el buffer de salida
    printf("Escribe v si lo quieres colocar en vertical o h si es en horizontal \n");
    scanf(" %c", &direccion);
    switch (direccion)
    {
    case 'h':
        int i;
        for (i = 0; i < 2; i++) {
            if(mapa1[cordx + i][cordy] == 4){
                printf("Mal hecho cruck");
                break;
            }
            else{
            mapa1[cordx + i][cordy] = 6;}
        }
        break;
    case 'v':
         int i3;
        for (i3 = 0; i3 < 2; i3++) {
            if(mapa1[cordx + i3][cordy] == 4){
                printf("Mal hecho cruck");
                break;
            }
            else{
            mapa1[cordx][cordy+ i3] = 6;}
        }
        break;

    default:
        break;
    }

    // portamos tu coche
    int portx;
    int porty;
    printf("Escribe la coordenada del portaviones en X \n");
    scanf("%d", &portx);
    printf("Escribe la coordenada de la portaviones en Y \n");
    scanf("%d", &porty);
    fflush(stdout); // Limpiar el buffer de salida
    printf("Escribe v si lo quieres colocar en vertical o h si es en horizontal \n");
    scanf(" %c", &direccion);
    switch (direccion)
    {
    case 'h':
        int i;
        for (i = 0; i < 4; i++) {
            if(mapa1[portx + i][porty] == 4||mapa1[portx + i][porty] == 6){
                printf("Mal hecho cruck");
                break;
            }
            else{
            mapa1[portx + i][porty] = 2;}
        }
        break;
    case 'v':
         int i3;
        for (i3 = 0; i3 < 4; i3++) {
            if(mapa1[portx + i3][porty] == 4 || mapa1[portx + i][porty] == 6){
                printf("Mal hecho cruck");
                break;
            }
            else{
            mapa1[portx][porty+ i3] = 2;}
        }
        break;

    default:
        break;
    }
printf("------------------------------------------------------------Turno de rellenar el jugador 2------------------------------\n");
int x3;
int y3;
   // bombardeen caracas
    printf("Escribe la coordenada del bombardero en X \n");
    scanf("%d", &x3);
    printf("Escribe la coordenada del bombardero en Y \n");
    scanf("%d", &y3);
    fflush(stdout); // Limpiar el buffer de salida
    printf("Escribe v si lo quieres colocar en vertical o h si es en horizontal \n");
    
    scanf(" %c", &direccion);
    switch (direccion)
    {
    case 'h':
        int i;
        for (i = 0; i < 3; i++) {
            if(mapa2[x3 + i][y3] == 2){
                printf("Mal hecho cruck");
                break;
            }
            else{
            mapa2[x3 + i][y3] = 4;}
        }
        break;
    case 'v':
         int i2;
        for (i2 = 0; i2 < 3; i2++) {
            if(mapa2[x3 + i2][y3] == 2){
                printf("Mal hecho cruck");
                break;
            }
            else{
            mapa2[x3][y+ i2]= 4;}
        }
        break;

    default:
        break;
    }
 // fragatas
    int cordx2;
    int cordy2;
    printf("Escribe la coordenada de la fragata en X \n");
    scanf("%d", &cordx2);
    printf("Escribe la coordenada de la fragata en Y \n");
    scanf("%d", &cordy2);
    fflush(stdout); // Limpiar el buffer de salida
    printf("Escribe v si lo quieres colocar en vertical o h si es en horizontal \n");
    scanf(" %c", &direccion);
    switch (direccion)
    {
    case 'h':
        int i;
        for (i = 0; i < 2; i++) {
            if(mapa2[cordx2 + i][cordy2] == 4){
                printf("Mal hecho cruck");
                break;
            }
            else{
            mapa2[cordx2 + i][cordy2] = 6;}
        }
        break;
    case 'v':
         int i3;
        for (i3 = 0; i3 < 2; i3++) {
            if(mapa2[cordx2 + i3][cordy2] == 4){
                printf("Mal hecho cruck");
                break;
            }
            else{
            mapa2[cordx2][cordy2+ i3] = 6;}
        }
        break;

    default:
        break;
    }

    // portamos tu coche
    int portx2;
    int porty2;
    printf("Escribe la coordenada del portaviones en X \n");
    scanf("%d", &portx2);
    printf("Escribe la coordenada de la portaviones en Y \n");
    scanf("%d", &porty2);
    fflush(stdout); // Limpiar el buffer de salida
    printf("Escribe v si lo quieres colocar en vertical o h si es en horizontal \n");
    scanf(" %c", &direccion);
    switch (direccion)
    {
    case 'h':
        int i;
        for (i = 0; i < 4; i++) {
            if(mapa2[portx2 + i][porty2] == 4||mapa2[portx2 + i][porty2] == 6){
                printf("Mal hecho cruck");
                break;
            }
            else{
            mapa2[portx2 + i][porty2] = 2;}
        }
        break;
    case 'v':
         int i3;
        for (i3 = 0; i3 < 4; i3++) {
            if(mapa2[portx2 + i3][porty2] == 4 || mapa2[portx2 + i][porty2] == 6){
                printf("Mal hecho cruck");
                break;
            }
            else{
            mapa2[portx2][porty2+ i3] = 2;}
        }
        break;

    default:
        break;
    }
    // rellenar el arenero de agua
    int itx;
    int ity;
    for (ity= 0; ity< y; ity++) {
        for (itx= 0; itx< x; itx++) {
            if(mapa1[itx][ity] == 2||mapa1[itx][ity] == 4||mapa1[itx][ity] == 6){}
            else{mapa1[itx][ity] = 0;}
            
    }}
    for (ity= 0; ity< y; ity++) {
        for (itx= 0; itx< x; itx++) {
            if(mapa2[itx][ity] == 2||mapa2[itx][ity] == 4||mapa2[itx][ity] == 6){}
            else{mapa2[itx][ity] = 0;}
            
    }}
// escribimos los mapas en txt

        FILE *archivo = fopen("mapas.txt", "w");
    
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
    
    fprintf(archivo, "Mapa 1:\n");
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            fprintf(archivo, "%d ", mapa1[j][i]);
        }
        fprintf(archivo, "\n");
    }
    
    fprintf(archivo, "\nMapa 2:\n");
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            fprintf(archivo, "%d ", mapa2[j][i]);
        }
        fprintf(archivo, "\n");
    }
    
    fclose(archivo);
    printf("Los mapas se han escrito correctamente en el archivo 'mapas.txt'.\n");
}

void mostrar_mapas() {
    FILE *archivo = fopen("mapas.txt", "r");
    
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
    
    char caracter;
    while ((caracter = fgetc(archivo)) != EOF) {
        printf("%c", caracter);
    }
    
    fclose(archivo);
}

int main() {
    generar_mapas();
    mostrar_mapas();
    
    return 0;

}
