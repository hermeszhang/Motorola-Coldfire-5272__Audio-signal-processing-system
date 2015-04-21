//---------------------------------------------------------------
// Vumetro.c
//
// Autores: Rafael López Martínez
//          Alfredo Álvarez Senra
//---------------------------------------------------------------


//------------------------------------------
//DECLARACIÓN DE VARIABLES Y ARRAYS GENERALES
//------------------------------------------

int signalSinTeclado=0;				// variable que servirá de máscara en la funcion teclado() para salvar los bits de la salida correspondientes a la matriz de leds


//----------------------
//DECLARACIÓN DE MÉTODOS
//----------------------

void configuraPuerto(WORD signalTecladoVumetro, int filtro, int nivelEnergia);
void gestionVumetro(int muestra, WORD teclado, int filtro);
void gestionEcualizGrafico(int nivel, WORD teclado, int filtro);

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------
// void gestionVumetro(int muestra, int teclado, int filtro)
// Descripcion:
//	Compara el nivel energía de la muestra con unos niveles
//	umbral (multiplicados por 24) para saber los leds debe iluminar 
//	en el vúmetro. Configura el puerto de salida al vúmetro 
//	para encender los leds necesarios llamando al método
//	configuraPuerto 
//	
//-------------------------------------------------------

void gestionVumetro(int muestra, WORD teclado, int filtro){
	int nivelEnergia=0;				// variable que guarda cuantos de los 8 niveles de energía debe pintar.
	
	if (muestra>= 6464496){
		nivelEnergia = 8;
	}else if (muestra>= 2309352){
		nivelEnergia = 7;
	}else if (muestra>= 824976){
		nivelEnergia = 6;
	}else if (muestra>= 294720){
		nivelEnergia = 5;
	}else if (muestra>= 105264){
		nivelEnergia = 4;
	}else if (muestra>= 37608){
		nivelEnergia = 3;
	}else if (muestra>= 13416){
		nivelEnergia = 2;
	}else if (muestra>= 4800){
		nivelEnergia = 1;
	}else if (muestra< 4800){
		nivelEnergia =0;			// en el caso de tener la mínima energía no se encenderá ningún led
	}
	
	configuraPuerto(teclado, filtro, nivelEnergia);

}


//-------------------------------------------------------
// void configuraPuerto(int signalTecladoVumetro, int filtro, int nivelEnergia)
// Descripcion:
//	Configura el puerto de salida al vúmetro mediante el
//	empleo de máscaras para la iluminación de los leds,
//	según el nivel de energía que tenga la banda.
//	
//-------------------------------------------------------

void configuraPuerto(WORD signalTecladoVumetro, int filtro, int nivelEnergia){
	int signalPuerto =0;
	int mascaraColumnas = 0;
	int mascaraFilas = filtro;			//MASK filtro1 (B4,B5,B6)= 000
							//MASK filtro2 (B4,B5,B6)= 001
							//MASK filtro3 (B4,B5,B6)= 010
							//MASK filtro4 (B4,B5,B6)= 011
							//MASK filtro5 (B4,B5,B6)= 100
							//MASK filtro6 (B4,B5,B6)= 101
							//MASK filtro7 (B4,B5,B6)= 110
							//MASK filtro8 (B4,B5,B6)= 111
							
	if (nivelEnergia == 0){		
		mascaraColumnas = 0;		//MASK nivelEnergia0 (B8,B9,B10,B11,B13,B14,B15)= 0000 0000 
	}else if (nivelEnergia == 1){		
		mascaraColumnas = 1;		//MASK nivelEnergia1 (B8,B9,B10,B11,B13,B14,B15)= 0000 0001
	}else if (nivelEnergia == 2){		
		mascaraColumnas = 3;		//MASK nivelEnergia2 (B8,B9,B10,B11,B13,B14,B15)= 0000 0011
	}else if (nivelEnergia == 3){		
		mascaraColumnas = 7;		//MASK nivelEnergia3 (B8,B9,B10,B11,B13,B14,B15)= 0000 0111
	}else if (nivelEnergia == 4){		
		mascaraColumnas = 15;		//MASK nivelEnergia4 (B8,B9,B10,B11,B13,B14,B15)= 0000 1111
	}else if (nivelEnergia == 5){		
		mascaraColumnas = 31;		//MASK nivelEnergia5 (B8,B9,B10,B11,B13,B14,B15)= 0001 1111
	}else if (nivelEnergia == 6){		
		mascaraColumnas = 63;		//MASK nivelEnergia6 (B8,B9,B10,B11,B13,B14,B15)= 0011 1111
	}else if (nivelEnergia == 7){		
		mascaraColumnas = 127;		//MASK nivelEnergia7 (B8,B9,B10,B11,B13,B14,B15)= 0111 1111
	}else if (nivelEnergia == 8){		
		mascaraColumnas = 255;		//MASK nivelEnergia8 (B8,B9,B10,B11,B13,B14,B15)= 1111 1111
	}
	
	mascaraFilas = mascaraFilas << 4;			//Desplaza 4 bits porque los correspondientes a las filas son del 5 al 7
	mascaraColumnas = mascaraColumnas <<8;			//Desplaza 8 bits porque los correspondientes a las columnas son del 9 al 16 
	signalSinTeclado = 0|mascaraFilas;
	signalSinTeclado = signalSinTeclado |mascaraColumnas;		// Mediante estas dos operaciones 'OR' consegue en un mismo parámetro las dos máscaras sin que se machaquen la una a la otra. Esta máscara se utiliza en el metodo teclado()
	signalPuerto = signalSinTeclado |signalTecladoVumetro;		// Añade la máscara del teclado, correspondiente a los 4 primeros bits de la salida, para no interferir en el funcionamiento de este.
	set16_puertoS(signalPuerto);					// Asignamos el valor de la máscara al puerto para que el en la matriz de leds se enciendan los leds que corresponden.
}



//-------------------------------------------------------
// void gestionEcualizGrafico(nivel[filtroIluminado] , signalTeclado, filtroIluminado)
// Descripcion:
//	Muestra el ecualizador gráfico en el matriz de leds
//	Dedica un led para cada dos niveles de ecualización.
//	Configura el puerto de salida al vúmetro para encender los leds 
//	necesarios llamando al método configuraPuerto 
//	
//-------------------------------------------------------

void gestionEcualizGrafico(int nivel, WORD teclado, int filtro){
	int nivelEnergia =0;
	
	if (nivel<= 1){
		nivelEnergia = 7;
	}else if (nivel<= 3){
		nivelEnergia = 6;
	}else if (nivel<= 5){
		nivelEnergia = 5;
	}else if (nivel<= 7){
		nivelEnergia = 4;
	}else if (nivel<= 9){
		nivelEnergia = 3;
	}else if (nivel<= 11){
		nivelEnergia = 2;
	}else if (nivel<= 13){
		nivelEnergia = 1;
	}else if (nivel<= 15){
		nivelEnergia = 0;
	}
	configuraPuerto(teclado, filtro, nivelEnergia);
}
