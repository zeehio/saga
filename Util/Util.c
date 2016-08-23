/* Saga - Un transcriptor fon�tico para el idioma espa�ol
 *
 * Copyright (C) 1993-2009  Albino Nogueiras Rodr�guez y Jos� B. Mari�o
 *       TALP - Universitat Polit�cnica de Catalunya, ESPA�A
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _POSIX_C_SOURCE 200809L

#include	<sys/types.h>
#include	<sys/stat.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<math.h>
#include	<limits.h>
#include	<time.h>
#include <unistd.h>
#include	"Util.h"

/***********************************************************************
 * CaseInsensitiveName - Construye el nombre reemplazando mayusculas 
 por mayusculas_ sin tener en cuenta el directorio ni la extensi�n.
 Tambi�n reemplaza '<' por '(' y '>' por ')'
 Ejemplos: 
	/vGhNj/adf/FGjkJ.hmm    -> /vGhNj/adf/F_G_jkJ_.hmm
	/vGhNj/adf/<pause>_49.hmm -> /vGhNj/adf/(pause)_49.hmm
 **********************************************************************/
int CaseInsensitiveName(const char* src, char* dst, size_t dpmax)
{
	int sp, ext = 0;
	size_t dp;

	/* check dpmax */
	if (dpmax == 0)
		return -1;

	/* reset dst */
	dst[0] = '\0';

	/* search directory */
	for (sp=strlen(src)-1; sp>=0; sp--) {
		char c = src[sp];
		if ((c == '\\') || (c == '/') || (c == ':'))
			break;
	}
	dp = ++sp;

	/* check limits */
	if (dp>=dpmax) 
		return -1;

	strncpy(dst, src, dp);
	for (; src[sp] && dp<dpmax; sp++, dp++) {
		char c = src[sp];
		if (c == '<') {
			dst[dp] = '(';
		}
		else if (c == '>') {
			dst[dp] = ')';
		}
		else {
			dst[dp] = c;
		}
		if (!ext && (c >= 'A') && (c <= 'Z') && (dp<dpmax-1)) {
			dst[++dp] = '_';
		}
/*
		if (c == '.')
			ext = 1;
*/
	}
	if (dp < dpmax) {
		dst[dp] = 0;
		return 0;
	}
	else {
		dst[0] = 0;
		return -1;
	}
}

int   IsAbsPath(char	*Path)
{

   if (Path == NULL || Path[0] == '\0')
      return -1;


#if defined(WIN32) || defined(_WINDOWS)
   return Path[0] == '/'  ||
          Path[0] == '\\' ||
          (Path[1] == ':' && Path[2] == '/') ||
          (Path[1] == ':' && Path[2] == '\\');

#else
   return Path[0] == '/';
#endif

}



/***********************************************************************
 * PathName - Construye el nombre completo a partir del path y el nombre
 **********************************************************************/

void	PathName(	const char	*Path,
					const char	*Name,
					const char	*Ext,
					char		*FullName)
{
	char	*ExtAnt;

	/*
	 * Si Path apunta a una posicion de memoria distinta de cero,
	 * construimos los nombres completos de los ficheros encadenando el
	 * path y el nombre.
	 */
	if (Path != NULL) {
		(void) sprintf(FullName, "%s/%s", Path, Name);
	}
	else {
		(void) sprintf(FullName, "%s", Name);
	}

	/*
	 * Si se ha indicado extension y el nombre no la incluia, anhadimos
	 * la indicada.
	 */
	if (Ext != NULL) {
		ExtAnt = strrchr(FullName, '.');
		if (ExtAnt == NULL || strcmp(ExtAnt + 1, Ext) != 0) {
			(void) strcat(FullName, ".");
			(void) strcat(FullName, Ext);
		}
	}
	
	return;
}

/***********************************************************************
 * ChkPathName - Comprueba la validez de un path y, si no existe,
 * lo crea.
 **********************************************************************/

