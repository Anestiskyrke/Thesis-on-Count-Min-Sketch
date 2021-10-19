// In this program we will determine the strength of the input password given by the user
// based on the frequency of the appearance of each password in the "passwords.txt"
// If the input given doesn't appear in the file then we give super strong!
// Scale goes from seriously give something better - super strong

// Anestis Kyrkenidis AM:3016 first attempt

// Include standard headers
# include "rangecms.h"


// Constructor for RangeQuery 
RangeCMSketch :: RangeCMSketch ( float e, float d, int lb, int md )
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

    // Logbits is the max size of bits, in our case 32
    // 1 is the level of detail in our sketches
    logbits = lb;

    levels = (int) ceil( log2(lb) + 1 );

    int shifter;
    /*
    for ( shifter = 0; shifter < levels; shifter++ )
    {
        if ( ( ( 1 << (1*shifter ) ) <= depth*width ) )
            bis = shifter;
    }
    */
    //bis = levels - bis;
    
    shifter = 1;
    mode = md;
    heavyreset = 0;
    heavy_counter = 0;

    // epsilon = (float) 2 / width;
    // confidence = 1 - 1 / pow( 2, depth );
    
    heavyhitter = new unsigned int [width*depth]; // used to store the heavy hitters
    // the first item of heavyhitter indicates the amount of heavy hitters
    // that exist in the data stream

    // each hash table is split into A and B with levels
    // this is done to ensure each level has its own random hashes
    // that don't overlap with the hashes of other levels

    range_table = new int* [levels];
    range_hashA = new int* [levels];
    range_hashB = new int* [levels];

    for (int i = 0; i < levels; i++ )
    {
        /*
        if ( i >= bis )
        {
            range_table[i] = (int *) calloc( 1<<(1*shifter), sizeof(int) );
            
            shifter++;
            cout << shifter << endl;
            range_hashA[i] = NULL;
            range_hashB[i] = NULL;
        }
        */
        range_table[i] = (int *) calloc( depth*width, sizeof(int) );
        range_hashA[i] = new int [depth];
        range_hashB[i] = new int [depth];
        
    }

    initHashAB( range_hashA, range_hashB, levels, depth );
    //initRangeTables( range_table, levels, width, depth, shifter );
    
}

void RangeCMSketch :: initRangeTables ( int** table, int levels, int width, int depth, int nmb )
{
    int i,j;

    for ( i = 0; i < levels; i++)
    {
        if ( i < bis )
        {
            for ( j = 0; j < width*depth; j++ )
            {
                table[i][j] = 0;
            }
        } 
    }

}


void RangeCMSketch :: initHashAB ( int** hashA, int** hashB, int l, int d )
{
    int i, j;
    srand( unsigned (time( NULL )) );

    for ( i = 0; i < l; i++ )
    {
        //if ( i < bis )
        //{
            for ( j = 0; j < d; j++ )
            {   
                hashA[i][j] = int(float( rand() ) * float( PRIME ) / float( RAND_MAX ) + 1);
                hashB[i][j] = int(float( rand() ) * float( PRIME ) / float( RAND_MAX ) + 1);
            }
        //}        
    }
    
}

unsigned int RangeCMSketch :: hash ( std::size_t item, int i, int j )
{
    std::size_t hash_item;
    // hash function : ( a * x + b ) mod p
    if ( mode == 0 )
        hash_item = ( ( long(range_hashA[i][j])*item ) 
        + range_hashB[i][j] ) % width;
    // hash function : ( ( a * x + b ) mod p ) mod w
    else if( mode == 1 )
        hash_item = ( ( ( long(range_hashA[i][j]) * item )
        + range_hashB[i][j] ) % PRIME ) % width;
    else if( mode == 2 )
    {
        hash_item = (long(range_hashA[i][j])*item) + range_hashB[i][j];
        hash_item += hash_item >> 32;
        hash_item &= PRIME;
        hash_item = ((int)hash_item) % width;
    }
    
    return (unsigned int) hash_item;
}

void RangeCMSketch :: addRangeItem ( int item, int times )
{

    size_t hash_item = item;
    int i, j, offset;
    
    // keep exact count
    count += times;
    for ( i = 0; i < levels; i++ )
    {
        offset = 0; // if outside the big for, it will go out of bounds 
        // thus creating memory problems
        /*
        if ( i >= bis )
        {
            //cout << hash_item << endl;
            range_table[i][hash_item] += times;
        }
        */
        for ( j = 0; j < depth; j++ )
        {
            range_table[i][ hash( hash_item, i, j) + offset ] += times;
            offset += width;
        }
        
        // the input is shifted to the right by "1"
        // this is done to ensure that every level has a different item
        // for example [1, n] has children [1, n/2] [n/2 + 1, n]
        // if 1 = 1, then hash_item >> 1 is equal to hash_item / 2
        hash_item = hash_item >> 1;
    }
}

int RangeCMSketch :: estimateCR ( int current, int item )
{

    int j;
    size_t hash_item = item;
    int estimate, offset;   
    estimate = RAND_MAX;
    offset = 0;

    if ( current >= levels )
        return return_actual_count();
    /*
    if ( current >= bis )
    {
        exact_count += range_table[current][hash_item];
        return range_table[current][hash_item];
    }
    */
    for ( j = 0; j < depth; j++ )
    {
        estimate = MIN( estimate, range_table[current][hash(hash_item, current, j ) + offset] );
        offset += width;
    }

    return estimate;
}

