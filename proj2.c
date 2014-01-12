/*
 * 	Author:			Stanislav Nechutný (xnechu01)
 *	Name:			2. project IZP at FIT VUT
 *	Revision:		7
 *	Repository:		git@nechutny.net:vut.git
 *	Created:		2013-10-07 18:49
 *	Modified:       2013-11-22 14:06
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create structures for better readable code
typedef struct
{
    double x, y;
} point;

typedef struct
{
    double a, b, c;
    point A,B,C;
    int error;
    
} triangle;

int check_params(int argc,const char **argv);

// Define prototypes for math functions
double my_sqrt(double x);
double my_asin(double x);
double my_acos(double x);
double my_abs(double x);

// Prototypes of function preparing triangle structure
triangle trinagle_create(const char **argv);
void triangle_fill(point *point, const char **argv, int i, triangle *triangle);
void triangle_validate(triangle *data);

// Prorotypes for function calculation on triangle structure
int calculate_triangle(triangle data);
double triangle_side_length(point A, point B);
double calculate_triangle_angle(double a, double b, double c);

// Prototypes for informing functions
void throw_ussage_error();
void show_help();

#define PI                          3.141592653589793
#define RETURN_USAGE_ERROR          64
#define RETURN_NO_ERROR             0
#define RETURN_SIGNED_ERROR         -1
#define RETURN_NAN_ERROR            -2
#define RETURN_NOT_VALID_TRIANGLE   -3



/**
 * Main function controling app
 * @param   int   number of arguments
 * @param   char  two dimensional array of given arguments
 * @return  int
 */
int main(int argc, const char **argv)
{
    
	if(!check_params(argc,argv))
    { // Not enought, or bad arguments
        throw_ussage_error();
        return RETURN_USAGE_ERROR;
    }
    
    if(strcmp(argv[1], "--help") == 0)
    { // help
        show_help();
    }
	else if(strcmp(argv[1],"--sqrt") == 0)
    { // sqrt
        double x = atof(argv[2]);
        
        // Check on conversion for non-int
        if(x == 0.0 && !(argv[2][0] == '0' || argv[2][0] == '+' || argv[2][0] == '-'))
        {
            throw_ussage_error();
            return RETURN_USAGE_ERROR;
        }
        
        x = my_sqrt(x);
        
        if(x == RETURN_SIGNED_ERROR)
        {
            printf("nan\n");
        }
        else
        {
            printf("%.10e\n",x);
        }
	}
    else if(strcmp(argv[1],"--asin") == 0)
    { // asin
        double x = atof(argv[2]);
        
        // Check on conversion for non-int
        if(x == 0.0 && !(argv[2][0] == '0' || argv[2][0] == '+' || argv[2][0] == '-'))
        {
            throw_ussage_error();
            return RETURN_USAGE_ERROR;
        }
        
        // if is signed, then set sign to -1, compute absolute value and after computing asin add back -
        int sign = 1;
        if(x < 0)
        {
            sign = -1;
        }
        
        x = my_abs(x);
        if(x > 1)
        {
            printf("nan\n");
        }
        else
        {
            double result = my_asin(x);
            printf("%.10e\n",result*sign);
        }
	}
    else if(strcmp(argv[1],"--triangle") == 0)
    { // triangle
        triangle data = trinagle_create(argv);
        if(data.error != RETURN_NO_ERROR)
        {
            throw_ussage_error();
            return RETURN_USAGE_ERROR;
        }
        
        
        return calculate_triangle(data);
    }
	
	
	return RETURN_NO_ERROR;
}

/**
 * Check, if is set all required arguments
 * @param   int     number of argumets
 * @param   char    argv(array of arrays - arguments)
 * @return  int
 */
int check_params(int argc,const char **argv)
{
    if(argc < 2)
    { // Enought arguments?
        return 0;
    }
    else if(strcmp(argv[1],"--help") != 0 && argc < 3)
    { // Enought arguments for sqrt and asin?
        return 0;
    }
    else if(strcmp(argv[1],"--triangle") == 0 && argc<8)
    { // Enought arguments for triangle?
        return 0;
    }
    return 1;
}

/**
 * Create structure representing triangle
 * @param   char    argv(array of arrays - arguments)
 * @return  triagle
 */
triangle trinagle_create(const char **argv)
{
    triangle result;
    
    result.error = RETURN_NO_ERROR;
    
    triangle_fill(&result.A,argv,2,&result);
    triangle_fill(&result.B,argv,4,&result);
    triangle_fill(&result.C,argv,6,&result);
    
    if(result.error != RETURN_NO_ERROR)
    { // If we got error from filling (NaN), then is no reason for continue
        return result;
    }
    
    result.a = triangle_side_length(result.B,result.C);
    result.b = triangle_side_length(result.A,result.C);
    result.c = triangle_side_length(result.B,result.A);
    
    triangle_validate(&result);
    
    return result;
}

/**
 * Fill point with correct position and check NaN.
 * @param   &point      reference on point structure for saving coords
 * @param   char        arguments with positions as strings
 * @param   int         offset for reading point coords from arguments
 * @params  &trinagle   reference on triangle structure for defining error if needed
 */
void triangle_fill(point *point, const char **argv, int i, triangle *triangle)
{
    point->x = atof(argv[i]);
    point->y = atof(argv[i+1]);
    
  
    if( (point->x == 0.0 && !(argv[i][0] == '0' || argv[i][0] == '+' || argv[i][0] == '-') ) || (point->y == 0.0 && !(argv[i+1][0] == '0' || argv[i+1][0] == '+' || argv[i+1][0] == '-')))
    { // Check, if is argument corectly converted to double. If is converted result 0, but argument doesn't start with 0, then is there something wrong
        triangle->error = RETURN_NAN_ERROR;
    }
}

