//---------------------------------------------------------------
// Menú.c
//
// Autores: Rafael López Martínez
//          Alfredo Álvarez Senra
//---------------------------------------------------------------


//---------------------------
//INCLUSION DE OTROS FICHEROS
//---------------------------

#include "teclado.c"

#include "m5272lib.c"
#include "m5272gpio.c"
#include "m5272.h"




//-------------------------
//DEFINICIÓN DE CONSTANTES
//-------------------------

#define TODOS_FILTROS 0		// variable que indica que están seleccionados todos los filtros a la vez

#define FUNCION 0		
#define CARACTERIZACION 1
#define ECUALIZACION 2
#define REVERBERACION 3
#define MUESTRAENTRADA 4
#define MUESTRAECUALIZADOR 5
#define ATTREVERB 6
#define TIEMPORETARDO 7



//------------------------------------------
//DECLARACIÓN DE VARIABLES Y ARRAYS GENERALES
//------------------------------------------

int estado[8]={0,0,0,0,0,0,0,0};// array que indica el estado en el que está, es decir, que opciones se han seleccionado en el menú 
				// PRIMERA POSICIÓN (funcion):(1) gestión caract, (2) gestión ecualiz...
				// SEGUNDA POSICIÓN (filtro seleccionado):(1) filtro 1, (2) filtro 2, ... , (0) todos los filtros
				// TERCERA POSICIÓN (banda seleccionada para ecualizar): (1) banda 32 Hz, (2) banda 64 Hz, ...
				// CUARTA POSICIÓN (estado reverberacion): (1) activada, (0) desactivada.
				// QUINTA POSICIÓN (muestra entrada/salida): (0) muestra salida, (1) muestra entrada
				// SEXTA POSICIÓN (muestra ecualizador grafico/ energia): (0) muestra energia (1) muestra ecualizador grafico
				// SEPTIMA POSICIÓN (nivel atenuación de la reverberación)
				// OCTAVA POSICIÓN (tiempo retardo de la reverberacióm).
				
int flagsVolver[2] = {0,0};	// dos flags utilizados para volver atrás en los submenús.			

// VARIABLES DE ECUALIZACIÓN
int ganancia[16] = {1024,790,610,471,364,281,217,167,129,99,77,59,46,35,27,21};		// valores de las ganancias que se aplicaran en la ecualización.
int nivel[7] = {0,0,0,0,0,0,0};									// nivel de ganancia de las 7 bandas de frecuencia. nivel 0 = 1024, nivel 15 = 21
int nivelesGuardados[9][7] = {{0,0,0,0,0,0,0},							// array que funciona como memoria en la opción de guardar la configuración. Es posible guardar 9 configuraciones.
			     {0,0,0,0,0,0,0},
			     {0,0,0,0,0,0,0},
			     {0,0,0,0,0,0,0},
			     {0,0,0,0,0,0,0},
			     {0,0,0,0,0,0,0},
			     {0,0,0,0,0,0,0},
			     {0,0,0,0,0,0,0},
	 		     {0,0,0,0,0,0,0}};
	


// VARIABLES DE REVERBERACIÓN
int reverbGuardadas[9][2] = {{0,0},								// array que funciona como memoria en la opción de guardar la configuración. Es posible guardar 9 configuraciones.
			     {0,0},
			     {0,0},
			     {0,0},
			     {0,0},
			     {0,0},
			     {0,0},
			     {0,0},
	 		     {0,0}};


//----------------------
//DECLARACIÓN DE MÉTODOS
//----------------------

void GestionMenuPrincipal(void);

void GestionCaracterizacion(void);

void GestionEcualizacion(void);
void seleccionNivel(int nivel);
void seleccionPreset(void);
void muestraEstadoEcualizacion(void);

void GestionReverb(void);
void selectAtenuacion(void);
void selectRetardo(void);


