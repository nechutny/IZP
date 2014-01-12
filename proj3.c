/*
 * 	Author:			Stanislav Nechutný (xnechu01)
 *	Name:			3. project IZP at FIT VUT
 *	Revision:		9
 *	Repository:		git@nechutny.net:vut.git
 *	Created:		2013-11-24 16:41
 *	Modified:       2013-12-02 14:06
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int rows;
    int cols;
    char *cells;
} Bitmap;

char getcolor(Bitmap *bitmap, int x, int y);

int find_vline(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2);
int find_hline(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2);
int find_square(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2);

int search_line(Bitmap *bitmap, int vertical, int size1, int size2, int *x1, int *y1, int *x2, int *y2);

// Garbage
void clean(Bitmap *bitmap);

Bitmap create_bitmap(const char *file, int *error);

// Prototypes for informing functions
int throw_error(int code);
void show_help();

void print_image(Bitmap *bitmap);


// Return erros coresponding to BSD sysexists.h
#define RETURN_USAGE_ERROR          64
#define RETURN_FORMAT_ERROR         65
#define RETURN_OPEN_ERROR           66
#define RETURN_SYSTEM_ERROR         71

// Functions return errors
#define ERROR_NO_ERROR             0
#define ERROR_FILE_NOT_FOUND       1
#define ERROR_BAD_PARAMS           2
#define ERROR_INVALID_FILE         3
#define ERROR_ALLOC_ERROR          4


/**
 * Main function controling app
 * @param   int   number of arguments
 * @param   char  two dimensional array of given arguments
 * @return  int
 */
int main(int argc, const char **argv)
{
    int error = ERROR_NO_ERROR;
    
    if(argc == 2 && strcmp(argv[1],"--help") == 0)
    {
        show_help();
        return ERROR_NO_ERROR;
    }
    
    if(argc < 3)
    {
        return RETURN_USAGE_ERROR;
    }
    
    Bitmap bitmap = create_bitmap(argv[2],&error);

    if(error != ERROR_NO_ERROR && !(strcmp(argv[1],"--test") == 0 && error == ERROR_INVALID_FILE) )
    {
        clean(&bitmap);
        return throw_error(error);
    }
    
    if(strcmp(argv[1],"--test") == 0)
    {
        if(error == ERROR_INVALID_FILE)
        {
            printf("Invalid\n");
        }
        else
        {
            printf("Valid\n");
        }
        
        clean(&bitmap);
        return ERROR_NO_ERROR;
    }
    
    int x1 = 0,x2 = 0,y1 = 0,y2 = 0;
    
    if(strcmp(argv[1],"--vline") == 0)
    {
        find_vline(&bitmap,&x1,&y1,&x2,&y2);
    }
    else if(strcmp(argv[1],"--hline") == 0)
    {
        find_hline(&bitmap,&x1,&y1,&x2,&y2);
    }
    else if(strcmp(argv[1],"--square") == 0)
    {
        find_square(&bitmap,&x1,&y1,&x2,&y2);
    }
    
    printf("%d %d %d %d\n",x1,y1,x2,y2);
    
    clean(&bitmap);
    return ERROR_NO_ERROR;
}



/**
 * Searching for biggest square in bitmap via getcolor()
 * @param   &bitmap reference on bitmap structure
 * @param   &int    coord x1 - start of square
 * @param   &int    coord y1 - start of square
 * @param   &int    coord x2 - end of square
 * @param   &int    coord y2 - end of square
 * @return  int
 */
int find_square(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2)
{
    
    int biggest = 0, k, i = 0, j, l, cache;
    while(i < bitmap->rows-biggest-1)
    { //loop on cols until can fit in bigger square
        j = 0;
        while(j < bitmap->cols-biggest-1)
        { //loop on rows until can fit in bigger square

            if(getcolor(bitmap, i,j) == 1)
            { // found upper left pixel from possible square, so lets search

                k = 1;
                
                while(i+k < bitmap->rows && j+k < bitmap->cols)
                {
                    cache = getcolor(bitmap,i+k,j) == 1 && getcolor(bitmap,i,j+k) == 1;
                    if(cache && k>biggest)
                    { // Next point from two sides (top, left) have right color

                        l = 1;
                        while(l <= k)
                        { // check remaining two sides (bottom, right)
                            if((l != k && (getcolor(bitmap,i+k,j+l) != 1 || getcolor(bitmap,i+l,j+k) != 1)) || getcolor(bitmap,i+k,j+l) != 1 )
                            { // Not square
                                break;
                            }
                            l++;
                        }
                        if(l > k && biggest < k)
                        { // Found bigger square, so lets save it
                            biggest = k;
                           
                            *x1 = i;
                            *y1 = j;
                            *x2 = i+biggest;
                            *y2 = j+biggest;
                        }
                    }
                    else if(!cache)
                    { // Got bad color pixel - end searching
                        break;
                    }
                    k++;
                }
            }
            j++; // next row
        }
        i++; // next column
    }
    
    if(biggest != 0)
    {
        return biggest;
    }
    
    return 0;
    
    
}