int		ChkPathName(const char	*FullPath)
{
	char	Path[_POSIX_PATH_MAX+1], *Dir, *Name;

	/*
	 * Si FullPath == NULL, error.
	 */
	if (FullPath == NULL) {
		return -1;
	}

	(void) strcpy(Path, FullPath);

	/* 
	 * Si Path no contiene ninguna barra (/), Path=. y es correcto.
	 */
	if ((Name = strrchr(Path, '/')) == NULL) {
		return 0;
	}
	else {
		Name[1] = '\0';
	}

	Dir = Path;
	/*
	 * Eliminamos los posibles '/' que haya al principio.
	 */
	while (Dir[0] == '/') Dir++;
	while ((Dir = strchr(Dir, '/')) != NULL) {
		Dir[0] = '\0';
		if (access(Path, F_OK) != 0) {
			if (mkdir(Path, 0755) != 0) {
				return -3;
			}
			errno = 0;
		}
		Dir[0] = '/';
		Dir++;
	}
	
	return 0;
}

/***********************************************************************
 * QuitaExt - Elimina una extension si la encuentra en el nombre
 **********************************************************************/

int		QuitaExt(
	char	*FicSen,
	char	*ExtSen)
{
	int		LongFic, LongExt;
	
	if (ExtSen == NULL) return 0;
	if (FicSen == NULL) return -1;

	LongFic = strlen(FicSen);
	LongExt = strlen(ExtSen);

	if (LongExt >= LongFic) return -1;

	if (strcmp(FicSen + LongFic - LongExt, ExtSen) == 0) {
		if (ExtSen[0] != '.' && FicSen[LongFic - LongExt - 1] == '.') LongExt++;
		FicSen[LongFic - LongExt] = '\0';
		return 0;
	}

	return -1;
}

/***********************************************************************
 * QuitaDir - Elimina el directorio si lo encuentra en el nombre
 **********************************************************************/

char	*QuitaDir(
	char	*FicSen,
	char	*DirSen)
{
	int		LongFic, LongDir;
	
	if (DirSen == NULL) return FicSen;
	if (FicSen == NULL) return NULL;

	LongFic = strlen(FicSen);
	LongDir = strlen(DirSen);

	if (LongDir >= LongFic) return FicSen;

	if (strncmp(FicSen, DirSen, LongDir) == 0) {
		FicSen += LongDir;
		
		if (*FicSen == '/') FicSen++;
		return FicSen;
	}

	return FicSen;
}

/***********************************************************************
 * CreaLisSen - Crea una lista de senhales a partir de un fichero guia
 * o de un vector de nombres de fichero rematado en NULL.
 **********************************************************************/

char	**CreaLisSen(
 	char	*LisFic,
	char	**NomFic,
	char	*DirSen,
	char	*ExtSen,
	int		*NumFic)
{
	FILE	*FpGui;
	char	**FicGui;
	char	**LisSen, *NomSen;
	char	FicSen[_POSIX_PATH_MAX];
	int		Gui, NumGui;

	/*
	 * Debe especificarse el nombre del fichero guia o el vector de
	 * nombres
	 */
	if (LisFic == NULL && NomFic == (char **) 0) {
		if (!DirSen || (!strcmp(DirSen, "-") && !strcmp(DirSen, "stdin") && !strcmp(DirSen, "stdio"))) {
			fprintf(stderr, "Error al crear la lista de senhales:\n");
			fprintf(stderr, "Debe especificarse el fichero guia o el nombre de los ficheros\n");
			return (char **) 0;
		}
	}

	if (LisFic != NULL && NomFic != (char **) 0) {
		fprintf(stderr, "Error al crear la lista de senhales:\n");
		fprintf(stderr, "No se puede especificar un fichero guia y el nombre de los ficheros al mismo tiempo\n");
		return (char **) 0;
	}

	if ((LisSen = (char **) malloc(sizeof(char *))) == (char **) 0) {
		fprintf(stderr, "Error al crear la lista de senhales (%s)\n", strerror(errno));
		return (char **) 0;
	}

	/*
	 * Si se ha indicado fichero guia, lo leemos.
	 */
	if (LisFic != NULL) {
		if ((FicGui = MatStrChr(LisFic, "+ ,")) == (char **) 0) {
			fprintf(stderr, "ERROR al procesar la lista de ficheros guia %s\n", LisFic);
			return (char **) 0;
		}

		for (NumGui = 0; FicGui[NumGui] != NULL; NumGui++) ;

		/*
		 * Abrimos los fichero guia
		 */
		*NumFic = 0;
		for (Gui = 0; Gui < NumGui; Gui++) {
			if ((FpGui = fopen(FicGui[Gui], "rt")) == (FILE *) 0) {
				fprintf(stderr, "Error al leer la lista de ficheros %s (%s)\n", FicGui[Gui], strerror(errno));
				return (char **) 0;
			}

			/*
			 * Cargamos cada uno de los nombres en la matriz
			 */
			while (fscanf(FpGui, "%s", FicSen) != EOF) {
				(*NumFic)++;

				if ((LisSen = (char **) realloc((void *) LisSen, *NumFic * sizeof(char *))) == (char **) 0) {
					fprintf(stderr, "Error al crear la lista de senhales (%s)\n", strerror(errno));
					return (char **) 0;
				}

				(void) QuitaExt(FicSen, ExtSen);
			
				LisSen[*NumFic - 1] = strdup(FicSen);
			}
			fclose(FpGui);
		}
		LiberaLisSen(FicGui, NumGui);
		return LisSen;
	}
	else if (NomFic != (char **) 0) {
		char* tmp;
		for (*NumFic = 0; NomFic[*NumFic] != NULL; (*NumFic)++) {
			if ((LisSen = (char **) realloc((void *) LisSen, (*NumFic + 1) * sizeof(char *))) == (char **) 0) {
				fprintf(stderr, "Error al crear la lista de senhales (%s)\n", strerror(errno));
				return (char **) 0;
			}

			tmp = NomSen = strdup(NomFic[*NumFic]);

			(void) QuitaExt(NomSen, ExtSen);

			if (DirSen != NULL) {
				if ((NomSen = QuitaDir(NomSen, DirSen)) == NULL) {
					fprintf(stderr, "Error al eliminar %s de %s\n", DirSen, NomFic[*NumFic]);
					return (char **) 0;
				}
			}

			LisSen[*NumFic] = strdup(NomSen);
			free(tmp);
		}
		return LisSen;
	}
	else if (DirSen && (!strcmp(DirSen, "-") || !strcmp(DirSen, "stdin") || !strcmp(DirSen, "stdio"))) {
		return LeeLisSen(NumFic);
	}

	return (char **) 0;
}

