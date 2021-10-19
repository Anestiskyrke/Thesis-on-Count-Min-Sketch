// In this program we will determine the strength of the input number given by the user
// based on the frequency of the appearance of each number in the "numbers.txt"
// If the input given doesn't appear in the file then we give super strong!
// Scale goes from seriously give something better - super strong

// Anestis Kyrkenidis AM:3016 first attempt

// Include standard headers
# include "cmsketch.h"
# include "rangecms.h"

// ----- Functions Used ----- //
int CountLines( char* filename );

std::ofstream csvfile; // CSV output file
int flag = 0; // this flag is used to check if file has already outputs inside
int format_type = 0; // another flag used to check the format of the input file
int lines; // the lines of the file ( or the items listed in the file )
// https://stackoverflow.com/questions/43403613/c-difference-between-unsigned-int-and-unsigned-long-int
unsigned long int total = 0;
map<int, map<int, int>> mapitems;
int* counts; // stores exact counts of the items in the datasets
map<int, map<int, int>>::iterator itr;
map<int, int>::iterator ptr;

// ----- Global Variables ----- //

int CountLines( char* filename );

void BMSAddItem( char* filename, CMSketch* item1, CMSketch* item2, RangeCMSketch* item3);

void BMSEstimateItem( char* filename, CMSketch* item );

void SPMFAddItem( char* filename, CMSketch* item1, CMSketch* item2, RangeCMSketch* item3);

void SPMFEstimateItem( char* filename, CMSketch* item );

int CountLines( char* filename ) 
{

    std::fstream afile(filename);
    int lines;

    lines = std::count(std::istreambuf_iterator<char>(afile), 
            std::istreambuf_iterator<char>(), '\n');

    afile.close();

    return lines; 

}

void BMSAddItem( char* filename, CMSketch* item1, CMSketch* item2, RangeCMSketch* item3)
{
    
    std::ifstream afile(filename);
    std::string line; // the line extracted from the file
    int numbers; // stores the number we hash
    int times; // stores the times the number appears

    // read the lines and add to query
    while (std::getline(afile,line))
    {
        
        stringstream content(line);
        content >> numbers;
        content >> times;
        item1->addItem( numbers, times );
        item2->addItem( numbers, times );
        item3->addRangeItem( numbers, times );

        counts[numbers] = times;
    }

    afile.close();

}

void BMSEstimateItem( char* filename, CMSketch* item, float epsilon )
{

    unsigned int estimate = 0; // the estimate for a point of the query
    int id; // stores the number we hash
    int times; // stores the times the number appears
    unsigned int cm_count = item->return_actual_count();

    // note: since probablistic (and not deterministic)
    // might not be accurate sometimes 
    for ( id = 0; id < lines; id++)
    {
        estimate = item->estimatePoint( id );
        total+=estimate;
        // store estimates in a map 
        // the number is inserted first ( seen as key )
        // then the times that it appears ( seen as key, functions as value )
        // ( due to the structure of map )
        // and then the estimate ( seen and functions as value )
        if( estimate >= (epsilon*cm_count) + counts[id] )
        {
            mapitems.insert( make_pair( id, map<int, int>() ) );
            mapitems[id].insert( make_pair( counts[id], estimate) );
        }

        if( estimate < counts[id] )
        {
            cout << "ERROR! " << id << " with " << counts[id] << " appears fewer times " << estimate << endl;
        }

    }

}


void IBMAddItem( char* filename, CMSketch* item1, CMSketch* item2, RangeCMSketch* item3)
{

    std::ifstream afile(filename);
    std::string line; // the line extracted from the file
    int id = 0; // stores the number we hash
    int itemset; // stores the times the number appears
    int endset; // stores the -1 or -2
    unsigned int counter; // stores the sum of values in each line

    // read the lines and add to query
    while (std::getline(afile,line))
    {
        counter = 0;
        stringstream content(line);
        while( 1 )
        {
            content >> itemset;
            if ( itemset == -2 && endset == -1 )
                break;
            counter += itemset;
            content >> endset;
            item1->addItem( id, itemset );
            item2->addItem( id, itemset );
            item3->addRangeItem( id, itemset );
        }

        counts[id] = counter;
        id++;
    }

    afile.close();

}