/**
 * Searching for longest line in bitmap via getcolor()
 * @param   &bitmap reference on bitmap structure
 * @param   int     vertical = 1, horizontal = 0
 * @param   int     first size
 * @param   int     second size
 * @param   &int    coord x1 - start of line
 * @param   &int    coord y1 - start of line
 * @param   &int    coord x2 - end of line
 * @param   &int    coord y2 - end of line
 * @return  int
 */
int search_line(Bitmap *bitmap, int vertical, int size1, int size2, int *x1, int *y1, int *x2, int *y2)
{
    char color;
    int length = 0, longest = 0, i,j,k;
    
    i = 0;
    while( i < size1)
    {
        
        j = 0;
        while(j < size2-1)
        {
            length = 0;
            
            k = j;
            while(k < size2)
            {
                if(vertical == 1)
                {
                    color = getcolor(bitmap,k,i);
                }
                else
                {
                    color = getcolor(bitmap,i,k);
                }
                
                if(color == 0)
                {
                    break;
                }
                else
                {
                    length++;
                }
                k++;
            }
            
            if(length > longest)
            {
                longest = length;
                if(vertical == 1)
                {
                    *x1 = j;
                    *y1 = i;
                    *x2 = k-1;
                    *y2 = i;
                }
                else
                {
                    *x1 = i;
                    *y1 = j;
                    *x2 = i;
                    *y2 = k-1;
                }
            }
            if(length != 0)
            { // Optimalization for skipping checked pixels
                j = k;
                
            }
            
            if(size2-j < longest)
            { // skip if remain less pixels, then is longest found vector
                break;
            }
            
            j++;
        }
        
        i++;
    }
    
    if(longest < 2)
    {
        return 0;
    }
    
    return longest;
}

/**
 * Searching for longest horizontal line in bitmap via getcolor()
 * @param   &bitmap reference on bitmap structure
 * @param   &int    coord x1 - start of line
 * @param   &int    coord y1 - start of line
 * @param   &int    coord x2 - end of line
 * @param   &int    coord y2 - end of line
 * @return  int
 */
int find_hline(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2)
{
    return search_line(bitmap,0,bitmap->rows,bitmap->cols,x1,y1,x2,y2);
}


/**
 * Searching for longest vertical line in bitmap via getcolor()
 * @param   &bitmap reference on bitmap structure
 * @param   &int    coord x1 - start of line
 * @param   &int    coord y1 - start of line
 * @param   &int    coord x2 - end of line
 * @param   &int    coord y2 - end of line
 * @return  int
 */
int find_vline(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2)
{
    return search_line(bitmap,1,bitmap->cols,bitmap->rows,x1,y1,x2,y2);
}

/**
 * Print bitmap for debugging
 * @param   &bitmap reference on bitmap structure
 */
void print_image(Bitmap *bitmap)
{
    printf("Sloupcu: %d | Radek: %d\n",bitmap->cols,bitmap->rows);
    printf("   ");
    for(int i = 0; i < bitmap->cols; i++)
    {
        printf(" %d ",i);
    }
    printf(" x ");
    
    for(int i = 0; i < bitmap->rows; i++)
    {
        printf("\n");
        printf(" %d ",i);
        for(int j = 0; j < bitmap->cols; j++)
        {
            printf(" %d ",getcolor(bitmap,i,j));
        }
    }
    
    printf("\n y \n");
}

/**
 * Create structure bitmap with correct values
 * @param   &char   file name for reading bitmap
 * @param   &int    error checking
 * @return  bitmap
 */