//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------
// void GestionMenuPrincipal(void)
// Descripción:
//   Muestra el menú principal y nos permite escoger una de las
//   tres opciones: CARACTERIZACIÓN DE FILTROS, ECUALIZACIÓN GRÁFICA,
//                  INCORPORACIÓN DE REVERBERACION SIMPLE.
//------------------------------------------------------
void GestionMenuPrincipal (void){
	char tecla;
	estado[FUNCION] = 0;					// inicializa el array estado a cero
      	estado[CARACTERIZACION] = 0;
      	estado[ECUALIZACION] = 0;
      	estado[REVERBERACION] = 0;
	estado[MUESTRAENTRADA] = 0;
	estado[MUESTRAECUALIZADOR] = 0;
	estado[ATTREVERB] = 0;
	estado[TIEMPORETARDO] = 0;

	output ("-------------------------------------------------------------------\n");
	output ("|  BIENVENIDO AL SISTEMA DE PROCESAMIENTO DIGITAL DE AUDIO LT-21  |\n");
	output ("-------------------------------------------------------------------\n\n");
      	output ("Seleccione una de las siguientes opciones:\n");		
      	output ("\t1 - Caracterización de filtros.\n");
      	output ("\t2 - Ecualizador gráfica.\n");
      	output ("\t3 - Incorporación de reverberación.\n");
      	tecla = teclado();
      	switch(tecla){
         	case '1': 
			output ("Ha seleccionado: ");
			output ("CARACTERIZACIÓN DE FILTROS\n\n");
	    		estado[FUNCION] = CARACTERIZACION;
			while(estado[FUNCION] == CARACTERIZACION){
   	         		GestionCaracterizacion();
			}             		  
            		break;
        	case '2': 
			output ("Ha seleccionado: ");
			output ("ECUALIZACIÓN GRÁFICA\n\n");
	    		estado[FUNCION] = ECUALIZACION;
			while (estado[FUNCION] == ECUALIZACION){
           	 		GestionEcualizacion();
			}
           		break;
         	case '3': 
			output ("Ha seleccionado: ");
			output ("INCORPORACIÓN DE REVERBERACION SIMPLE\n\n");
            		estado[FUNCION] = REVERBERACION;
            		while(estado[FUNCION] == REVERBERACION){
				GestionReverb();
			}
            		break;
         	default: 
			output ("Tecla no válida.\n\n\n");				
            		break;	
	}
}





//-----------------------------
// SUBMENÚ DE CARACTERIZACIÓN
//-----------------------------



//------------------------------------------------------
// void GestionCaracterizacion(void)
// Descripción:
//   Muestra el submenú de "Caracterizacón principal" y nos permite
//   seleccionar el filtro (individualmente o todo el banco de filtros)
//   que queremos utilizar
//------------------------------------------------------
void GestionCaracterizacion(void){
	char tecla;
	
	output ("\t1,7 - Seleccione el filtro que quiere caracterizar (1-7).\n");
	output ("\tF - Todos los filtros.\n");
      	output ("\tA - Volver.\n\n");
	output ("\tB - VúMETRO: Mostrar salida/Mostrar entrada.\n\n");

      	tecla = teclado();
      	switch(tecla){
        	case '1':
			output ("Ha seleccionado: ");
			output ("FILTRO 1 \n\n");
			estado[CARACTERIZACION]= 1;
			break;
		case '2':
			output ("Ha seleccionado: ");
			output ("FILTRO 2 \n\n");
			estado[CARACTERIZACION]= 2;
			break;
		case '3':
			output ("Ha seleccionado: ");
			output ("FILTRO 3 \n\n");
			estado[CARACTERIZACION]= 3;
			break;
		case '4':
			output ("Ha seleccionado: ");
			output ("FILTRO 4 \n\n");
			estado[CARACTERIZACION]= 4;
			break;
		case '5':
			output ("Ha seleccionado: ");
			output ("FILTRO 5 \n\n");
			estado[CARACTERIZACION]= 5;
			break;
		case '6':
			output ("Ha seleccionado: ");
			output ("FILTRO 6 \n\n");
			estado[CARACTERIZACION]= 6;
			break;
		case '7':
			output ("Ha seleccionado: ");
			output ("FILTRO 7 \n\n");
			estado[CARACTERIZACION]= 7;								  
	           	break;
		case 'F':
			output ("Ha seleccionado: ");
			output ("TODOS LOS FILTROS\n\n");
			estado[CARACTERIZACION]= TODOS_FILTROS;						
	         	break;
		case 'B':
			if (estado[MUESTRAENTRADA] == 0){
				estado[MUESTRAENTRADA]= 1;
				output ("VUMETRO: muestra entrada\n\n");
			}else {
				estado[MUESTRAENTRADA]= 0;
				output ("VUMETRO: muestra salida\n\n");
			}						
	         	break;                  
	        case 'A':
			estado[FUNCION] = 0;
			break;
		default:
			output ("Tecla no válida.\n\n");
	            	break;	
	}
}