/***********************************************************************
 * LiberaMatStr - Libera una lista de strings acabados en 0 
 **********************************************************************/

void	LiberaMatStr(
	char	**matStr)
{
	size_t i;
	if (matStr == NULL) {
		return;
	}
	for (i=0; matStr[i] != NULL; i++)
		free(matStr[i]);
	free(matStr);
	return;
}

/***********************************************************************
 * LiberaLisSen - Libera una lista de senhales 
 **********************************************************************/

void	LiberaLisSen(
	char	**NomFic,
	int		NumFic)
{
	int i;
	for (i=0; i<NumFic; i++)
		free(NomFic[i]);
	free(NomFic);
}




/***********************************************************************
 * LeeLisSen - Lee una lista de senhales de la entrada estandar
 **********************************************************************/

char	**LeeLisSen(int	*NumFic)
{
	char	**LisSen;
	char	FicSen[_POSIX_PATH_MAX];
	int		Fic;

	if (scanf("%d", NumFic) < 1) return (char **) 0;
	if ((LisSen = (char **) malloc(*NumFic * sizeof(char *))) == (char **) 0) {
		fprintf(stderr, "Error al crear la lista de senhales (%s)\n", strerror(errno));
		return (char **) 0;
	}
	for (Fic = 0; Fic < *NumFic; Fic++) {
		if (scanf("%s\n", FicSen) != 1) {
			fprintf(stderr, "Error al leer la lista de senhales (%s)\n", strerror(errno));
			return NULL;
                }
		LisSen[Fic] = strdup(FicSen);
	}

	return LisSen;
}

/***********************************************************************
 * EscrLisSen - Escribe en la selida estandar la lista de senhales
 **********************************************************************/

int		EscrLisSen(
	char	**LisSen,
	int		NumFic)
{
	int		Fic;

	printf("%d\n", NumFic);
	for (Fic = 0; Fic < NumFic; Fic++) {
		printf("%s\n", LisSen[Fic]);
	}

	return 0;
}

/***********************************************************************
 * TamMat - Calcula el n�mero de elementos de una matriz a partir de una
 * cadena separada en "tokens" por comas y/o espacios.
 **********************************************************************/

int		TamMat(char	*_Str)
{
	char	*Str1, *Str = strdup(_Str);
	size_t	i;

	if ((Str1 = Str) == NULL) {
		return -1;
	}

	for (i = 0; strtok(Str, " ,") != NULL; i++) {
		Str = NULL;	/* Para la siguiente llamada a strtok	*/
	}

	free((void *) Str1);

	return i;
}

