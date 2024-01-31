//Serdyukov Antoniy G20982067

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <vector>

using namespace std;


//classes

class CSlice {
public:
	int type;
	int amount;
	string name;

    CSlice(istringstream* stream) {
        *stream >> this->type >> this->amount >> this->name;
    }
};

class CRound {
public:
    string word;

    CRound(istringstream* stream) {
     
        *stream >> this->word;
        
    }
};


class CPlayer {
public:
    string name;

    CPlayer(string newName) {
        this->name = newName;
    }
};

// Returns a random number in the range 1 .. x
// Note that I am using casting to convert one data type to another
int Random(int x)
{
    return static_cast<int>(static_cast<double> (rand()) / (RAND_MAX)*x + 1);
}

template <typename T>
int ReadFile(string filename, vector<T>* array) {
    ifstream inputFile(filename);
    
    if (!inputFile.is_open()) {
        cerr << "Error opening the file" << endl;
        return 1;
    }

    
    string line;

    // Read each line from the file
    while (getline(inputFile, line)) {
        istringstream iss(line);
        
        T object(&iss);  
        array->push_back(object);            
          
    }

    inputFile.close();


    return 0;

}


int main() {

    vector<CSlice> SliceArray;

	ReadFile<CSlice>("wheel.txt", &SliceArray);

    vector<CRound> RoundArray;

    ReadFile<CRound>("rounds.txt", &RoundArray);

    CPlayer firstPlayer("John");
    CPlayer firstPlayer("Maria");


    cout << "Welcome to WheelOfFortune-ish";

    while (true) {



    }


}