unsigned long int RangeCMSketch :: estimateRange ( int start, int end )
{
    int i, j, left, right, top;
    unsigned long int estimate;

    top = logbits;
    estimate = 0;

    //if( ( end > top ) && (start == 0)  )
    //    return count;

    end = MIN( top, end ); // must not exceed 1 << logbits
    
    end+=1;
    for ( i = 0; i < levels; i++ )
    {

        if ( start == end )
            break;
        
        if ( ( end - start + 1 ) < ( 1 << 1 ) )
        {
            for ( j = start; j < end; j++ )
                estimate += estimateCR( i, j );
            break;
        }
        else
        {
            left = ( ( (start>>1) +1 ) << 1 ) - start;
            right = end - ( ( end >> 1 ) << 1 );
            if ( left > 0 && ( start < end ) )
            {
                for ( j = 0; j < left; j++ )
                {
                    estimate += estimateCR( i, start + j );
                }
            }
            if ( right > 0 && ( start < end ) )
            {
                for ( j = 0; j < right; j++ )
                {
                    estimate += estimateCR( i, end - j - 1 );
                }
            }
            start = start >> 1;
            if ( left > 0 )
                start++;
            end = end >> 1;
        }

    }

    return estimate;

}

int RangeCMSketch :: findLeft ( unsigned long int sum )
{
    unsigned int i, maxim, l, h, mid;
    unsigned long int estimate;
    h = maxim = logbits;
    l = mid = 0;

    for ( i = 0; i < (maxim); i++ )
    {
        mid = ( l + h ) / 2;
        estimate = estimateRange( 0, mid );
        if ( estimate <= sum )
        {
            l = mid;
        } else 
        {
            h = mid;
        }
    }

    return mid;
}

int RangeCMSketch :: findRight ( unsigned long int sum )
{
    unsigned int i, maxim, l, h, mid;
    unsigned long int estimate;
    h = maxim = logbits; //  ( as in { 1....n } )
    l = mid = 0;

    for ( i = 0; i < (maxim); i++ )
    {
        mid = ( l + h ) / 2;
        estimate = estimateRange( mid, maxim );
        if ( estimate < sum )
        {
            h = mid;
        } else 
        {
            l = mid;
        }
    }
    
    return mid;
}

unsigned int RangeCMSketch :: findQuantile ( float fraction )
{
    int quantile, current_Left, current_Right;
    if ( fraction < 0 || fraction > 1 )
    {
        return 0;
    }
    
    current_Left = count * fraction;
    current_Right = count * ( 1- fraction );
    quantile = ( findLeft( current_Left ) + findRight( current_Right ) ) / 2 ;
  
    return quantile;
}

unsigned int* RangeCMSketch :: findHeavyHitters ( int current_depth, int start, unsigned long int mincount )
{

    int i, estimate, current_block;

    // if the first time we run the function
    // initialize the array ( set all items to zero ) 
    // since the array is a global variable
    if ( heavyreset == 0 )
    {
        heavyhitter = (unsigned int*) calloc(width*depth,sizeof(int));
        // set heavyreset to 1 since the program has started
        heavyreset = 1;
    }

    if ( current_depth < 0 )
        return heavyhitter;

    // get estimate based on the current depth and the given starting item 
    estimate = estimateCR( current_depth, start );
    // we need to return estimated heavy hitters that appear
    // more times than the given mincount
    if ( estimate >= mincount )
    {        
        if ( current_depth == 0 )
        {
            if ( heavy_counter < width*depth )
            {
                heavyhitter[ heavy_counter ] = start;
                heavy_counter++;
            }
        }
    

        // if not at the bottom level start recursion
        // lower current depth by 1, go to next block ( each block has size 1 << 1 )
        // keep same mincount
        else
        {
            current_block = start << 1; // left shift start by 1
            for ( i = 0; i < (1 << 1); i++ )
                findHeavyHitters( current_depth - 1, current_block + i, mincount );
        }

    }
    
    //heavyreset = 0;

    return heavyhitter;

}

unsigned long int RangeCMSketch :: return_actual_count ()
{
    return count;
}

int** RangeCMSketch :: return_table ( )
{
    return range_table;
}

int RangeCMSketch :: return_levels ()
{
    return levels;
}

std::size_t RangeCMSketch :: return_byte_size ()
{
    return width * depth * 8;
}

float RangeCMSketch :: return_epsilon ()
{
    return epsilon;
}

float RangeCMSketch :: return_delta ()
{
    return delta;
}

int RangeCMSketch :: return_width ()
{
    return width;
}

int RangeCMSketch :: return_depth ()
{
    return depth;
}

RangeCMSketch :: ~RangeCMSketch () 
{
    int i;
    
    for ( i = 0; i < levels; i++ )
    {
        delete[] range_hashA[i];
        delete[] range_hashB[i];    
        delete[] range_table[i];        
    }

    delete[] heavyhitter;
    delete[] range_hashA;
    delete[] range_hashB;
    delete[] range_table;
}

