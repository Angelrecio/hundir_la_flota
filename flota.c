#include<stdio.h>
/* 
0 = agua sin tocar
1 = agua tocada
2 = portaviones 
3 = portaviones hit
4 = bombardero
5 = bombardero hit
6 = fragata
7 = fragata hit
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
    //predeterminado para testing
    // portaavion
    mapa2[3][2] = 6;
    mapa2[3][3] = 6;
    mapa2[3][4] = 6;
    mapa2[3][5] = 6;
    // bombardero
    mapa2[0][2] = 4;
    mapa2[1][2] = 4;
    mapa2[2][2] = 4;
     // fragata
    mapa2[5][5] = 2;
    mapa2[5][4] = 2;
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
int main() {
    generar_mapas();
    return 0;

}