//---------------------------
// SUBMENÚS DE ECUALIZACIÓN
//---------------------------



//-------------------------------------------------------------
// void GestionEcualizacion(void)
// Descripción:
//   Muestra el submenú de "Gestión ecualización" y nos permite
//   seleccionar la banda de frecuencias que queremos ecualizar,
//   seleccionar una configuración preestablecida, mostrar la
//   configuración actual o desactivarla.
//   Además siempre tendremos la opción de seleccionar que
//   queremos visualizar en la matriz de leds, la energía de
//   las bandas o el ecualizador gráfico.
//-------------------------------------------------------------
void GestionEcualizacion(void){		
	int i;
	char tecla;
	output ("Seleccione que ecualización desea implementar.\n");
	output ("\t1,7 - Ecualización personalizada: Seleccione la banda que quiere ecualizar (1-7).\n");
      	output ("\tF - Presets de ecualización.\n");
	output ("\tD - Guardar la configuración.\n");
	output ("\t0 - Mostrar estado actual.\n");
      	output ("\tC - Desactivar ecualización.\n");
      	output ("\tA - Volver.\n\n");
	output ("\tE - VÚMETRO: Mostrar ecualizador gráfico/Mostrar energía.\n");
	output ("\tB - VÚMETRO: Mostrar salida/Mostrar entrada.\n\n");

	tecla = teclado();
	switch(tecla){
        	case '1':
			output ("Ha seleccionado: ");
            		output ("Banda 32Hz \n\n");
		    	estado[ECUALIZACION]= 1;
			flagsVolver[0]= 0;
			while(flagsVolver[0] == 0){
				seleccionNivel(estado[ECUALIZACION]);
			}
			break;
        	case '2':
			output ("Ha seleccionado: ");
			output ("Banda 64Hz \n\n");
			estado[ECUALIZACION]= 2;
			flagsVolver[0]= 0;
			while(flagsVolver[0]== 0){
				seleccionNivel(estado[ECUALIZACION]);
			}
			break;
		case '3':
			output ("Ha seleccionado: ");
			output ("Banda 125Hz \n\n");
			estado[ECUALIZACION]= 3;
			flagsVolver[0]= 0;
			while(flagsVolver[0]== 0){
				seleccionNivel(estado[ECUALIZACION]);
			}
			break;
		case '4':
			output ("Ha seleccionado: ");
			output ("Banda 250Hz \n\n");
			estado[ECUALIZACION]= 4;
			flagsVolver[0]= 0;
			while(flagsVolver[0]== 0){
				seleccionNivel(estado[ECUALIZACION]);
			}
			break;
		case '5':
			output ("Ha seleccionado: ");
			output ("Banda 500Hz \n\n");
			estado[ECUALIZACION]= 5;
			flagsVolver[0]= 0;
			while(flagsVolver[0]== 0){
				seleccionNivel(estado[ECUALIZACION]);
			}
			break;
		case '6':
			output ("Ha seleccionado: ");
			output ("Banda 1kHz \n\n");
			estado[ECUALIZACION]= 6;
			flagsVolver[0]= 0;
			while(flagsVolver[0] == 0){
				seleccionNivel(estado[ECUALIZACION]);
			}
			break;
		case '7':
			output ("Ha seleccionado: ");
			output ("Banda 2kHz \n\n");
			estado[ECUALIZACION]= 7;
			flagsVolver[0]= 0;
			while(flagsVolver[0]== 0){
				seleccionNivel(estado[ECUALIZACION]);
			}	  
			break;
		case '0':
			muestraEstadoEcualizacion();
			break;
		case 'F':
			flagsVolver[0] = 0;
			while(flagsVolver[0] == 0){
				seleccionPreset();
			}					
	         	break;
		case 'D':
			output ("¿Dónde desea guardarlo? Seleccione una de las 9 posiciones de memoria.\n\n"); 
			tecla = teclado();
			while ((tecla < '1') || (tecla > '9')){
				output ("Tecla no válida\n\n");
				tecla = teclado();
			}
			output ("Ha seleccionado: ");
			outch (tecla);
			output("\n\n");
			nivelesGuardados[tecla-'0'][0] = nivel [0];			//guarda la configuración actual en la posicion de memoria seleccionada
			nivelesGuardados[tecla-'0'][1] = nivel [1];
			nivelesGuardados[tecla-'0'][2] = nivel [2];
			nivelesGuardados[tecla-'0'][3] = nivel [3];
			nivelesGuardados[tecla-'0'][4] = nivel [4];
			nivelesGuardados[tecla-'0'][5] = nivel [5];
			nivelesGuardados[tecla-'0'][6] = nivel [6];
			break;
		case 'C':
			output ("ECUALIZACIÓN DESACTIVADA\n\n");
			for(i=0; i<7; i++){				// para desactivarla reinicia el array nivel
				nivel[i]=0;
			}
			muestraEstadoEcualizacion();
			break;
		case 'E':
			if (estado[MUESTRAECUALIZADOR]== 0){
				estado[MUESTRAECUALIZADOR]= 1;
				output ("VUMETRO: muestra ecualizador gráfico\n\n");
			}else{
				estado[MUESTRAECUALIZADOR]= 0;
				output ("VUMETRO: muestra energía\n\n");
			}
			break;
		case 'B':
			if (estado[MUESTRAENTRADA]== 0){
				estado[MUESTRAENTRADA]= 1;
				output ("VUMETRO: muestra entrada\n\n");
			}else {
				estado[MUESTRAENTRADA]= 0;
				output ("VUMETRO: muestra salida\n\n");
			}						
	         	break;
		case 'A':
			estado[FUNCION] = 0;
			break;
		default: 
			output ("Tecla no válida.\n\n");
			break;
		}
}