/***********************************************************************
 * MatStr - Construye una matriz de cadenas a partir de una cadena 
 * separada en "tokens" por comas y/o espacios.
 **********************************************************************/

char	**MatStr(const char	*Str)
{
	return MatStrChr(Str, " ,");
}

/***********************************************************************
 * MatStr - Construye una matriz de cadenas a partir de una cadena 
 * separada en "tokens" por comas y/o espacios.
 **********************************************************************/

char	**MatStrChr(
	const char	*_Str,
	const char	*Delim)

{
	char	**Mat, **Mat2 = NULL, *Str = strdup(_Str);
	char *token;
	char	*fStr = Str;
	size_t	i;

	if (Str == NULL) {
		return NULL;
	}

	/* Allocate space for the final NULL */
	if ((Mat = malloc(sizeof(char *))) == NULL) {
		free(fStr);
		return NULL;
	}

	for (i = 0; (token = strtok(Str, Delim)) != NULL; i++) {
		token = strdup(token);
		if (token == NULL) {
			free(Mat);
			free(fStr);
			return NULL;
		}
		/* We have a new token. Allocate space for it */
		Mat2 = realloc(Mat, (i + 2) * sizeof(char *));
		if (Mat2 == NULL) {
			free(fStr);
			free(Mat);
			return NULL;
		} else { /* realloc successful */
			Mat = Mat2;
			Mat2 = NULL;
		}
		/* Store the token */
		Mat[i] = token;
		/* All the following calls to strtok continue on the same str */
		Str = NULL;	/* Para la siguiente llamada a strtok	*/
	}

	Mat[i] = NULL;
	free(fStr);

	return Mat;
}

/***********************************************************************
 * MatInt - Construye una matriz de enteros a partir de una cadena 
 * separada en "tokens" por comas y/o espacios.
 **********************************************************************/

int	*MatInt(char	*_Str)
{
	char	*Str = strdup(_Str);
	char	*fStr = Str;
	char	*Cadena;
	int		*Mat;
	size_t	i;
	
	if (Str == NULL) {
		return (int *) 0;
	}

	if((Mat = (int *) malloc(sizeof(int))) == (int *) 0) {
		free(fStr);
		return (int *) 0;
	}

	for (i = 0; (Cadena = strtok(Str, " ,")) != NULL; i++) {
		Str = NULL;	/* Para la siguiente llamada a strtok	*/
		Mat[i] = atoi(Cadena);
		if((Mat = (int *) realloc((void *) Mat, (i + 2) * sizeof(int))) == (int *) 0) {
			free(fStr);
			return (int *) 0;
		}
	}

	Mat[i] = -1;
	free(fStr);

	return Mat;
}

/***********************************************************************
 * MatLng - Construye una matriz de enteros long a partir de una cadena 
 * separada en "tokens" por comas y/o espacios.
 **********************************************************************/

long	*MatLng(char	*_Str)
{
	char	*tmp = strdup(_Str);
	char	*Str = tmp;
	char	*Cadena;
	long		*Mat;
	size_t	i;

	if (Str == NULL) {
		return (long *) 0;
	}

	if((Mat = (long *) malloc(sizeof(long))) == (long *) 0) {
		return (long *) 0;
	}

	for (i = 0; (Cadena = strtok(Str, " ,")) != NULL; i++) {
		Str = NULL;	/* Para la siguiente llamada a strtok	*/
		Mat[i] = atol(Cadena);
		if((Mat = (long *) realloc((void *) Mat, (i + 2) * sizeof(long))) == (long *) 0) {
			return (long *) 0;
		}
	}

	Mat[i] = -1;
	free(tmp);
	return Mat;
}

/***********************************************************************
 * MatDbl - Construye una matriz de reales doble precision a partir de
 * una cadena separada en "tokens" por comas y/o espacios.
 **********************************************************************/

