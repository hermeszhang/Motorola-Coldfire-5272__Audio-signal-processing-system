//---------------------------------------------------------------
// LT-21.c
//
// Autores: Rafael López Martínez
//          Alfredo Álvarez Senra
//---------------------------------------------------------------


//---------------------------
//INCLUSION DE OTROS FICHEROS
//---------------------------

#include "m5272adc_dac.c"
#include "m5272lib.c"
#include "m5272lib.h"

#include "m5272gpio.c"
#include "m5272.h"


#include "Vumetro.c"
#include "Menu.c"
#include "Filtros.c"
#include "Reverberacion.c"




//---------------------------------------
//DEFINICIÓN DE CONSTANTES Y DIRECCIONES
//---------------------------------------

#define V_BASE 0x40				 				// Dirección de inicio de los vectores de interrupción

#define DIR_VTMR0 4*(V_BASE+5)			  				// Dirección del vector de TMR0

#define FREC_INT 8000				  				// Frecuencia de interrupción TMR0 = 8000 Hz (cada 125 us)
#define PRESCALADO 2

#define CNT_INT1 MCF_CLK/(FREC_INT*PRESCALADO*16)		        	// Valor de precarga del temporizador de interrupciones TRR0
#if CNT_INT1>0xFFFF

#error PRESCALADO demasiado pequeño para esa frecuencia (CNT_INT1>0xFFFF)

#endif


#define BORRA_REF 0x0002			 				// Valor de borrado de interrupciones pendientes de tout1 para TER0

volatile ULONG cont_retardo;

#define FONDO_ESCALA 0xFFF							// Valor de lectura máxima del ADC
#define V_MAX 5									// Valores de tensión máxima del ADC





//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------
// void bucleMain(void)
// Descripción:
//	Función del programa principal.
//	Llama a la función GestionMenuPrincipal
//	que nos mostrará el menú.
//------------------------------------------------------
void bucleMain(void){
	GestionMenuPrincipal ();				//muestra el menú en pantalla
}




//------------------------------------------------------
// void __init(void)
// Descripción:
//	Función por defecto de inicialización del sistema
//------------------------------------------------------

void __init(void){
	DAC_ADC_init();						// Inicializa el DAC y el ADC.

	mbar_writeByte(MCFSIM_PIVR,V_BASE);			// Fija comienzo de vectores de interrupción en V_BASE.

	ACCESO_A_MEMORIA_LONG(DIR_VTMR0)= (ULONG)_prep_TOUT0;	// Escribimos la dirección de la función para TMR0
	mbar_writeShort(MCFSIM_TMR0, (PRESCALADO-1)<<8|0x3D);	// TMR0: PS=2-1=1 CE=00 OM=1 ORI=1 FRR=1 CLK=10 RST=1
	mbar_writeShort(MCFSIM_TCN0, 0x0000);		        // Ponemos a 0 el contador del TIMER0
	mbar_writeShort(MCFSIM_TRR0, CNT_INT1);	                // Fijamos la cuenta final del contador
	mbar_writeLong(MCFSIM_ICR1, 0x8888C888);		// Marca la interrupción del TIMER0 como no pendiente

						                // y de nivel 4.
	cont_retardo = 0;
	sti();							// Habilitamos interrupciones

}




//------------------------------------------------------
// Definición de rutinas de atención a las interrupciones
//------------------------------------------------------ 