//-------------------------------------------------------
// void seleccionNivel(int nivel)
// Descripcion:
//	Permite seleccionar el nivel de ecualización con
//	las teclas 8 y 9 (subir y bajar respectivamente)
//	y muestra el estado de ecualizacion de cada banda.
//	Además siempre tendremos la opción de seleccionar que
//	queremos visualizar en la matriz de leds, la energía de
//	las bandas o el ecualizador gráfico.
//-------------------------------------------------------

void seleccionNivel(int bandaNivel){
	char tecla;

	output ("Seleccione el nivel de ecualización: nivel 0 mínima atenuación - nivel 15 máxima atenuación\n");
	output ("\t8 - Subir nivel.\n");
	output ("\t9 - Bajar nivel.\n");
	output ("\tA - Volver.\n\n");
	output ("\tE - VÚMETRO: Mostrar ecualizador gráfico/Mostrar energía.\n");
	output ("\tB - VÚMETRO: Mostrar salida/Mostrar entrada.\n\n");
	tecla = teclado();
	switch(tecla){
		case '8':
			if (nivel[bandaNivel-1] < 15){
				nivel[bandaNivel-1]++;			// sube uno a uno la posción correspondiente al filtro seleccionado en el array nivel.
				muestraEstadoEcualizacion();
			}
			break;
		case '9':
			if (nivel[bandaNivel-1] > 0){
				nivel[bandaNivel-1]--;			// baja uno a uno la posción correspondiente al filtro seleccionado en el array nivel.
				muestraEstadoEcualizacion();
			}
			break;
		case 'E':
			if (estado[MUESTRAECUALIZADOR]== 0){
				estado[MUESTRAECUALIZADOR]= 1;
				output ("VUMETRO: muestra ecualizador gráfico\n\n");
			}else {
				estado[MUESTRAECUALIZADOR]= 0;
				output ("VUMETRO: muestra energía\n\n");
			}
			break;
		case 'B':
			if (estado[MUESTRAENTRADA]== 0){
				estado[MUESTRAENTRADA]= 1;
				output ("VUMETRO: muestra entrada\n\n");
			}else {
				estado[MUESTRAENTRADA]= 0;
				output ("VUMETRO: muestra salida\n\n");
			}						
	         	break;
		case 'A':
			flagsVolver[0]= 1;
			break;
		default:
			output ("Tecla no válida.\n");
			break;
	}
}




