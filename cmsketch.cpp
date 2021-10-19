// In this program we will determine the strength of the input password given by the user
// based on the frequency of the appearance of each password in the "passwords.txt"
// If the input given doesn't appear in the file then we give super strong!
// Scale goes from seriously give something better - super strong

// Anestis Kyrkenidis AM:3016 first attempt

// Include standard headers
# include "cmsketch.h"

// Basic Constructor 
CMSketch :: CMSketch ( float e, float d, int md ) 
{

    count = 0;
    // epsilon is 0.01 <= epsilon < 1
    // delta is 0 < delta < 1
    epsilon = e;
    delta = d;
    // new width and depth values 
    // as shown in a newer paper of the original Count-Min Sketch authors
    width = ceil( 2/epsilon );
    depth = ceil( -log2( 1-delta ) );

    mode = md;
    
    // epsilon = (float) 2 / width;
    // confidence = 1 - 1 / pow( 2, depth );
    table = new int* [depth];
    hashAB = new int* [depth];

    for (int i = 0; i < depth; i++ )
    {
        table[i] = new int [width];
        hashAB[i] = new int[2];
    }

    initTables( table, width, depth );
    initHashAB( hashAB, depth );

}

// Copy Constructor
CMSketch :: CMSketch ( const CMSketch& cms )
{

    count = 0;
    // epsilon is 0.01 <= epsilon < 1
    // delta is 0 < delta < 1
    epsilon = cms.epsilon;
    delta = cms.delta;
    // new width and depth values 
    // as shown in a newer paper of the original Count-Min Sketch authors
    width = ceil( 2/epsilon );
    depth = ceil( -log2( 1-delta ) );

    mode = cms.mode;

    // epsilon = (float) 2 / width;
    // confidence = 1 - 1 / pow( 2, depth );
    table = new int* [depth];
    hashAB = new int* [depth];

    for (int i = 0; i < depth; i++ )
    {
        table[i] = new int [width];
        hashAB[i] = new int[2];
    }

    initTables( table, width, depth );
    initHashAB( hashAB, depth );    

}

void CMSketch :: initTables ( int** table, int width, int depth )
{
    int i,j;

    for ( i = 0; i < depth; i++)
    {
        for ( j = 0; j < width; j++ )
        {
            table[i][j] = 0;
        }
    }

}

void CMSketch :: initHashAB ( int** hashAB, int depth )
{
    int i;
    srand( unsigned (time( NULL )) );

    for ( i = 0; i < depth; i++ )
    {   
        hashAB[i][0] = int(float( rand() ) * float( PRIME ) / float( RAND_MAX ) + 1);
        hashAB[i][1] = int(float( rand() ) * float( PRIME ) / float( RAND_MAX ) + 1);
    }

}

void CMSketch :: addItem ( int item, int times )
{
    std::size_t hash_item = item;
    int i;
    count += times;
    
    for ( i = 0; i < depth; i++ )
    {
        
        table[i][hash( hash_item, i )] += times;

    }
}

/*
void CMSketch :: addItem ( string item, int times )
{

    std::size_t hash_item = std::hash<std::string>{}(item);
    int i;
    count += times;
    
    for ( i = 0; i < depth; i++ )
    {
        
        table[i][hash( hash_item, i )] += times;

    }

}
*/

unsigned int CMSketch :: hash ( std::size_t item, int i )
{
    std::size_t hash_item;
    // hash function : ( a * x + b ) mod p
    if ( mode == 0 )
        hash_item = ( ( long(hashAB[i][0])*item ) 
        + hashAB[i][1] ) % width;
    // hash function : ( ( a * x + b ) mod p ) mod w
    else if( mode == 1 )
        hash_item = ( ( ( long(hashAB[i][0]) * item )
        + hashAB[i][1] ) % PRIME ) % width;
    else if( mode == 2 )
    {
        hash_item = (long(hashAB[i][0])*item);
        hash_item += hash_item >> 32;
        hash_item &= PRIME;
        hash_item = ((int)hash_item)  % width;
    }
        
    return (unsigned int) hash_item;
}

unsigned int CMSketch :: estimatePoint ( std::size_t item )
{
    std::size_t hash_item = item;
    int i;
    int estimate = RAND_MAX;

    for ( i = 0; i < depth; i++ )
    {
        
        estimate = MIN( estimate, table[i][hash( hash_item, i )] );
        
    }

    // should be estimate < actual_appearance + epsilon*N, 
    // where N is the sum of appearances of the inputs

    return estimate;

}

/*
unsigned int CMSketch :: estimatePoint ( string item )
{
    
    std::size_t hash_item = std::hash<std::string>{}(item);
    int i;
    int estimate = RAND_MAX;

    for ( i = 0; i < depth; i++ )
    {
        
        estimate = MIN( estimate, table[i][hash( hash_item, i )] );
        
    }

    // should be estimate < actual_appearance + epsilon*N, 
    // where N is the sum of appearances of the inputs

    return estimate;

}
*/

unsigned int CMSketch :: estimateInnerProduct ( int w, int d, int** new_table )
{
    int i, j;
    int temp, inner;
    inner = RAND_MAX;
    
    // check if the two CMSketches are the same ( in terms of width and depth )
    if ( width != w || depth != d )
    {
        cout << "The two Count-Min Sketches are not compatible. " << endl;
        return 0;
    }
    
    for ( j = 0; j < depth; j++ )
    {
        temp = 0;
        // std::inner_product exists in <numeric>
        for ( i = 0; i < width; i++ )
            temp += ( table[j][i] *  new_table[j][i]);

        inner = MIN( inner, temp );
    }

    return inner;
}

unsigned int CMSketch :: return_count (int max_hash)
{
    unsigned int cmscount = 0;
    
    for ( int i = 0; i < max_hash; i++ )
        cmscount += estimatePoint(i);

    return cmscount;
}

unsigned long int CMSketch :: return_actual_count ()
{
    return count;
}

int** CMSketch :: return_table ( )
{
    return table;
}

std::size_t CMSketch :: return_byte_size ()
{
    return width * depth * 8;
}

float CMSketch :: return_epsilon ()
{
    return epsilon;
}

float CMSketch :: return_delta ()
{
    return delta;
}

int CMSketch :: return_width ()
{
    return width;
}

int CMSketch :: return_depth ()
{
    return depth;
}


CMSketch :: ~CMSketch () 
{
    int i;
    for ( i = 0; i < depth; i++ )
    {
        delete[] table[i];
        delete[] hashAB[i];
    }
    
    delete[] table;
    delete[] hashAB;

}

