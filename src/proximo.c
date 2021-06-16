extern unsigned char proximo(unsigned char** a, unsigned int i, unsigned int j, unsigned char regla, unsigned int n){
	char previo, central, siguiente;
	int posicion;

	if (j == 0) {
		previo = a[i][n-1];
		siguiente = a[i][j+1];
	} else if (j == n-1) {
		previo = a[i][j-1];
		siguiente = a[i][0];
	} else {
		previo = a[i][j-1];
		siguiente = a[i][j+1];
	}
	central = a[i][j];

	posicion = previo << 2 | central << 1 | siguiente << 0;

	return (1 & (regla >> posicion));
}