//-------------------------------------------------------------
// void seleccionPreset(void)
// Descripción:
//      Permite seleccionar uno de los presets de ecualización.
//      o cargar una de las configuraciones previamente guardada
//	en una de las posiciones de memoria.
//	Además siempre tendremos la opción de seleccionar que
//	queremos visualizar en la matriz de leds, la energía de
//	las bandas o el ecualizador gráfico.
//-------------------------------------------------------------
void seleccionPreset(void){
	char tecla;

	output ("Seleccione una de las siguientes opciones de ecualización:\n");
      	output ("\t1 - Rock.\n");
      	output ("\t2 - Rap.\n");
      	output ("\t3 - Grunge.\n");
      	output ("\t4 - Metal.\n");
      	output ("\t5 - Dance.\n");
      	output ("\t6 - Tecno.\n");
      	output ("\t7 - Jazz.\n");
      	output ("\t8 - Clásica.\n");
      	output ("\tD - Ecualizaciones guardadas.\n");
      	output ("\tA - Volver.\n\n");
	output ("\tE - VÚMETRO: Mostrar ecualizador gráfico/Mostrar energía.\n");
	output ("\tB - VÚMETRO: Mostrar salida/Mostrar entrada.\n\n");
	tecla = teclado();
	switch(tecla){
        	case '1':
            		output ("Ha seleccionado: ROCK \n\n");
			nivel[0] = 4;
			nivel[1] = 3;
			nivel[2] = 2;
			nivel[3] = 0;
			nivel[4] = 2;
			nivel[5] = 3;
			nivel[6] = 1;
			muestraEstadoEcualizacion();
			break;
        	case '2':
            		output ("Ha seleccionado: RAP \n\n");
			nivel[0] = 3;
			nivel[1] = 2;
			nivel[2] = 1;
			nivel[3] = 0;
			nivel[4] = 1;
			nivel[5] = 2;
			nivel[6] = 1;
			muestraEstadoEcualizacion();
			break;
		case '3':
            		output ("Ha seleccionado: GRUNGE \n\n");
			nivel[0] = 5;
			nivel[1] = 3;
			nivel[2] = 1;
			nivel[3] = 3;
			nivel[4] = 2;
			nivel[5] = 1;
			nivel[6] = 0;
			muestraEstadoEcualizacion();
			break;
		case '4':
            		output ("Ha seleccionado: METAL \n\n");
			nivel[0] = 5;
			nivel[1] = 3;
			nivel[2] = 2;
			nivel[3] = 2;
			nivel[4] = 1;
			nivel[5] = 1;
			nivel[6] = 0;
			muestraEstadoEcualizacion();
			break;
		case '5':
            		output ("Ha seleccionado: DANCE \n\n");
			nivel[0] = 4;
			nivel[1] = 2;
			nivel[2] = 0;
			nivel[3] = 3;
			nivel[4] = 4;
			nivel[5] = 3;
			nivel[6] = 1;
			muestraEstadoEcualizacion();
			break;
		case '6':
            		output ("Ha seleccionado: TECNO \n\n");
			nivel[0] = 6;
			nivel[1] = 3;
			nivel[2] = 1;
			nivel[3] = 3;
			nivel[4] = 4;
			nivel[5] = 3;
			nivel[6] = 1;
			muestraEstadoEcualizacion();
			break;
		case '7':
            		output ("Ha seleccionado: JAZZ \n\n");
			nivel[0] = 3;
			nivel[1] = 3;
			nivel[2] = 2;
			nivel[3] = 1;
			nivel[4] = 0;
			nivel[5] = 1;
			nivel[6] = 2;
			muestraEstadoEcualizacion();
			break;
		case '8':
            		output ("Ha seleccionado: CLÁSICA \n\n");
			nivel[0] = 4;
			nivel[1] = 0;
			nivel[2] = 1;
			nivel[3] = 2;
			nivel[4] = 3;
			nivel[5] = 4;
			nivel[6] = 4;
			muestraEstadoEcualizacion();
			break;
		case 'D':
			output ("¿Cuál desea cargar? Seleccione una de las 9 posiciones de memoria.\n\n");
			tecla = teclado();
			while ((tecla < '1') || (tecla > '9')){			//si pulsamos una tecla distinta del 1,2,3,4,5,6,7,8 o 9 nos indica el error y espera que pulsemos una tecla corracta
				output ("Tecla no válida\n\n");
				tecla = teclado();
			}
			output ("Ha seleccionado: ");
			outch (tecla);
			output("\n\n");
			nivel [0] = nivelesGuardados[tecla-'0'][0];
			nivel [1] = nivelesGuardados[tecla-'0'][1];
			nivel [2] = nivelesGuardados[tecla-'0'][2];
			nivel [3] = nivelesGuardados[tecla-'0'][3];
			nivel [4] = nivelesGuardados[tecla-'0'][4];
			nivel [5] = nivelesGuardados[tecla-'0'][5];
			nivel [6] = nivelesGuardados[tecla-'0'][6];
			muestraEstadoEcualizacion();
			break;
		case 'E':
			if (estado[MUESTRAECUALIZADOR]== 0){
				estado[MUESTRAECUALIZADOR]= 1;
				output ("VUMETRO: muestra ecualizador gráfico\n\n");
			}else {
				estado[MUESTRAECUALIZADOR] = 0;
				output ("VUMETRO: muestra energía\n\n");
			}
			break;
		case 'B':
			if (estado[MUESTRAENTRADA]== 0){
				estado[MUESTRAENTRADA]= 1;
				output ("VUMETRO: muestra entrada\n\n");
			}else {
				estado[MUESTRAENTRADA] = 0;
				output ("VUMETRO: muestra salida\n\n");
			}						
	         	break;
		case 'A':
			flagsVolver[0]= 1;
			break;
		default: 
			output ("Tecla no válida.\n\n");
			break;
		}
}





