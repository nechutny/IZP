/*
 *	Autor:			Stanislav Nechutný (xnechu01)
 *	Název:			1. projekt IZP na FIT VUT
 *	Revize:			13
 *	Repozitář:		git@nechutny.net:vut.git
 *	Vytvořeno:		2013-10-05 11:17
 *	Poslední změna:	2013-10-30 11:21
*/


#include <stdio.h>
#include <string.h>

// Maximální délka slova v debug modu
#define DEBUG_MAXLEN		80

// Zástupné znaky pro filtr
#define FILTER_ALL			"."
#define FILTER_NUMBER		":"
#define FILTER_CAPITAL		"^"

// Nadefinujeme si kody znaků
#define LETTER_SMALL_A		'a'
#define LETTER_SMALL_Z		'z'
#define LETTER_CAPITAL_A	'A'
#define LETTER_CAPITAL_Z	'Z'
#define LETTER_NUMBER_0		'0'
#define LETTER_NUMBER_9		'9'
#define	LETTER_MINUS		'-'
#define LETTER_UNDERSCORE	'_'

// Navratovy kod pri spatnem pouziti - z unixoveho sysexits.h
#define RETURN_USAGE_ERROR	64

// Vycteme dostupne funkce na zacatku, aby jsme je mohli uzivat i pred jejich deklaraci
int isSetParams(int argc, const char **argv);
int isHelp(int argc, const char **argv);
int isDebug(int argc, const char **argv);
void showHelp(const char *argv);
void readStdin(const char *letter, int position, int debug);
int preparePositionParam(int argc, const char **argv);
int charToInt(const char *integer);
int isWhiteCharacter(int code);
int isNumeric(int code);
int isChar(int code);
int isCapitalChar(int code);
int checkLetter(int input, const char *letter);



int main(int argc, const char **argv)
{
	
	if(!isSetParams(argc,argv))
	{
		fprintf(stderr,"Nejsou definovany nutne parametry, nebo jsou definovany spatne. Pro napovedu pouzijte prepinac --help\n");
		return RETURN_USAGE_ERROR; // Chybovy kod signalizujici spatne pouziti
	}
	else if(isHelp(argc, argv))
	{
		showHelp(argv[0]);
	}
	else
	{
		readStdin(argv[1], preparePositionParam(argc, argv), isDebug(argc,argv));
	}
	
	return 0;
}

int isSetParams(int argc, const char **argv)
{
	
	// Nespravny pocet argumentu
	if(argc == 1 || argc > 4)
	{
		return 0; 
	}
	
	// neni help vypis
	if(strcmp(argv[1],"--help") != 0)
	{
		// prvni argument je akceptovatelny znak
		if(strlen(argv[1]) != 1 || !(!isWhiteCharacter(argv[1][0]) || strcmp(argv[1],FILTER_ALL) == 0 || strcmp(argv[1],FILTER_CAPITAL) == 0 || strcmp(argv[1],FILTER_NUMBER) == 0 ))
		{
			return 0;
		}
		
		// Jestli je cislo OK
		if(argc >= 3 && strcmp(argv[2],"-d") != 0 && preparePositionParam(argc,argv) == -1)
		{
			return 0;
		}
	}
	
	return 1;
		
}

int isHelp(int argc, const char **argv)
{
	return ( argc == 2 && strcmp(argv[1],"--help") == 0 );
}

void showHelp(const char *argv)
{
	// Nazev spustitelneho souboru v napovede dostaneme z predanych hodnot funkci main, aby byl spravny i v pripade prejmenovani souboru proj1 na neco jineho
	printf(" Program se spouští v následující podobě (%s značí umístění a název programu a hranaté závorky reprezentují volitelnost daného argumentu programu):\n\n%s --help\nnebo\n%s X [N] [-d]\n\nArgumenty programu:\n\n--help způsobí, že program vytiskne nápovědu používání programu a skončí.\nX reprezentuje hledaný znak v počítaných slovech. Speciální znak : navíc symbolizuje libovolné číslo 0-9, znak ^ symbolizuje libovolné velké písmeno A-Z a znak . symbolizuje libovolný znak.\nVolitelný argument N představuje číslo vyjadřující pozici hledaného znaku v počítaných slovech. Platné počítané slovo musí mít hledaný znak práve na dané pozici. Číslo N je kladné - první znak ve slově má pozici 1.\nVolitelný argument -d zapíná tzv. ladicí (debug) režim programu. V ladicím řežimu program na výstup průběžně vypisuje každé načtené slovo, jedno slovo na řádek. Počet slov je programem vypsán teprve po skončení počítání.\n",argv,argv,argv);
}

