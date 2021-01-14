#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cabeceras.h"
#define LONGITUD_COMANDO 100

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps);
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2);
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup);
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, 
              char *nombre);
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos);
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, 
              char *nombreantiguo, char *nombrenuevo);
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, 
             EXT_DATOS *memdatos, char *nombre);
int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
           EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
           char *nombre,  FILE *fich);
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
           EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
           EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich);
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich);
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich);
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich);
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich);


int  BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre){
	int x;
	int p=0;
	for (x = 1; x <= sizeof(directorio); x++) {
		if (strcmp(nombre,directorio[x].dir_nfich)==0) {
			p=x;
		}
	} 
	return p;
}

void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup){
	printf("Bloque %d Bytes\n", psup->s_block_size);
	printf("Inodos particion = %d\n", psup->s_inodes_count);
	printf("Inodos libres = %d\n", psup->s_free_inodes_count);
	printf("Bloques particion = %d\n", psup->s_blocks_count);
	printf("Bloques libres = %d\n", psup->s_free_blocks_count);
	printf("Primer bloque de datos = %d \n", psup->s_first_data_block);
}

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps) {
	int x, y;

	printf("Inodos :");
	for (y = 0; y < MAX_INODOS; y++) {
		printf("%d ", ext_bytemaps->bmap_inodos[y]);
	}

	printf("\nBloques [0-25] :");
	for (x = 0; x < 25; x++) {
		printf("%d ", ext_bytemaps->bmap_bloques[x]);
	}

	printf("\n");
}

void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos) {			// 20 ficheros 1- raiz
	int x, y;

	for (x = 1;x < MAX_FICHEROS ; x++) {		
		if(directorio[x].dir_inodo != NULL_INODO){// hasta 0xffff para evitar basura
			printf("%s \t", directorio[x].dir_nfich);
			printf(" tamaño: %d \t", inodos->blq_inodos[directorio[x].dir_inodo].size_fichero);
			printf(" inodo: %d \t", directorio[x].dir_inodo);
			printf(" Bloques:");
			for (y = 0; y < MAX_NUMS_BLOQUE_INODO; y++){
				if(inodos->blq_inodos[directorio[x].dir_inodo].i_nbloque[y] != NULL_BLOQUE){
					printf(" %d", inodos->blq_inodos[directorio[x].dir_inodo].i_nbloque[y]);
				}	
			}printf("\n");
		}
	}
}


int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo){
	int p1=BuscaFich(directorio, inodos,nombreantiguo);
	int p2=BuscaFich(directorio, inodos,nombrenuevo);

	if(p1==0){		  
		printf("ERROR: Fichero %s no encontrado\n", nombreantiguo);
		return -1;
	}
	
	if(p2!=0){		 
		printf("ERROR: El fichero  %s ya existe \n", nombrenuevo);
		return -1;
	}
		
	strcpy(directorio[p1].dir_nfich, nombrenuevo);
	return 0; 
}


int Imprimir(EXT_ENTRADA_DIR* directorio, EXT_BLQ_INODOS* inodos, EXT_DATOS* memdatos, char* nombre) {
	int x;
	int aux = 0;
	EXT_DATOS texto[MAX_NUMS_BLOQUE_INODO];
	int p2= BuscaFich(directorio, inodos, nombre);
	if(p2==0){
		printf("ERROR: Fichero %s no encontrado\n", nombre);
		return -1;
	}
	for(x = 0; x < MAX_NUMS_BLOQUE_INODO && inodos->blq_inodos[directorio[p2].dir_inodo].i_nbloque !=0 ; x++){
		if(inodos->blq_inodos[directorio[p2].dir_inodo].i_nbloque[x] != NULL_BLOQUE){
			strcpy(&texto[x], &memdatos[inodos->blq_inodos[directorio[p2].dir_inodo].i_nbloque[x]- PRIM_BLOQUE_DATOS]);
		}	
	}
	memcpy(&texto[x], "\0", 1);
	puts(texto);
	return 0;
}

int Copiar(EXT_ENTRADA_DIR* directorio, EXT_BLQ_INODOS* inodos, EXT_BYTE_MAPS* ext_bytemaps, EXT_SIMPLE_SUPERBLOCK* ext_superblock, EXT_DATOS* memdatos, char* nombreorigen, char* nombredestino, FILE* fich) {
	int p1 = 0;
	int p2 = 0;
	int x, j, w,t ;
	int aux;
	
	p1 = BuscaFich(directorio, inodos, nombreorigen);
	p2 = BuscaFich(directorio, inodos, nombredestino);

	if (p1 == 0) {
		printf("ERROR: Fichero %s no encontrado\n", nombreorigen);
		return -1;
	}
	if (p2 != 0) {
		printf("ERROR: El fichero  %s ya existe \n", nombredestino);
		return -1;
	}
	
 
	aux=inodos->blq_inodos[directorio[x].dir_inodo].i_nbloque[0]; // cogemos el primer bloque ocupado
	
	
	for (x = 1; x < sizeof(directorio); x++) {			// ver si es dsp del fichero anterior o no
		if (directorio[x].dir_nfich == NULL) {															// cambiar
			strcpy(directorio[x].dir_nfich, nombredestino);			// copiamos nombre en el array de 
			inodos->blq_inodos[directorio[x].dir_inodo].size_fichero = inodos->blq_inodos[directorio[p1].dir_inodo].size_fichero;
			for(j=p2 ; j< MAX_INODOS; j++){							// j = entrada indo fichero viejo 
				if(ext_bytemaps->bmap_inodos[j]==0){
					directorio[x].dir_inodo= j;			// guardamos siguiente inodo
					ext_bytemaps->bmap_inodos[j]=1;
				}
			}
			for(w=aux; w< MAX_BLOQUES_PARTICION; w++){				// empiza por el primer bloq ocupado 
				if(ext_bytemaps->bmap_bloques[w] == 0){
					ext_bytemaps->bmap_bloques[w]=1;
					inodos->blq_inodos[directorio[x].dir_inodo].i_nbloque[w];		// añdir los datos a los bloques correspondientes
				}
			}
		}
	}
	
	
	return 0;
}