//-------------------------------------------------------
// void muestraEstadoEcualizacion(void)
// Descripcion:
//	Muestra una tabla con el estado de ecualizacion
//	de cada filtro.
//-------------------------------------------------------

void muestraEstadoEcualizacion(void){
	int i;
	output("----------------------------------------------------\n");
	output("BANDA:\t32Hz\t64Hz\t125Hz\t250Hz\t500Hz\t1kHz\t2kHz\n");
	output("GANANCIA:\t");
	for (i=0; i<7; i++){
		outNum(10, ganancia[nivel[i]],0);
		output("\t");
	}
	output("\n");
	output("NIVEL:\t");
	for (i=0; i<7; i++){
		outNum(10, nivel[i],0);
		output("\t");
	}
	output("\n----------------------------------------------------");
	output("\n\n");
}





//---------------------------
// SUBMENÚS DE REVERBERACIÓN
//---------------------------



//------------------------------------------------------
// void GestionReverb(void)
// Descripción:
//   Muestra el submenú de "Gestión de reverberación" y nos permite
//   activarla, mostrar la configuración actual, guardarla
//   en una de las 9 posiciones de memoria disponibles,cargar
//   una de las configuraciones previamente guardada o desactivar 
//   la función de reverberación
//------------------------------------------------------
void GestionReverb(void){
	char tecla;

	output ("Activación de la reverberación.\n");	
	output ("\t1 - Activar.\n");
	output ("\t2 - Desactivar.\n");
	output ("\t0 - Muestra configuración actual.\n");
	output ("\tD - Guardar configuración actual.\n");
	output ("\tE - Cargar una configuración previamente guardada.\n");
	output ("\tA - Volver.\n\n");
	output ("\tB - VÚMETRO: Mostrar salida/Mostrar entrada.\n\n");

	flagsVolver[0]= 0;
	tecla = teclado();
      	switch(tecla){
		case '1':
			while (flagsVolver[0]== 0){
				selectAtenuacion();
			}
			break;
		case '2':
			output ("Reverberación desactivada.\n\n");
			estado[REVERBERACION]= 0;
			estado[ATTREVERB] =0;
			estado[TIEMPORETARDO]=0;
			break;
		case '0':
			output ("Configuración actual:\n");
			output ("\tNIVEL DE ATENUACIÓN: ");
			outNum(10, estado[ATTREVERB],0);
			output ("\n\tRETARDO: ");
			outNum(10, estado[TIEMPORETARDO], 0);
			output("\n\n");
			break;
		case 'D':
			output ("¿Dónde desea guardarlo? Seleccione una de las 9 posiciones de memoria.\n\n");
			tecla = teclado();
			while ((tecla < '1') || (tecla > '9')){
				output ("Tecla no válida\n\n");
				tecla = teclado();
			}
			output ("Ha seleccionado: ");
			outch (tecla);
			output("\n\n");
			reverbGuardadas[tecla-'0'][0] = estado[ATTREVERB];
			reverbGuardadas[tecla-'0'][1] = estado[TIEMPORETARDO];
			output ("Configuración guardada.");
			break;
		case 'E':
			output ("¿Cuál desea cargar? Seleccione una de las 9 posiciones de memoria.\n\n");
			tecla = teclado();
			while ((tecla < '1') || (tecla > '9')){
				output ("Tecla no válida\n\n");
				tecla = teclado();
			}
			estado[ATTREVERB]= reverbGuardadas[tecla-'0'][0];
			estado[TIEMPORETARDO]= reverbGuardadas[tecla-'0'][1];
			output ("Reverberación activada.\n");
			output ("\tNIVEL DE ATENUACIÓN: ");
			outNum(10, estado[ATTREVERB],0);
			output ("\n\tRETARDO: ");
			outNum(10, estado[TIEMPORETARDO],0);
			output("\n\n");
			break;
		case 'B':
			if (estado[MUESTRAENTRADA]== 0){
				estado[MUESTRAENTRADA]= 1;
				output ("VUMETRO: muestra entrada\n\n");
			}else {
				estado[MUESTRAENTRADA]= 0;
				output ("VUMETRO: muestra salida\n\n");
			}						
	         	break;
		case 'A':
			estado[FUNCION] = 0;
			break;
		default:
			output ("Tecla no válida.\n\n");
			break;
	}
}




