//---------------------------------------------------------------
// Reverberacion.c
//
// Autores: Rafael L�pez Mart�nez
//          Alfredo �lvarez Senra
//---------------------------------------------------------------


//----------------------
//DECLARACI�N DE M�TODOS
//----------------------

int salidaReverberacion(int muestra, int atenuacion, int retardo);



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------
// int salidaReverberacion(int muestra, int atenuacion, int retardo)
// Descripci�n:
//	calcula la salida correspondiente al efecto de 
//	reverberaci�n. 
//	Los parametros son:
//		muestra: la muestra a la que se le suma el eco
//		atenuacion: factor de atenuaci�n del eco
//		retardo: tiempo que tarda en llegar el eco
//	Devuelve la muestra con el eco sumado
//------------------------------------------------------
int salidaReverberacion(int muestra, int atenuacion, int retardo){
	static int muestrasAlmacenadas[8000]; 				// array que guarda 8000 muestras equivalentes a 1 segundo.
	static int posicionMuestraActual=0;				// variable que funciona como �ndice de la posici�n donde se debe guardar la muestra entrante dentro del array
	static int posicionMuestraRetardada=0;				// variable que funciona como �ndice de la posici�n donde se encuentra la muestra antigua que se sumar� como eco
	int salidaReverb=0;
	int retardoAtenuado=0;
	
	if (posicionMuestraActual == 8000){		// si llegamos al final del bucle circular donde se guardan las muestras volvemos al principio
		posicionMuestraActual = 0;
	}
	if(estado[REVERBERACION] == 1){			// si la reverberaci�n esta activada
		posicionMuestraRetardada = (posicionMuestraActual) - 8*retardo;		// Puesto que retardo es un m�ltiplo de 100ms y cada 100ms guardamos 8 muestras, la muestra antigua correspondiente al retardo seleccionado estar� 8*retardo posiciones m�s atras de la actual.
		if (posicionMuestraRetardada < 0){					// si posicionMuestraRetardada es un n�mero negativo, le suma el tama�o total del array
			posicionMuestraRetardada += 8000;
		}
		
		if(atenuacion== 1){
			retardoAtenuado = muestrasAlmacenadas[posicionMuestraRetardada];    		// selecciona la muestra antigua correspondiente y atenua el factor correspondiente
			salidaReverb = muestra + retardoAtenuado;
		}else if(atenuacion == 2){
			retardoAtenuado = (muestrasAlmacenadas[posicionMuestraRetardada]>>1);    	// si atenuacion = 2, divide por 2.
			salidaReverb = muestra + retardoAtenuado;
		}else if(atenuacion == 3){
			retardoAtenuado = (muestrasAlmacenadas[posicionMuestraRetardada]*85)>>8;    	// si atenuacion = 3, divide por 3. (utiliza un escalado de 256 para no trabajar con decimales. Despu�s desplaza 8 posiciones para eleminar dicho escalado)
			salidaReverb = muestra + retardoAtenuado;
		}else if(atenuacion == 4){
			retardoAtenuado = (muestrasAlmacenadas[posicionMuestraRetardada])>>2;    
			salidaReverb = muestra + retardoAtenuado;
		}else if(atenuacion == 5){
			retardoAtenuado = (muestrasAlmacenadas[posicionMuestraRetardada]*51)>>8;    
			salidaReverb = muestra + retardoAtenuado; 
		}else if(atenuacion == 6){
			retardoAtenuado = (muestrasAlmacenadas[posicionMuestraRetardada]*43)>>8;    
			salidaReverb = muestra + retardoAtenuado; 
		}else if(atenuacion == 7){
			retardoAtenuado = (muestrasAlmacenadas[posicionMuestraRetardada]*37)>>8;    
			salidaReverb = muestra + retardoAtenuado;
		}else if(atenuacion == 8){
			retardoAtenuado = (muestrasAlmacenadas[posicionMuestraRetardada])>>3;    
			salidaReverb = muestra + retardoAtenuado;
		}
		
	}else{						//En caso de que la reverberaci�n no este activada saca la muestra entrante.
		salidaReverb = muestra;
	}
	muestrasAlmacenadas[posicionMuestraActual]=salidaReverb;		// guarda la muestra con el eco a�adido para su posterior utilizaci�n.
	posicionMuestraActual++;						// incrementa el �ndice que indica la posici�n donde debe guardar la siguiente muestra
	return salidaReverb;
}