double	*MatDbl(char	*Str)
{
	char	*Cadena;
	double	*Mat;
	size_t	i;

	if (Str == NULL) {
		return (double *) 0;
	}

	if((Mat = (double *) malloc(sizeof(double))) == (double *) 0) {
		return (double *) 0;
	}

	for (i = 0; (Cadena = strtok(Str, " ,")) != NULL; i++) {
		Str = NULL;	/* Para la siguiente llamada a strtok	*/
		Mat[i] = atof(Cadena);
		if((Mat = (double *) realloc((void *) Mat, (i + 2) * sizeof(double))) == (double *) 0) {
			return (double *) 0;
		}
	}

	Mat[i] = -1;

	return Mat;
}

/***********************************************************************
 * EsExcUdf - Comprueba si una unidad fonetica es excluida.
 **********************************************************************/

int		EsExcUdf(	char	*_Unidad,	/* Unidad a comprobar	*/
					char	**ExcUdf)	/* Unidades excluidas	*/
{
	int		Exc;
	char	*Marca;
	char	Unidad[1024];

	strcpy(Unidad, _Unidad);

	while((Marca = strpbrk(Unidad, "_�:@'")) != NULL) {
		memmove(Marca, Marca + 1, strlen(Marca));
	}

	if (ExcUdf == (char **) 0) return -1;

	for (Exc = 0; ExcUdf[Exc] != NULL; Exc++) {
		if (strcmp(Unidad, ExcUdf[Exc]) == 0) return Exc;
	}

	return -1;
}

/***********************************************************************
 * EsIdenUdf - Comprueba si dos unidades foneticas estan identificadas.
 **********************************************************************/

int		EsIdenUdf(	char	*_Uni1,		/* Unidad a comprobar	*/
					char	*_Uni2,		/* Unidad a comprobar	*/
					char	***IdenUdf)	/* Unidades identif.	*/
{
	int		Iden, Udf;
	int		EsIden1, EsIden2;
	char	*Marca;
	char	Uni1[1024], Uni2[1024];

	strcpy(Uni1, _Uni1);
	strcpy(Uni2, _Uni2);

	while((Marca = strpbrk(Uni1, "_�:@'")) != NULL) {
		memmove(Marca, Marca + 1, strlen(Marca));
	}
	while((Marca = strpbrk(Uni2, "_�:@'")) != NULL) {
		memmove(Marca, Marca + 1, strlen(Marca));
	}

	if (!strcmp(Uni1, Uni2)) return 1;

	if (IdenUdf == (char ***) 0) return -1;

	for (Iden = 0; IdenUdf[Iden] != (char **) 0; Iden++) {
		EsIden1 = EsIden2 = 0;
		for (Udf = 0; IdenUdf[Iden][Udf] != NULL; Udf++) {
			if (strcmp(Uni1, IdenUdf[Iden][Udf]) == 0) break;
		}
		if (IdenUdf[Iden][Udf] != NULL) EsIden1 = 1;

		for (Udf = 0; IdenUdf[Iden][Udf] != NULL; Udf++) {
			if (strcmp(Uni2, IdenUdf[Iden][Udf]) == 0) break;
		}
		if (IdenUdf[Iden][Udf] != NULL) EsIden2 = 1;

		if (EsIden1 && EsIden2) return Iden;
	}

	return -1;
}

/***********************************************************************
 * NovaIden - Construye la matriz de identificacion fonetica
 **********************************************************************/

int		NovaIden(	char	****IdenUdf,
					int		*NumIden,
					char	*Iden)
{
	if (*IdenUdf == (char ***) 0) {
		*NumIden = 1;
		if ((*IdenUdf = (char ***) malloc(2*sizeof(char **))) == (char ***) 0) {
			(void) fprintf(stderr, "Error al ubicar la matriz de identificaciones\n");
			return -1;
		}
	}
	else {
		*NumIden += 1;
		if ((*IdenUdf = (char ***) realloc((void *) *IdenUdf, (*NumIden+1)*sizeof(char **))) == (char ***) 0) {
			(void) fprintf(stderr, "Error al reubicar la matriz de identificaciones\n");
			return -1;
		}
	}

	(*IdenUdf)[*NumIden-1] = MatStr(Iden);
	(*IdenUdf)[*NumIden] = (char **) 0;

	if ((*IdenUdf)[*NumIden-1][0] == NULL ||
		(*IdenUdf)[*NumIden-1][1] == NULL) {
		(void) fprintf(stderr, "Cada identificacion fonetica debe indicarse como:\n");
		(void) fprintf(stderr, "\t-a Udf1,Udf2[,...]\n");
		return -1;
	}

	return 0;
}