void IBMEstimateItem( char* filename, CMSketch* item, float epsilon )
{

    
    unsigned int estimate = 0; // the estimate for a single point query
    int id; // stores the number we hash
    unsigned int cm_count = item->return_actual_count(); // CM Sketch count    
    // read the lines and add to query
    for ( id = 0; id < lines; id++ )
    {

        estimate = item->estimatePoint( id );
        total+=estimate;
        
        // store estimates in a map 
        // the number is inserted first ( seen as key )
        // then the times that it appears ( seen as key, functions as value )
        // ( due to the structure of map )
        // and then the estimate ( seen and functions as value )
        if( estimate >= (epsilon*cm_count) + counts[id] )
        {
            mapitems.insert( make_pair( id, map<int, int>() ) );
            mapitems[id].insert( make_pair( counts[id], estimate) );
        }

        if( estimate < counts[id] )
        {
            cout << "ERROR! " << id << " with " << counts[id] << " appears fewer times " << estimate << endl;
        }

    }

}

void KosarakAddItem( char* filename, CMSketch* item1, CMSketch* item2, RangeCMSketch* item3)
{

    std::ifstream afile(filename);
    std::string line; // the line extracted from the file
    int id = 0; // stores the number we hash
    int itemset; // stores the times the number appears
    unsigned int counter; // stores the sum of values in each line

    // read the lines and add to query
    while (std::getline(afile,line))
    {
        counter = 0;
        stringstream content(line);
        while( content >> itemset )
        {
            counter += itemset;
            item1->addItem( id, itemset );
            item2->addItem( id, itemset );
            item3->addRangeItem( id, itemset );
        }

        counts[id] = counter;
        id++;
    }

    afile.close();

}

void KosarakEstimateItem( char* filename, CMSketch* item, float epsilon )
{

    
    unsigned int estimate = 0; // the estimate for a point of the query
    int id; // stores the number we hash
    int itemset; // stores the times the number appears
    long int cm_count = item->return_actual_count(); // CM Sketch count    

    // read the lines and add to query
    for ( id = 0; id < lines; id++ )
    {
        estimate = item->estimatePoint( id );
        //total+= estimate;

        // store estimates in a map 
        // the number is inserted first ( seen as key )
        // then the times that it appears ( seen as key, functions as value )
        // ( due to the structure of map )
        // and then the estimate ( seen and functions as value )
        if( estimate >= (epsilon*cm_count) + counts[id] )
        {
            mapitems.insert( make_pair( id, map<int, int>() ) );
            mapitems[id].insert( make_pair( counts[id], estimate) );
        }
        if( estimate < counts[id] )
        {
            cout << "ERROR! " << id << " with " << counts[id] << " appears fewer times " << estimate << endl;
        }
            
    }

}