//-------------------------------------------------------
// void selectAtenuacion(void)
// Descripcion:
//	Permite ajustar el parámetro de atenuación de la
//	reverberación. Dicho parametro indica por cuanto
//	se divide la señal en cada rebote.
//-------------------------------------------------------

void selectAtenuacion(void){
	char tecla;

	output("\t1,8 - Seleccione el nivel de atenuación: de 1 a 8\n");
	output("\tA - Volver.\n\n");

	flagsVolver[1]= 0;
	tecla = teclado();
	switch(tecla){
        	case '1':
			estado[ATTREVERB]= 1;
			while (flagsVolver[1] == 0){
				selectRetardo();
			}
			break;
		case '2':
			estado[ATTREVERB]= 2;
			while (flagsVolver[1] == 0){
				selectRetardo();
			}
			break;
		case '3':
			estado[ATTREVERB]= 3;
			while (flagsVolver[1] == 0){
				selectRetardo();
			}
			break;
		case '4':
			estado[ATTREVERB]= 4;
			while (flagsVolver[1] == 0){
				selectRetardo();
			}
			break;
		case '5':
			estado[ATTREVERB]= 5;
			while (flagsVolver[1] == 0){
				selectRetardo();
			}
			break;
		case '6':
			estado[ATTREVERB]= 6;
			while (flagsVolver[1] == 0){
				selectRetardo();
			}
			break;
		case '7':
			estado[ATTREVERB]= 7;
			while (flagsVolver[1] == 0){
				selectRetardo();
			}
			break;
        	case '8':
			estado[ATTREVERB]= 8;
			while (flagsVolver[1] == 0){
				selectRetardo();
			}
			break;
		case 'A':
			flagsVolver[0] = 1;
			break;
		default: 
			output ("Tecla no válida.\n\n");
			break;
	}


}



//-------------------------------------------------------
// void selectRetardo(void)
// Descripcion:
//	Permite seleccionar el retardo del efecto de
//	reverberación. Dicho parámetro indica cuanto
//	tarda el eco en volver desde que se emite un
//	sonido.
//-------------------------------------------------------

