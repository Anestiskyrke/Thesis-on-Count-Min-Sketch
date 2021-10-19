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

class CMSketch 
{

    float epsilon, delta;
    unsigned long int count;
    int width, depth;
    int mode;
    int** table;
    int** hashAB;
    
    void initTables ( int** table, int width, int depth );

    void initHashAB ( int** hashAB, int depth );

public:

    CMSketch ( float epsilon, float delta, int md );

    CMSketch ( const CMSketch &cms );

    void addItem ( int item, int times );

    //void addItem ( string item, int times);

    unsigned int hash ( std::size_t item, int i );

    unsigned int estimatePoint ( std::size_t item );

    //unsigned int estimatePoint ( string item );
    
    unsigned int estimateInnerProduct ( int width, int depth, int** new_table );

    unsigned int return_count (int max_hash);

    unsigned long int return_actual_count ();
    
    int** return_table ();

    std::size_t return_byte_size ();

    float return_epsilon ();

    float return_delta ();
    
    int return_width ();

    int return_depth ();
    
    ~CMSketch ();

};