int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombre,  FILE *fich){
	int p2=BuscaFich(directorio, inodos,nombre);
	int x=0;
	int aux1=0;
	int aux2=0;
	int j;
	
	if(p2==0){
		printf("ERROR: Fichero %s no encontrado \n", nombre);	
		return -1;
	}
	
 	ext_bytemaps->bmap_inodos[p2]=0;					// eliminamos inodo de la entrada de bytemaps
	
	for(j=0; j< MAX_NUMS_BLOQUE_INODO && inodos->blq_inodos[directorio[p2].dir_inodo].i_nbloque !=0; j++){		// bytemaps de bloques 
		if(inodos->blq_inodos[directorio[p2].dir_inodo].i_nbloque[j] != NULL_BLOQUE){
			aux2=inodos->blq_inodos[directorio[p2].dir_inodo].i_nbloque[j]; 
			ext_bytemaps->bmap_bloques[aux2]=NULL;								 
			inodos->blq_inodos[directorio[p2].dir_inodo].i_nbloque[j]=5;	
		}
	}
	strcpy(directorio[p2].dir_nfich,"");				
	directorio[p2].dir_inodo=NULL;							 
	inodos->blq_inodos[directorio[p2].dir_inodo].size_fichero= 0;

	return 0;
}


int ComprobarComando(char* strcomando, char* orden, char* argumento1, char* argumento2) { // strcomando todo lo que pasa al buffer
    char *delim;

    delim = strtok(strcomando, "\n");				// eliminar /n fgets 
    delim = strtok(strcomando, " ");
    strcpy(orden, delim);						// copiar orden sin argumentos 
	delim = strtok(NULL, " ");
     
    if(delim != NULL){						// 1 argumento 
       strcpy(argumento1, delim);
       delim = strtok(NULL, " ");			// 2 argumetos
       if(delim != NULL){
    	    strcpy(argumento2, delim);
        }
  	}

    return 0;
}

void Procs_salir() {
	exit(-1);
}

int main() {
	char* comando[LONGITUD_COMANDO];
	char* orden[LONGITUD_COMANDO];
	char* argumento1[LONGITUD_COMANDO];
	char* argumento2[LONGITUD_COMANDO];
	int  j;
	unsigned long int m;
	int entradadir, grabardatos;
	FILE* fent;
	fent = fopen("particion.bin", "r+b");


	if (fent == NULL) {
		printf("No se ha podido cargar el fichero\n");
		exit(-1);
	}

	EXT_SIMPLE_SUPERBLOCK ext_superblock;
	EXT_BYTE_MAPS ext_bytemaps;
	EXT_BLQ_INODOS ext_blq_inodos;
	EXT_ENTRADA_DIR directorio[MAX_FICHEROS];
	EXT_DATOS memdatos[MAX_BLOQUES_DATOS];
	EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];

	fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fent);

	memcpy(&ext_superblock, (EXT_SIMPLE_SUPERBLOCK*)&datosfich[0], SIZE_BLOQUE);
	memcpy(&directorio, (EXT_ENTRADA_DIR*)&datosfich[3], SIZE_BLOQUE);
	memcpy(&ext_bytemaps, (EXT_BLQ_INODOS*)&datosfich[1], SIZE_BLOQUE);
	memcpy(&ext_blq_inodos, (EXT_BLQ_INODOS*)&datosfich[2], SIZE_BLOQUE);
	memcpy(&memdatos, (EXT_DATOS*)&datosfich[4], MAX_BLOQUES_DATOS * SIZE_BLOQUE);
	
  	for (;;) {
		do {
			printf(">> ");
			fflush(stdin);
			fgets(comando, LONGITUD_COMANDO, stdin);
		} while (ComprobarComando(comando, orden, argumento1, argumento2) != 0);
		    if (strcmp(orden,"dir")==0){
	           Directorio(directorio, &ext_blq_inodos);
	           continue;
			}
			else if(strcmp(orden,"info")==0){
			    LeeSuperBloque(&ext_superblock);
				continue;
			 }
	        else if(strcmp(orden,"bytemaps")==0){
				Printbytemaps(&ext_bytemaps);
				continue;
			}
			else if(strcmp(orden,"rename")==0){
				Renombrar(directorio, &ext_blq_inodos, argumento1, argumento2);
				continue;
			}
			else if(strcmp(orden,"imprimir")==0){
				Imprimir(directorio, &ext_blq_inodos, memdatos, argumento1);
				continue;
			}
			else if(strcmp(orden,"remove")==0){
				Borrar(directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, argumento1,  fent);
				continue;
			}	 
			else if(strcmp(orden,"copy")==0){
				Copiar(directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, memdatos, argumento1, argumento2,  fent);
				continue;
			}
	        else if (strcmp(orden,"salir")==0){
	            //GrabarDatos(&memdatos,fent);
	            fclose(fent);
	       	    return 0;
	        }													// grabar datos despues de metodos modificadores
			else{
				printf("ERROR: Comando ilegal [info, bytemaps, dir, rename, imprimir, remove, copy, salir]\n");
				continue;
			} 
	}
}
