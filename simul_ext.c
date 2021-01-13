
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
	for (x = 1; x < sizeof(directorio); x++) {
		if (strcmp(nombre, directorio[x].dir_nfich) == 0) {
			return 1;
		}
		else if (strcmp(nombre, directorio[x].dir_nfich)!=0) {
			return 	0;
		}
		else {
			return x;
		}
	} 

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

	for (x = 1;x < MAX_FICHEROS && directorio[x].dir_inodo != 0xffff; x++) {		// hasta 0xffff para evitar basura
		printf("%s \t", directorio[x].dir_nfich);
		printf(" tamaño: %d \t", inodos->blq_inodos[directorio[x].dir_inodo].size_fichero);
		printf(" inodo: %d \t", directorio[x].dir_inodo);
		printf(" Bloques:");
		for (y = 0; y < MAX_NUMS_BLOQUE_INODO && inodos->blq_inodos[directorio[x].dir_inodo].i_nbloque[y] != 0xffff; y++) {
			printf(" %d", inodos->blq_inodos[directorio[x].dir_inodo].i_nbloque[y]);
		}				printf("\n");
	}
}

int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo) {
	int p1, p2;
	int x,y;
	
	p2 = BuscaFich(directorio, inodos, nombreantiguo);
	p1 = BuscaFich(directorio, inodos, nombrenuevo);

	if (p2 == 0) {
		printf("\n ERROR: Fichero %s no encontrado", nombreantiguo);
	}
	else {
		if (p1 == 0) {
			printf("ERROR: El fichero %s ya existe \n", nombrenuevo);
		}
		else {
			strcpy(directorio->dir_nfich, nombrenuevo);
		}
	}

	printf("\n");
	
	return 0;
}

int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre) {
	/*
	int p1;
	int x,y,bloque;
	int z;
	
	p1 = BuscaFich(directorio, inodos, nombre,z);

	if (p1 == 1) {							// imprimir datos 
		for(x=0 ; x< MAX_NUMS_BLOQUE_INODO; x++){
			bloque=inodos->blq_inodos[directorio[z].dir_inodo].i_nbloque[x];
			for(y=0 ;sizeof(datos[bloque].dato[y]]; y++){
				printf("%c", datos[bloque]dato[y]);	
			}
		}
	}
	else {
		printf("\n ERROR: Fichero %s no encontrado", nombre);
	}
	return 0;
	*/
}

int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich) {
	int p1;
	int p2;

	p1 = BuscaFich(directorio, inodos, nombreorigen);

	if (p1 == 1) {							// copiar

	}
	else {
		printf("\n ERROR: Fichero %s no encontrado", nombreorigen);
	}
	return 0;
}

int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombre,  FILE *fich){
	int num_fichero, aux, x;
	 
/*	aux= directorio[num_fichero].dir_inodo;
	strcpy(directorio[num_fichero].dir_nfich,"");
	directorio[num_fichero].dir_inodo=0xffff;
	inodos->blq_inodos[aux].size_fichero=0;
	// vaciar bloques 0xffff*/
	return 0;
}


int ComprobarComando(char* strcomando, char* orden, char* argumento1, char* argumento2) { // strcomando todo lo que pasa al buffer
    char *token= strtok(strcomando, " ");
 
     if(strcmp(strcomando, " ")==0){
         strcpy(strcomando, argumento1);
        if(token != NULL){
            token(strtok(NULL, " "));
            strcpy(strcomando, argumento2);
        }
    }else {
        strcpy(strcomando, orden);
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
     for (;;){
			do{
		 	printf ("\n>> ");
			fflush(stdin);
			fgets(comando, LONGITUD_COMANDO, stdin);
			}while (ComprobarComando(comando, orden, argumento1, argumento2) != 0);
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
		/*	// Escritura de metadatos en comandos rename, remove, copy     
			Grabarinodosydirectorio(&directorio, &ext_blq_inodos, fent);			// check
			GrabarByteMaps(&ext_bytemaps, fent);
			GrabarSuperBloque(&ext_superblock, fent);
			if (grabardatos)
				GrabarDatos(&memdatos, fent);
			grabardatos = 0;
			//Si el comando es salir se habrÃ¡n escrito todos los metadatos
			//faltan los datos y cerrar */
	        else if (strcmp(orden,"salir")==0){
	            //GrabarDatos(&memdatos,fent);
	            fclose(fent);
	       	    return 0;
	        }
			else{
				printf("ERROR: Comando ilegal [info, bytemaps, dir, rename, imprimir, remove, copy, salir]\n");
				continue;
			} 
		}
	}
}