void rutina_int2(void){}
void rutina_int3(void){}
void rutina_int1(void){}
void rutina_int4(void){}
void rutina_tout0(void){
	
	static int filtroIluminado =0; 			// variable que guarda cual de las 7 bandas debe pintar.
	static int contadorVumetro=0;				// variable que sirve como contador de las 24 interrupciones de 8kHz durante las que se mantiene una linea de leds pintada.
	static int salidaVumetro=0;								// variable para guardar el sumatorio de la energia de la banda en las 24 interrupciones
	int lectura = 0;				//variable para guardar la lectura del ADC
	int salida = 0;					//variable para guardar la salida que entregaremos al DAC
	
	
	mbar_writeShort(MCFSIM_TER0,BORRA_REF); 	// Reset del bit de fin de cuenta.
	
	
	lectura = ADC_dato();				// lee del ADC
	if (lectura >= 0x800){				// Hace la extensión de signo
		lectura |= 0xFFFFF000;
	}
	
	
	
	
	
	
	//------------------------------------------------
	//CONFIGURACIÓN DE LA SALIDA QUE SE ENTREGA AL DAC
	//------------------------------------------------
	salida = 0;
	
	//Salida caracterización
	//-----------------------
	if(estado[FUNCION] == CARACTERIZACION){				// si está dentro del menú de caracterización calcula la salida correspondiente.
		calculaSalidasFiltros(lectura);				// filtra la señal entregada por el ADC
		if (estado[CARACTERIZACION] == TODOS_FILTROS){
			int i;
			for(i=0; i<7; i++){
				salida += salidasFiltros[i];		// si está caracterizando todos los filtros a la vez, salida es la suma las 7 componentes, una de cada filtro
			}
			salida = (salida*1500)>>11;			//divide entre raiz(2) para compensar la ganancia de 3 dB. En realidad multiplica por raiz(2)/2 escalado 1024 para no trabajar con decimales.
			salidaVumetro += salidasFiltros[filtroIluminado] * salidasFiltros[filtroIluminado];	//guarda la energia de la componente correspondiente al filtro que estamos pintando en el vúmetro
		} else{
			salida = salidasFiltros[estado[CARACTERIZACION]-1];	// si está caracterizando un solo filtro, salida es solo la componente correspondiente al filtro seleccionado.
			if (estado[MUESTRAENTRADA]== 0){
				salidaVumetro += salida * salida;
			}else {
				salidaVumetro += salidasFiltros[filtroIluminado] * salidasFiltros[filtroIluminado];
			}
		}
	

	//Salida ecualización
	//--------------------
	} else if(estado[FUNCION] == ECUALIZACION){			// si está dentro del menú de ecualización calcula la salida correspondiente.
		int i;
		
		calculaSalidasFiltros(lectura);				// filtra la señal entregada por el ADC
		for(i =0; i<7; i++){
			salida += (salidasFiltros[i]*ganancia[nivel[i]])>>10;	// salida es la suma las 7 componentes, una de cada filtro, cada una atenuada según el nivel de atenuación seleccionado.
		}
				
		salida = (salida*1500)>>11;				//divide entre raiz(2) para compensar la ganancia de 3 dB. En realidad multiplica por raiz(2)/2 escalado 1024 para no trabajar con decimales.
		if (estado[MUESTRAENTRADA]== 0){
			salidaVumetro += ((salidasFiltros[filtroIluminado]*ganancia[nivel[filtroIluminado]])>>10) * ((salidasFiltros[filtroIluminado]*ganancia[nivel[filtroIluminado]])>>10);	//guarda la energia de la componente ecualizada correspondiente al filtro que estamos pintando en el vúmetro
		}else {
			salidaVumetro += salidasFiltros[filtroIluminado] * salidasFiltros[filtroIluminado];
		}
	
	//Salida reverberación
	//--------------------	
	} else if(estado[FUNCION] == REVERBERACION){			// si está dentro del menú de reverberación calcula la salida correspondiente.
		salida = salidaReverberacion(lectura, estado[ATTREVERB], estado[TIEMPORETARDO]);		// llama a la función definida en Reverberacion.c que calcula la salida
		if (estado[MUESTRAENTRADA]== 0){
			calculaSalidasFiltros(salida);					// filtra la salida con el eco para poder seleccionar la energía de la banda que está pintando en el vúmetro
			salidaVumetro += salidasFiltros[filtroIluminado] * salidasFiltros[filtroIluminado];
		}else {
			calculaSalidasFiltros(lectura);
			salidaVumetro += salidasFiltros[filtroIluminado] * salidasFiltros[filtroIluminado];
		}
	
	//Salida por defecto
	//------------------
	} else{								// si está en el menú principal, entrega al DAC la lectura del ADC
		calculaSalidasFiltros(lectura);				// filtra para poder seleccionar la energía de la banda que está pintando en el vúmetro
		salida = lectura;
		salidaVumetro += salidasFiltros[filtroIluminado] * salidasFiltros[filtroIluminado];
	}
	
		
	
	//--------
	//VÚMETRO
	//--------
	
	if (contadorVumetro<24){			//si el contador de 24 interrupciones es menor de 24, se incrementa.
		contadorVumetro++;
	}else{						// cuando el contador valga 24, pinta la siguiente banda del vúmetro
		if (estado[MUESTRAECUALIZADOR]== 1){
			gestionEcualizGrafico(nivel[filtroIluminado] , signalTeclado, filtroIluminado);		// si está seleccionada la opción de mostrar el ecualizador gráfico en el vúmetro, llama al método que lo muestra.
		}
		else if (estado[CARACTERIZACION] != TODOS_FILTROS){						// si está caracterizando una sola banda de frecuancia, pinta constantemente dicha banda.
			gestionVumetro(salidaVumetro , signalTeclado, estado[CARACTERIZACION]-1);
		}
		else{
			gestionVumetro(salidaVumetro , signalTeclado, filtroIluminado);				// en todas las demás funciones pinta la energía de cada banda. Cada 24 interrupciones pinta una banda.
		}
		contadorVumetro = 0;			//contador de 24 interrupciones
		filtroIluminado++;			//después de pintar la banda correspondiente, incrementa el índice que indica que banda que debe pintarse.
		salidaVumetro =0;
	}
	if (filtroIluminado == 7){			// si ya ha pintado las 7 bandas, vuelve a la primera banda.
		filtroIluminado = 0;
	}
	
	
		
	salida += 0x0800;
	DAC_dato(salida);

}

void rutina_tout1(void){}

void rutina_tout2(void){}

void rutina_tout3(void){}
