//---------------------------------------------------------------
// Filtros.c
//
// Autores: Rafael López Martínez
//          Alfredo Álvarez Senra
//---------------------------------------------------------------


//------------------------------------------
//DECLARACIÓN DE VARIABLES Y ARRAYS GENERALES
//------------------------------------------



		
int salidasFiltros[7]={0,0,0,0,0,0,0};				//Array para guardar las salidas de cada filtro y asi poder acceder a ellas cuando sea necesario. Una columna para cada filtro.



//----------------------
//DECLARACIÓN DE MÉTODOS
//----------------------

void calculaSalidasFiltros(int lectura);		


//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------




//-------------------------------------------------------
// void calculaSalidasFiltros(int lectura)
// Descripcion:
//	Calcula la salida de todos los filtros y las
//	las guarda en su posicion correspondiente del
//	array salidasFiltros[]
//-------------------------------------------------------

inline void calculaSalidasFiltros(int lectura){
	int i;
								// Array con los coeficientes de los filtros. 7 filas, una para cada banda. 6 columnas para cada coeficiente ordenados de la forma: G, b0, b1, b2, a1, a2.
static int coefs[7][6] = {{8,1024,0,-1024,-2029,1006},		//PRIMERA LINEA DEL ARRAY = banda 0 - frecuencia central 31,25Hz
                 	  {17,1024,0,-1024,-2011,988},		//PRIMERA LINEA DEL ARRAY = banda 1 - frecuencia central 62,50Hz
	                  {34,1024,0,-1024,-1970,955},		//PRIMERA LINEA DEL ARRAY = banda 2 - frecuencia central 125Hz
        	          {66,1024,0,-1024,-1878,890},		//PRIMERA LINEA DEL ARRAY = banda 3 - frecuencia central 250Hz
                	  {125,1024,0,-1024,-1660,772},		//PRIMERA LINEA DEL ARRAY = banda 4 - frecuencia central 500Hz
	                  {227,1024,0,-1024,-1115,569},		//PRIMERA LINEA DEL ARRAY = banda 5 - frecuencia central 1000Hz
        	          {392,1024,0,-1024,141,239}};		//PRIMERA LINEA DEL ARRAY = banda 6 - frecuencia central 2000Hz

	static int historia[7][2] =    {{0,0},					//Array para guardar las historias necesarias para carlcular filtrado
					{0,0},					//Una linea para cada filtro. Una columna para cada instante
		     			{0,0},
		     			{0,0},
		    			{0,0},
		   			{0,0},
		  		 	{0,0}};

	for(i=0; i<7; i++){						//Hace las operaciones necesarias para calcular las salidas de cada filtros y se almacenan en su posición correspondiente del array salidasFiltros
		int operando1=0;
		int operando2=0;
		int operando3=0;
		int operando4=0;
		int operando5=0;
		int suma1=0;
		int suma1temp=0;
		int suma2=0;
	
		operando1 = lectura*coefs[i][1];
		operando2 = historia[i][0]*(-coefs[i][4]);
		operando3 = historia[i][1]*(-coefs[i][5]);
		operando4 = historia[i][0]*coefs[i][2];
		operando5 = historia[i][1]*coefs[i][3];
		suma1 = operando1 + operando2 + operando3;
		suma1temp = suma1>>10;					//Utiliza esta variable temporal para guardar las historias escaladas.
		historia[i][1] = historia[i][0];
		historia[i][0] = suma1temp;
		suma2 = (suma1 + operando4 + operando5)>>10;		//Desplaza 10 posiciones para eliminar el escalado por 1024 de los coeficientes a y b.
		suma2 = suma2*coefs[i][0];
		suma2 = suma2>>10;					//Desplaza 10 posiciones para eliminar el escalado por 1024 de la ganancia
		salidasFiltros[i] = suma2;				//Almacena en la posición correspondiente del array salidasFiltros
	}
}