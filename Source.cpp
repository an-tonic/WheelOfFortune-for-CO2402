//Serdyukov Antoniy G20982067
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <vector>

using namespace std;


//Util functions
// Returns a random number in the range 1 .. x
// Note that I am using casting to convert one data type to another
int Random(int x)
{
    return static_cast<int>(static_cast<double> (rand()) / (RAND_MAX)*x + 1);
}

//classes
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

using PlayerPair = pair<shared_ptr<CPlayer>, shared_ptr<CPlayer>>;


class CSlice {
public:
    int type;
    int amount;
    string name;

    CSlice(istringstream* stream) {
        *stream >> this->type >> this->amount >> this->name;
    }

    bool executeSliceActions(unique_ptr<CRound>* round, PlayerPair* players, weak_ptr<CPlayer>* currentPlayer) {

    }
};




class Game {
private:
    vector<unique_ptr<CSlice>> SlicesArray;
    vector<unique_ptr<CRound>> RoundsArray;
    shared_ptr<CPlayer> firstPlayer;
    shared_ptr<CPlayer> secondPlayer;
    PlayerPair players;

    weak_ptr<CPlayer> currentPlayer;
    int currentSliceIndex;
    int numberOfSlices;

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

    Game() {
        currentSliceIndex = 0;
        ReadFile<CSlice>("wheel.txt", &SlicesArray);
        ReadFile<CRound>("rounds.txt", &RoundsArray);
        numberOfSlices = SlicesArray.size();
        firstPlayer = make_shared<CPlayer>("John");
        secondPlayer = make_shared<CPlayer>("Maria");
        players = make_pair(firstPlayer, secondPlayer);
        currentPlayer = firstPlayer;

    }


    void SetNextPlayer() {
        currentPlayer = currentPlayer.lock() == firstPlayer ? secondPlayer : firstPlayer;
    }

    void StartGame() {
        cout << "Welcome to WheelOfFortune-ish" << endl;

        int roundIndex = 1;

        for (auto& round : RoundsArray) {
            cout << "Round " << roundIndex << ": " << round->word << endl;

            //Determine the first player for this round
            currentPlayer = (roundIndex % 2 == 0) ? secondPlayer : firstPlayer;
            int index = 0;
            cout << currentPlayer.lock()->name << endl;
            while (++index < 100) {

                //Rolling the next slice
                int rollNumber = Random(numberOfSlices);
                currentSliceIndex += rollNumber;
                //Wrapping around the wheel
                if (currentSliceIndex > numberOfSlices - 1) {
                    currentSliceIndex -= numberOfSlices;
                }
                cout << currentPlayer.lock()->name << " rolls " << rollNumber << endl;
                cout << currentPlayer.lock()->name << " rolls " << SlicesArray.at(currentSliceIndex)->name << endl;

                bool isNextPlayerTurn = SlicesArray.at(currentSliceIndex)->executeSliceActions(&round, &players, &currentPlayer);

                if (isNextPlayerTurn) {
                    SetNextPlayer();
                }
            }


            roundIndex++;
        }
    }
};

int main() {

    unique_ptr<Game> pGame = make_unique<Game>();
    pGame->StartGame();

    pGame.reset();


    _CrtDumpMemoryLeaks();

}