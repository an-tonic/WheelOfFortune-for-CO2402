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



class Game {
private:
    vector<unique_ptr<CSlice>> SlicesArray; 
    vector<unique_ptr<CRound>> RoundsArray;
    shared_ptr<CPlayer> firstPlayer;
    shared_ptr<CPlayer> secondPlayer;
    weak_ptr<CPlayer> currentPlayer;
    int currentSlice;

public:

    template <typename T>
    int ReadFile(string filename, vector<unique_ptr<T>>* array) {
        ifstream inputFile(filename);

        if (!inputFile.is_open()) {
            cerr << "Error opening the file" << endl;
            return 1;
        }


        string line;

        // Read each line from the file
        while (getline(inputFile, line)) {
            istringstream iss(line);

            /*T object(&iss);*/
            array->push_back(make_unique<T>(&iss));

        }

        inputFile.close();


        return 0;

    }

    Game(){
        //ToDo
        currentSlice = 0;
        ReadFile<CSlice>("wheel.txt", &SlicesArray);
        ReadFile<CRound>("rounds.txt", &RoundsArray);
        firstPlayer =  make_shared<CPlayer>("John");
        secondPlayer = make_shared<CPlayer>("Maria");
        currentPlayer = firstPlayer;
        
    }

    void SetNextPlayer() {
        currentPlayer = currentPlayer.lock() == firstPlayer ? secondPlayer : firstPlayer;
    }

    void StartGame() {
        cout << "Welcome to WheelOfFortune-ish" << endl;

        int roundIndex = 1; 

        for (auto& round : RoundsArray) {
            cout << "Round " << roundIndex++ << ": " << round->word << endl;

            cout << currentPlayer.lock()->name << endl;
            SetNextPlayer();

        }
    }
};

int main() {

    
    
    auto pGame = make_unique<Game>();
    
    pGame->StartGame();

    

}