void OutputResults( char* inputfile, float epsilon, float delta )
{

    char filename[60];
    int mode, i;
    strcpy( filename, inputfile );

    lines = CountLines( inputfile ); // number of lines in the file
    
    // we initialize with calloc
    // calloc is more preferable because we don't have to initialize
    // each item to zero if we used malloc
    // calloc is the same as malloc but initializes each item to zero
    counts = (int*) calloc( lines, sizeof(int) );

    std::string s = string(filename);
    if ( format_type == 0 )
    {
        if ( s.find("BMS/") != string::npos )
        {
            format_type = 1;
        } 
        else if ( s.find("IBM/") != string::npos )
        {
            format_type = 2;
        }
        else if ( s.find("Kosarak/") != string::npos )
        {
            format_type = 3;
        }
    }
    
    for ( mode = 0; mode < 3; mode++ )
    {
    
        CMSketch* cms = new CMSketch( epsilon, delta, mode );
        CMSketch* cms2 = new CMSketch( epsilon, delta, mode );
        RangeCMSketch* cmsRange = new RangeCMSketch( epsilon, delta, lines, mode );
        
        csvfile << ceil( 2/epsilon ) << ",";
        csvfile << ceil( -log2( 1-delta ) ) << ",";

        csvfile << lines << ",";

        // If quite a bit of memory is used ( i.e 100mb ) RangeCMSketch will run into
        // memory problems causing segmentation faults.

        // Normal CMSketch can run without problems even on high storage usage

        // get the items in the file
        // then check for overestimates

        if( format_type == 1 )
        {
            BMSAddItem( filename, cms, cms2, cmsRange);
            BMSEstimateItem( filename, cms, epsilon );
        } 
        else if ( format_type == 2 )
        {
            IBMAddItem( filename, cms, cms2, cmsRange);
            IBMEstimateItem( filename, cms, epsilon );
        } 
        else if ( format_type == 3 )
        {
            KosarakAddItem( filename, cms, cms2, cmsRange);
            KosarakEstimateItem( filename, cms, epsilon );
        }
        
        if ( mode == 0 )
            csvfile << "( a*x + b ) mod p, ";
        else if ( mode == 1)
            csvfile << "( ( a*x + b ) mod p ) mod w, " ;
        else if ( mode == 2)
            csvfile << "( ( ( a*x - b) + ( a*x - b) >> 32 ) & p ) mod w, " ;

        // Inner Product Query
        std::cout << std::fixed << std::setprecision(2);
        std::size_t cmsize = cms->return_byte_size();
        double storage = cmsize / 1048576;;
        // Display storage used in Megabytes
        if ( storage >= 1.0 )
        {

            //cout << "Storage used: " << storage <<"Mb." << endl;
            csvfile << storage << "Mb,"; 
        }
        // Display storage used in Kilobytes
        else
        {
            storage = cmsize / 1000;
            //cout << "Storage used: " << storage <<"Kb." << endl;
            csvfile << storage << "Kb,"; 
        }

        // Εrror related stats
        double mapsize = mapitems.size();
        double error_rate = ( mapsize / lines );

        csvfile << mapsize << ","; 
        csvfile << error_rate << ","; 
        
        // CMSketch actual input total count
        
        csvfile << cms->return_actual_count() << ",";
        // CMSketch total count of inputs
        // ( basically estimateRange from start to end )
        //cout << "Count-Min Sketch total count: " << 
        //cms.return_count() << endl;

        csvfile << total << ",";

        csvfile << cms->estimateInnerProduct( cms2->return_width(),
        cms2->return_depth(), cms2->return_table() ) << ",";

        // Range Query

        //csvfile << cmsRange->return_actual_count() << ",";
        csvfile << cmsRange->estimateRange( 0, lines ) << ",";

        // Quantile
        // If output is -1 then that number doesnt exist
        unsigned int quantile = cmsRange->findQuantile( 0.97 );
        csvfile << quantile << ",";

        // Heavy Hitters.
        // find the ones that surpass the given number.
        // i.e if the given number is 1000, find the items that appear more than 1000 times 
        
        int mincount = cmsRange->return_actual_count() * 0.4;
        int range_width = cmsRange->return_width()*cmsRange->return_depth();
        unsigned int* heavyhitters;
        heavyhitters = cmsRange->findHeavyHitters( cmsRange->return_depth(), 0, mincount );
        for ( i = 0; i < range_width; i++ )
        {
            if ( heavyhitters[i] != 0 )
                csvfile << heavyhitters[i] << " " ; 
        }

        csvfile << "\n";
        // reset all counts to zero
        memset(counts, 0, lines * sizeof(int));
        total = 0;
        mapitems.clear();

    }    

    free(counts);

}

int main(int argc,char* argv[])
{

    char filename[100];
    strcpy( filename, "Formats/Kosarak/SUSY.txt" );

    int i, j;
    csvfile.open( "Results/Kosarak/Susy.csv" );
    // Headers for output CSV file

    csvfile << "Width, " << "Depth," << "Items," << "Hash Function," << "Storage used, " 
            << "Errors ( estimates outside of e )," 
            << "Error Rate," << "Count-Min Sketch actual input total, " 
            << "Count-Min Sketch total count, " << "Inner Product, "
            << "Range Query, " << "Quantile, " << "Heavy Hitters " << '\n';
    
    OutputResults( filename, atof(argv[1]), atof(argv[2]) );

    csvfile << "\n";
    csvfile << "\n";
    csvfile.close();

    return 0;
    
}