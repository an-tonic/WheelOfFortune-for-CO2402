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
};

int ReadFile(string filename, vector<CSlice>* array) {
    ifstream inputFile("wheel.txt");
    
    if (!inputFile.is_open()) {
        cerr << "Error opening the file" << endl;
        return 1;
    }

    
    string line;

    // Read each line from the file
    while (getline(inputFile, line)) {
        istringstream iss(line);
        CSlice slice;

        if (iss >> slice.type >> slice.amount >> slice.name) {
            array->push_back(slice);
        }
        else {
            cerr << "Error reading line: " << line << endl;
        }
    }

    inputFile.close();


    return 0;

}


int main() {

    vector<CSlice> SliceArray;

	ReadFile("wheel.txt", &SliceArray);

    cout;
}