int isDebug(int argc, const char **argv)
{
	// Prepinac -d muze byt jako 2 argument, pokud neni definovan hledany znak, jinak hledat na 3 pozici
	return ( (argc == 3 && strcmp(argv[2],"-d") == 0) || (argc == 4 && strcmp(argv[3],"-d") == 0) );
}

int checkLetter(int input, const char *letter)
{
	if(strcmp(letter,FILTER_ALL) == 0)
	{ // Jakýkoliv znak
		return (isChar(input) || isNumeric(input));
	}
	else if(strcmp(letter,FILTER_NUMBER) == 0)
	{ // Číslice
		return isNumeric(input);
	}
	else if(strcmp(letter,FILTER_CAPITAL) == 0)
	{ // Velká písmena
		return isCapitalChar(input);
	}
	else if(input == (int)letter[0])
	{ // Porovnáme s přesně specifikovaným znakem
		return 1;
	}
	
	return 0;
}

void readStdin(const char *letter, int position, int debug)
{
	int input;
	int i = 0;
	int found = 0;
	
	
	// Kontrola, jestli toto slovo jiz bylo zapocteno - nepocitat opakujici se pismena v jednom slove vickrat
	int counted = 0;
	
	// Dokud nedojde k stistku enteru
	while( (input = getchar()) != EOF)
	{
		
		i++;
		
		//        Odpovida pozice,        slovo nebylo zapocteno       a je pismenem jake chceme
		if( (position == i || position == 0) && !counted  && checkLetter(input, letter))
		{
			found++;
			counted = 1;
			
		}
		
		if(isWhiteCharacter(input) )
		{ // Pokud znak neni mezi povolenymy vynulujeme citac
			if(debug && i != 1)
			{
				printf("\n");
			}
			
			i = 0;
			counted = 0;
		}
		else if(debug && i <= DEBUG_MAXLEN)
		{ // Pokud je debug mod, tak vypiseme znak
			printf("%c",input);
		}


		
		
	}
	
	if(debug && i!=0)
	{ 
		// Znak noveho radku, pokud je debug mod, aby jsme vysledek nemeli na stejnem radku jako posledni slovo
		// druha cast podminky je pro odchyceni prazdneho radku pokud retezec konci na "prazdny" znak
		printf("\n");
	}
	printf("%d\n",found);
}

int preparePositionParam(int argc, const char **argv)
{
	if(argc > 2 && strcmp(argv[2],"-d") != 0)
	{ // Pokud je druhy parametr definovan a neni debug prepinac
		return charToInt(argv[2]);
	}
	
	return 0;
}

int isWhiteCharacter(int code)
{
	return (!isNumeric(code) && !isChar(code));
}

int isNumeric(int code)
{
	// Odpovida predany kod cislu?
	return (code >= LETTER_NUMBER_0 && code <= LETTER_NUMBER_9);
}

int isChar(int code)
{ 
	// Odpovida predany kod pismenum a-z
	if(LETTER_SMALL_A <= code && code <= LETTER_SMALL_Z)
	{
		return 1;
	}
	
	// Odpovida predany kod pismenum A-Z
	if(isCapitalChar(code))
	{
		return 1;
	}
	
	// "_"        nebo       "-"  
	if(LETTER_UNDERSCORE == code || code == LETTER_MINUS)
	{
		return 1;
	}
	
	return 0;
}

int isCapitalChar(int code)
{
	// Je kod reprezentaci znaku A-Z ?
	
	if(LETTER_CAPITAL_A <= code && code <= LETTER_CAPITAL_Z)
	{
		return 1;
	}
	
	return 0;
}

int charToInt(const char* integer)
{
	// Převod typu řetězec na integer
	int ret = 0;
	int unsigned i = 0;
	
	// to deleni sizeofu je implementace strlenu, abych nemusel includovat dalsi knihovnu
	while(i < strlen(integer))
	{
		if(isNumeric(integer[i]))
		{ // Zkontrolujem, jestli je to znak cisla, jinak ho budeme ignorovat
		
			// Protože procházíme zleva, tak předchozí výsledek posuneme o jeden řád a přičteme kod znaku od kterého odečteme kod nuly, aby jsme ziskali cislo, ktere kod reprezentuje
			ret = 10*ret + (int)integer[i] - LETTER_NUMBER_0;
		}
		else
		{
			return -1;
		}
		i++;
	}
	
	return ret;
}

