#include <iostream>
#include <ctime>
using namespace std;

// Returns a random number in the range 1 .. x
// Note that I am using casting to convert one data type to another
int Random(int x)
{
	return static_cast<int>( static_cast<double> (rand()) / (RAND_MAX) * x + 1 );
}


int main()
{
	// A random number generator actually produces a pseudo-random sequence of numbers.
	// This means that the random number generator will always produce the same sequence of numbers.
	// The generator needs to be "seeded" with a value. You seed the generator with the function srand().
    srand( 10 );
    for( int i = 0; i < 40; i++ )
	{
		cout << Random(4) << endl;
	}
	cout << endl;
}