void selectRetardo(void){
	char tecla;

	output ("Seleccione el retardo:\n");		
	output ("\t1 - 100 ms\n");
	output ("\t2 - 200 ms\n");
	output ("\t3 - 300 ms\n");		
	output ("\t4 - 400 ms\n");
	output ("\t5 - 500 ms\n");
	output ("\t6 - 600 ms\n");		
	output ("\t7 - 700 ms\n");
	output ("\t8 - 800 ms\n");
	output ("\t9 - 900 ms\n");
	output ("\t0 - 1 s\n");
	output ("\tA - Volver\n\n");

	tecla = teclado();
	switch(tecla){
		case '1':
			output ("Reverberación activada.\n");
			output ("\tNIVEL DE ATENUACIÓN: ");
			outNum(10, estado[ATTREVERB],0);
			output ("\n\tRETARDO: 100ms\n\n");
			estado[REVERBERACION]= 1;
			estado[TIEMPORETARDO]= 100;
			flagsVolver[0] = 1;
			flagsVolver[1] = 1;
			break;
		case '2':
			output ("Reverberación activada.\n");
			output ("\tNIVEL DE ATENUACIÓN: ");
			outNum(10, estado[ATTREVERB],0);
			output ("\n\tRETARDO: 200ms \n\n");
			estado[REVERBERACION]= 1;
			estado[TIEMPORETARDO]= 200;
			flagsVolver[0] = 1;
			flagsVolver[1] = 1;
			break;
		case '3':
			output ("Reverberación activada.\n");
			output ("\tNIVEL DE ATENUACIÓN: ");
			outNum(10, estado[ATTREVERB],0);
			output ("\n\tRETARDO: 300ms \n\n");
			estado[REVERBERACION]= 1;
			estado[TIEMPORETARDO]= 300;
			flagsVolver[0] = 1;
			flagsVolver[1] = 1;
			break;
		case '4':
			output ("Reverberación activada.\n");
			output ("\tNIVEL DE ATENUACIÓN: ");
			outNum(10, estado[ATTREVERB],0);
			output ("\n\tRETARDO: 400ms \n\n");
			estado[REVERBERACION]= 1;
			estado[TIEMPORETARDO]= 400;
			flagsVolver[0] = 1;
			flagsVolver[1] = 1;
			break;
		case '5':
			output ("Reverberación activada.\n");
			output ("\tNIVEL DE ATENUACIÓN: ");
			outNum(10, estado[ATTREVERB],0);
			output ("\n\tRETARDO: 500ms \n\n");
			estado[REVERBERACION]= 1;
			estado[TIEMPORETARDO]= 500;
			flagsVolver[0] = 1;
			flagsVolver[1] = 1;
			break;
		case '6':
			output ("Reverberación activada.\n");
			output ("\tNIVEL DE ATENUACIÓN: ");
			outNum(10, estado[ATTREVERB],0);
			output ("\n\tRETARDO: 600ms \n\n");
			estado[REVERBERACION]= 1;
			estado[TIEMPORETARDO]= 600;
			flagsVolver[0] = 1;
			flagsVolver[1] = 1;
			break;
		case '7':
			output ("Reverberación activada.\n");
			output ("\tNIVEL DE ATENUACIÓN: ");
			outNum(10, estado[ATTREVERB],0);
			output ("\n\tRETARDO: 700ms \n\n");
			estado[REVERBERACION]= 1;
			estado[TIEMPORETARDO]= 700;
			flagsVolver[0] = 1;
			flagsVolver[1] = 1;								  
           		break;
        	case '8':
			output ("Reverberación activada.\n");
			output ("\tNIVEL DE ATENUACIÓN: ");
			outNum(10, estado[ATTREVERB],0);
			output ("\n\tRETARDO: 800ms \n\n");
			estado[REVERBERACION]= 1;
			estado[TIEMPORETARDO]= 800;
			flagsVolver[0] = 1;
			flagsVolver[1] = 1;
			break;
        	case '9':
			output ("Reverberación activada.\n");
			output ("\tNIVEL DE ATENUACIÓN: ");
			outNum(10, estado[ATTREVERB],0);
			output ("\n\tRETARDO: 900ms \n\n");
			estado[REVERBERACION]= 1;
			estado[TIEMPORETARDO]= 900;
			flagsVolver[0] = 1;
			flagsVolver[1] = 1;
			break;
		case '0':
			output ("Reverberación activada.\n");
			output ("\tNIVEL DE ATENUACIÓN: ");
			outNum(10, estado[ATTREVERB], 0);
			output ("\n\tRETARDO: 1s \n\n");
			estado[REVERBERACION]= 1;
			estado[TIEMPORETARDO]= 1000;
			flagsVolver[0] = 1;
			flagsVolver[1] = 1;
			break;

		case 'A':
			flagsVolver[1]= 1;
			break;
		default:
			output ("Tecla no válida.\n\n");
			break;
	}

}