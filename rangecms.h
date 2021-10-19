// In this program we will determine the strength of the input password given by the user
// based on the frequency of the appearance of each password in the "passwords.txt"
// If the input given doesn't appear in the file then we give super strong!
// Scale goes from seriously give something better - super strong

// Anestis Kyrkenidis AM:3016 first attempt

// Include standard headers
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <cstdlib>
# include <iostream>
# include <fstream>
# include <sstream>
# include <vector>
# include <string>
# include <cstring>
# include <algorithm>
# include <iterator>
# include <iomanip>
# include <cmath>
# include <map>
# include <ctime>
# include <climits>
# include <functional> 
# include <math.h>
# include <numeric>

# define PRIME 4294967311l
# define MIN(a,b) ( a < b ? a : b )
# define MAX(a,b) ( a > b ? a : b )

using namespace std;

class RangeCMSketch 
{

    float epsilon, delta;
    unsigned long int count;
    int width, depth, levels;
    int bis, logbits;
    int mode, heavyreset, heavy_counter;
    unsigned int* heavyhitter;
    int** range_hashA;
    int** range_hashB;
    int** range_table;
    
    void initRangeTables ( int** range_table, int levels, int width, int depth, int nmb );

    void initHashAB ( int** hashA, int** hashB, int levels, int depth );

public:


    RangeCMSketch ( float epsilon, float delta, int logbits, int md );

    unsigned int hash ( std::size_t item, int i, int j );

    void addRangeItem ( int item, int times);

    int estimateCR ( int current_level, int item );

    unsigned long int estimateRange ( int start, int end );

    int findLeft ( unsigned long int sum );

    int findRight ( unsigned long int sum );

    unsigned int findQuantile ( float fraction );

    unsigned int* findHeavyHitters ( int depth, int start, unsigned long int mincount );

    unsigned long int return_actual_count ();
    
    int** return_table ();

    int return_levels ();

    std::size_t return_byte_size ();

    float return_epsilon ();

    float return_delta ();
    
    int return_width ();

    int return_depth ();

    ~RangeCMSketch ();    

};