/**
 * Validate triangle - sum of two smaller sides must be greater than biggest one. If error, then is set error property
 * @param   &triangle   reference on triangle structure
 */
void triangle_validate(triangle *data)
{
    double small1, small2, big;
    
    // Sort three values so in variables small1 and small2 should be length of two shorter sides and in variable big longest
    if(data->a > data->b)
    {
        small1 = data->b;
        if(data->a > data->c)
        {
            small2 = data->c;
            big = data->a;
        }
        else
        {
            small2 = data->a;
            big = data->c;
        }
    }
    else
    {
        small1 = data->a;
        if(data->b > data->c)
        {
            small2 = data->c;
            big = data->b;
        }
        else
        {
            small2 = data->b;
            big = data->c;
        }
    }
    
    // if is sum of two shorter sides smaller, or equal to longest, then it can't be valid triangle
    if(small1+small2 <= big)
    {
        data->error = RETURN_NOT_VALID_TRIANGLE;
    }

}



/**
 * Compute distance between two points
 * @param   point   structure representing point A
 * @param   point   structure representing point B
 * @return  double
 */
double triangle_side_length(point A, point B)
{
    return my_sqrt( (A.x-B.x)*(A.x-B.x) + (A.y-B.y)*(A.y-B.y) );
}

/**
 * Compute and print angles in triangle
 * @param   triangle   structure representing triangle
 * @return  int
 */
int calculate_triangle(triangle data)
{
    // Performance tweak - faster is calculating angles only two times and third calculate PI/2-(angle1+angle2)
    double alpha, beta;
    alpha = calculate_triangle_angle(data.a,data.b,data.c);
    printf("%.10e\n",alpha);
    beta = calculate_triangle_angle(data.b,data.c,data.a);
    printf("%.10e\n",beta);
    printf("%.10e\n",PI-beta-alpha);
    
    return 0;
}

/**
 * Compute angle in triangle between sides at second and third argument (opposite first argument/side)
 * @param   double  length of side a
 * @param   double  length of side b
 * @param   double  length of side c
 * @return  double
 */
double calculate_triangle_angle(double a, double b, double c)
{
    return my_acos( ( b*b - a*a + c*c) / (2*b*c) );
}

/**
 * Compute arccos - depend on arcsin
 * @param   double   number for compute arccos
 * @return  double
 */
double my_acos(double x)
{
    return PI/2 - my_asin(x);
}

/**
 * Compute arcsin
 * @param   double   number for compute arcsin
 * @return  double
 */
double my_asin(double x)
{
    x = my_abs(x);
    // For X>=PI/4 is this optimalization
    if(x >= PI/4)
    {
        return 0.5*PI - my_asin(my_sqrt(1-x*x));
    }
    
    double result = x;
    double i = 0;
    
    // Cache coef and root of X, for better performace
    double coef = 1;
    double pow_cache = x;
    double cache = 0;
    double pow_x = x*x;
    
    while(my_abs(cache-result) > 1e-15*result)
    { // Calculate to fit accuracy
        
        // Taylor theorem with performance tweaks (caching repeating values)
        cache = result;
        i++;
        coef = coef*i;
        i++;
        coef = coef/i;
        pow_cache = pow_cache * pow_x;
        result = result + coef*pow_cache/(i+1);
        
       
    }
    
    return result;
}



/**
 * Compute square of given argument
 * @param   double   number for compute sqrt
 * @return  double
 */
double my_sqrt(double x)
{
    if(x == 0)
    {
        return 0;
    }
    if(x < 0)
    {
        return RETURN_SIGNED_ERROR;
    }
    double result = 1;
    double cache = 0;
    while(my_abs(cache-result) > 1e-18*result )
    {
        cache = result;
        result = ( (x/result) + result)/2;
        
    }
    
    return result;
}


/**
 * Compute absolute value of given parameter
 * @param   double
 * @return  double
 */
double my_abs(double x)
{
    // Be cool and use bitmask, for better performance
    //unsigned long long* temp = (unsigned long long*)&x;
    //*temp &= 0x7fffffffffffffff;

    //return x;
    
    return (x < 0)? -x : x;
}

/**
 * Print on stderr information about error
 * @param   void
 * @return  void
 */
void throw_ussage_error()
{
    fprintf(stderr,"Nejsou definovany nutne parametry, nebo jsou definovany spatne. Pro napovedu pouzijte prepinac --help\n");
}

/**
 * Print on help message
 * @param   void
 * @return  void
 */
void show_help()
{
    printf("Argumenty programu:\n");
        printf("\t --help\n");
            printf("\t\t způsobí, že program vytiskne nápovědu používání programu a skončí.\n");
        printf("\t --sqrt X\n");
            printf("\t\t vypočítá a vypíše druhou odmocninu z čísla X.\n");
        printf("\t --asin X\n");
            printf("\t\t vypočítá a vypíše arcus sinus z čísla X.\n");
        printf("\t --triangle AX AY BX BY CX CY\n");
            printf("\t\t vypočítá a vypíše tři úhly troujúhelníku, který je dán vrcholy A=AX,AY, B=BX,BY a C=CX,CY.\n");
    printf("\nAutor: Stanislav Nechutny - xnechu01\n");
    
}