Bitmap create_bitmap(const char *file, int *error)
{
    Bitmap result;
    result.cells = NULL;
    int col = 0, row = 0, readed = 0;
    FILE *fp;
    
    fp = fopen(file, "r");
    if (fp == NULL)
    { // can't open file
        *error = ERROR_FILE_NOT_FOUND;
        return result;
    }
    
    readed = fscanf(fp, "%d %d", &row, &col);
    
    if(readed != 2)
    { // no two numbers with cols and rows
        *error = ERROR_INVALID_FILE;
        fclose(fp);
        return result;
    }
    
    
    
    char *data = malloc(sizeof(char)*col*row);
    if(!data)
    { // can't alloc memory
        *error = ERROR_ALLOC_ERROR;
        fclose(fp);
        return result;
    }

    result.cells = data;

    int point;
    int i = 0;
    // %1d for good reading 01 as invalid
    while((readed = fscanf(fp, "%*[ \n\t]%1d", &point)) != EOF)
    { // reading data
        if(readed != 1)
        { // invalid file - no valid char
            *error = ERROR_INVALID_FILE;
            fclose(fp);
            return result;
        }
        if(point != 0 && point != 1)
        { // invalid file - no valid number
            *error = ERROR_INVALID_FILE;
            fclose(fp);
            return result;
        }
        if(i >= col*row)
        { // invalid file - too much values
            *error = ERROR_INVALID_FILE;
            fclose(fp);
            return result;
        }
        data[i] = point;
        i++;
    }
    
    if(i < row*col)
    { // invalid file - not enought values
        *error = ERROR_INVALID_FILE;
        fclose(fp);
        return result;
    }

    
    
    result.rows = row;
    result.cols = col;
    result.cells = data;
    
    fclose(fp);
    return result;
}


/**
 * Get color of pixel from bitmap.cells
 * @param   &bitmap reference on bitmap structure
 * @param   int    coord x - row
 * @param   int    coord y - col
 * @return  char
 */
char getcolor(Bitmap *bitmap, int x, int y)
{
    if(x >= bitmap->rows || y >= bitmap->cols || x<0 || y<0)
    {
        fprintf(stderr,"Pristup k neexistujicimu pixelu! [%d,%d]\n",x,y);
        return ERROR_BAD_PARAMS;
        
    }
    
    return bitmap->cells[x*bitmap->cols+y];
}

/**
 * Cleaning allocated memory
 * @param   &bitmap reference on bitmap structure
 */
void clean(Bitmap *bitmap)
{
    if(bitmap->cells != NULL)
    {
        free(bitmap->cells);
    }
}

/**
 * Show error message on stderr and return correct return code for exiting app
 * @param   int   one of ERROR* codes
 * @return  int
 */
int throw_error(int code)
{
    switch(code)
    {
        case(ERROR_INVALID_FILE):
        {
            fprintf(stderr,"Soubor nema spravny format.");
            return RETURN_FORMAT_ERROR;
        }
        case(ERROR_FILE_NOT_FOUND):
        {
            fprintf(stderr,"Soubor nemohl byt otevren.");
            return RETURN_OPEN_ERROR;
        }
        case(ERROR_ALLOC_ERROR):
        {
            fprintf(stderr,"Chyba alokovani pameti.");
            return RETURN_SYSTEM_ERROR;
        }
    }
    
    return 1;
}

/**
 * Show help message
 */
void show_help()
{
    printf("Argumenty programu:\n\n");
           
        printf("\t--help \tzpůsobí, že program vytiskne nápovědu používání programu a skončí.\n");
        printf("\t--test \tpouze zkontroluje, že soubor daný druhým argumentem programu obsahuje řádnou definici bitmapového obrázku. V případě, že formát obrázku odpovídá definici (viz níže), vytiskne Valid. V opačném případě (např. ilegálních znaků, chybějících údajů nebo špatných hodnot) program tiskne Invalid.\n");
        printf("\t--hline \tv daném obrázku nalezne a vytiskne počáteční a koncovou souřadnici první nejdelší horizontální úsečky.\n");
        printf("\t--vline \tv daném obrázku nalezne a vytiskne počáteční a koncovou souřadnici první nejdelší vertikální úsečky.\n");
        printf("\t--square \tv daném obrázku nalezne a vytiskne počáteční a koncovou souřadnici prvního největšího čtverce.\n");
}

