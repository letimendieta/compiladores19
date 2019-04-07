/*********** Librerias utilizadas **************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

/************* Definiciones ********************/

//Codigos
#define L_CORCHETE 	      256
#define R_CORCHETE	      257
#define L_LLAVE 	      258
#define R_LLAVE 	      259
#define COMA		      260
#define DOS_PUNTOS	      261
#define STRING                262
#define NUMBER  	      263
#define PR_TRUE		      264
#define PR_FALSE	      265
#define PR_NULL               266
#define TAMAUX                5
#define TAMLEX                50
#define TAMESP               101
#define VACIO                 1


/************* Estructuras ********************/

typedef struct {
	int compLex;
        char lexema[TAMLEX];
} token;

/************* Variables globales **************/

token t;	// token global para recibir componentes del Analizador Lexico			   


/***** Variables para el analizador lexico *****/

FILE *entrada;	// Fuente json		   
FILE *salida;	// Resultado analisis lexico			
char id[TAMLEX];		    
int nroLinea=1;	// Numero de Linea		    

/**************** Funciones **********************/


void error(const char* mensaje)
{
	printf("\nLinea %d: Error Lexico. %s",nroLinea,mensaje);	
}

void lexema()
{
	int indice=0;
    	char car=0;
    	int flag=0;
	int acepto=0;
	int estado=0;
	char msg[50];
	char aux[TAMAUX] = " "; 
     
   	while((car=fgetc(entrada))!=EOF)
	{
        if(car=='\n')
	    {
		   
		    	nroLinea++;
          		fputs("\n",salida);
		    continue;
	    }
	    else if (car==' ')
	    {
            do
            {
                
		fputs(" ",salida);
                car=fgetc(entrada); 
            }while(car ==' ');
            car=ungetc(car,entrada);
	    }
        else if (car=='\t')
        { 
            
            while(car=='\t')
            {
			fputs(" ",salida);
	        	car=fgetc(entrada);
            }
        }
    	else if (isdigit(car))
	    {
            
            indice=0;
            estado=0;
            acepto=0;
            id[indice]=car;
			while(!acepto)
			{
				switch(estado)
				{
				    case 0: //una secuencia netamente de digitos, puede ocurrir . o e
						car=fgetc(entrada);
						if (isdigit(car))
						{
							id[++indice]=car;
							estado=0;
						}
						else if(car=='.')
						{
							id[++indice]=car;
							estado=1;
						}
						else if(tolower(car)=='e')
						{
							id[++indice]=car;
							estado=3;
						}
						else
							estado=6;
						break;		
					case 1://un punto, debe seguir un digito 
						car=fgetc(entrada);						
						if (isdigit(car))
						{
							id[++indice]=car;
							estado=2;
						}
						else{
							sprintf(msg,"No se esperaba '%c' despues del . ",car);
							fputs("Error lexico",salida);
							estado=-1;
						}
						break;
					case 2://la fraccion decimal, pueden seguir los digitos o e
						car=fgetc(entrada);
						if (isdigit(car))
						{
							id[++indice]=car;
							estado=2;
						}
						else if(tolower(car)=='e')
						{
							id[++indice]=car;
							estado=3;
						}
						else
							estado=6;
						break;
					case 3://una e, puede seguir +, - o una secuencia de digitos
						car=fgetc(entrada);
						if (car=='+' || car=='-')
						{
							id[++indice]=car;
							estado=4;
						}
						else if(isdigit(car))
						{
							id[++indice]=car;
							estado=5;
						}
						else
						{
							sprintf(msg,"Se esperaba signo o digitos despues del exponente");
							fputs("Error lexico",salida);
							estado=-1;
						}
						break;
					case 4://necesariamente debe venir por lo menos un digito
						car=fgetc(entrada);
						if (isdigit(car))
						{
							id[++indice]=car;
							estado=5;
						}
						else
						{
							sprintf(msg,"No se esperaba '%c' despues del signo",car);
							fputs("Error lexico",salida);
							estado=-1;
						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						car=fgetc(entrada);
						if (isdigit(car))
						{
							id[++indice]=car;
							estado=5;
						}
						else
							estado=6;
						break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (car!=EOF)
							ungetc(car,entrada);
						else
                            car=0;
						id[++indice]='\0';
						acepto=1;
                        t.compLex=NUMBER;
						strcpy(t.lexema,id);
						fputs("numero",salida);
						
						break;
					case -1:
						if (car==EOF){
                            error("No se esperaba el fin de entrada\n");
							fputs("Error lexico",salida);
						}
						else
                            error(msg);
                        acepto=1;
                    t.compLex=VACIO;
                    while(car!='\n')
                        car=fgetc(entrada);
                    ungetc(car,entrada);
					break;
				}
			}
			break;
		}
        else if (car=='\"')
		{
            //un caracter o una cadena de caracteres
			indice=0;
			id[indice]=car;
			indice++;
			do
			{
				car=fgetc(entrada);
				if (car=='\"')
				{
                    id[indice]=car;
                    indice++;
                    flag=1;
                    break;
				}
                else if(car==EOF || car==',' || car=='\n' || car==':')
				{
                    sprintf(msg,"Se esperaba que finalice el literal");
					error(msg);
					fputs("Error lexico",salida);
                    
                    while(car!='\n')
                        car=fgetc(entrada);

                    ungetc(car,entrada);
                    break;                       
				}
				else
				{
					id[indice]=car;
					indice++;
				}
			}while(isascii(car) || flag==0);
			    id[indice]='\0';
            strcpy(t.lexema,id);
			t.compLex = STRING;
			fputs("string",salida);
			
			break;
		}
		else if (car==':')
		{
            //puede ser un :
            t.compLex=DOS_PUNTOS;
            strcpy(t.lexema,":");
			fputs("dos_puntos",salida);
			
            break;
		}
		else if (car==',')
		{
			t.compLex=COMA;
			strcpy(t.lexema,",");
			fputs("coma",salida);
			
			break;
		}
		else if (car=='[')
		{
			t.compLex=L_CORCHETE;
			strcpy(t.lexema,"[");
			fputs("L_corchete",salida);
			
			break;
		}
		else if (car==']')
		{
			t.compLex=R_CORCHETE;
			strcpy(t.lexema,"]");
			fputs("R_corchete",salida);
			
			break;
		}
		else if (car=='{')
		{
			t.compLex=L_LLAVE;
			strcpy(t.lexema,"{");
			fputs("L_llave",salida);
			
			break;		
        }
        else if (car=='}')
		{
			t.compLex=R_LLAVE;
			strcpy(t.lexema,"}");
			fputs("R_llave",salida);
						
			break;		
        }
		else if (car=='n' || car=='N')
        {
            ungetc(car,entrada);
            fgets(aux,5,entrada);//ver si es null
            if (strcmp(aux, "null")==0 || strcmp(aux, "NULL")==0)
            {
                t.compLex = PR_NULL;
                strcpy(t.lexema,aux);
				fputs("PR_null",salida);
				
            }
            else
            {
                sprintf(msg,"%c no esperado",car);
			    error(msg);
				fputs("Error lexico",salida);

                while(car!='\n')
                    car=fgetc(entrada);

                t.compLex = VACIO;
                ungetc(car,entrada);
            }
            break;
        }   
        else if (car=='f' || car=='F')
        {
            ungetc(car,entrada);
            fgets(aux,6,entrada);//ver si es null
            if (strcmp(aux, "false")==0 || strcmp(aux, "FALSE")==0)
            {
                t.compLex = PR_FALSE;
                strcpy(t.lexema,aux);
				fputs("PR_false",salida);
				
            }
            else{
                sprintf(msg,"%c no esperado",car);
			    error(msg);
				fputs("Error lexico",salida);

                while(car!='\n')
                    car=fgetc(entrada);    

                t.compLex = VACIO;
                ungetc(car,entrada);
            }
            break;
        }   
        else if (car=='t' || car=='T')
        {
            ungetc(car,entrada);
            fgets(aux,5,entrada);//ver si es null
            if (strcmp(aux, "true")==0 || strcmp(aux, "TRUE")==0)
            {
                t.compLex = PR_TRUE;
                strcpy(t.lexema,aux);
				fputs("PR_true",salida);
				
            }
            else
            {
                sprintf(msg,"%c no esperado",car);
			    error(msg);
				fputs("Error lexico",salida);

                while(car!='\n')
                    car=fgetc(entrada);

                t.compLex = VACIO;
                ungetc(car,entrada);
            }
            break;
        }
        else if (car!=EOF)
		{
			sprintf(msg,"%c no esperado",car);
			error(msg);
			fputs("Error lexico",salida);
            while(car!='\n')
                car=fgetc(entrada);
            ungetc(car,entrada);
		}
	}
	if (car==EOF)
	{
		t.compLex=EOF;
		strcpy(t.lexema,"EOF");
		sprintf(t.lexema,"EOF");
		fputs("EOF",salida);
		

	}
}

int main(int argc,char* args[])
{
	
	if(argc > 1)
	{
		if (!(entrada=fopen(args[1],"rt")))
		{
			printf("Fuente no encontrada.\n");
			exit(1);
		}
		salida = fopen("output.txt","w");
		while (t.compLex!=EOF)
		{
			lexema();
		}
		fclose(entrada);
		fclose(salida);
	}
	else
	{
		printf("Debe pasar como parametro el path al entrada fuente.\n");
		exit(1);
	}

	return 0;
}
