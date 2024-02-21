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
// Returns a random number in the range 0 .. x
// Note that I am using casting to convert one data type to another
int Random(int x) {
	
	return static_cast<int>(static_cast<double> (rand()) / (RAND_MAX)*x + 1);
}

//classes
class CRound {
public:
	string word;
	string copyOfWord;
	bool isFinished;

	string alphbet = "abcdefghijklmnopqrstuvwxyz";

	CRound(istringstream* stream) {

		*stream >> this->word;
		copyOfWord = word;
		isFinished = false;
	}

};

class CPlayer {
public:
	string name;
	int totalBank;
	int currentRoundBank;
	int secondChanceTokens;

	CPlayer(string newName) {
		totalBank = 0;
		currentRoundBank = 0;
		secondChanceTokens = 0;
		name = newName;
	}
};

class CSlice {
public:
	int type;
	int amount;
	string name;

	CSlice(string* line) {
		istringstream stream(*line);
		
		stream >> type >> amount >> name;
		if (stream.fail()) {
			cerr << "Error: Failed to parse the string." << endl;
			
		}
		
	}
	bool executeSlice(CRound& round, CPlayer& opponentPlayer, CPlayer& currentPlayer) {
		cout << currentPlayer.name << " rolls " << this->name << endl;
		return executeSliceActions(round, opponentPlayer, currentPlayer);
	}
	

	virtual bool executeSliceActions(CRound& round,  CPlayer& opponentPlayer, CPlayer& currentPlayer) {
		return false;
	}

	int revealLetter(CRound& round, CPlayer& opponentPlayer, CPlayer& currentPlayer) {

		int chosenInt = Random(round.alphbet.length()) - 1;
		char chosenLetter = round.alphbet[chosenInt];
		round.alphbet.erase(chosenInt, 1);

		int letterCount = 0;
		cout << currentPlayer.name << " guesses " << chosenLetter << endl;

		for (int i = 0; i < round.copyOfWord.length(); i++) {
			if (round.copyOfWord.at(i) == chosenLetter) {
				round.copyOfWord.erase(i, 1);

				i--; //If the same letter is consecutive need to return to the same index
				letterCount++;
			}
		}
		cout << currentPlayer.name << " reveals " << letterCount << " letter" << ((letterCount > 1) || letterCount == 0 ? "s" : "") << endl;

		//Ending the round
		if (round.copyOfWord.length() == 0) {

			round.isFinished = true;
		}

		return letterCount;
	}

};

class CRegularSlice : public CSlice {
public:

	using CSlice::CSlice;

	bool executeSliceActions(CRound& round,  CPlayer& opponentPlayer, CPlayer& currentPlayer) override {

		int letterCount = this->revealLetter(round, opponentPlayer, currentPlayer);
	
		if (letterCount > 0) {
			int sliceBank = this->amount * letterCount;
			currentPlayer.currentRoundBank += sliceBank;
			cout << currentPlayer.name << " earns " << sliceBank << endl;
			return false;
		}

		cout << currentPlayer.name << " loses turn due to inappropriate letter choice" << endl;
		return true;
	}
};

class CLoseTurnSlice : public CSlice {
public:
	using CSlice::CSlice;

	bool executeSliceActions(CRound& round,  CPlayer& opponentPlayer, CPlayer& currentPlayer) override {
			
		cout << currentPlayer.name << " loses turn" << endl;
		return true;
	}
};

class CStealSlice : public CSlice {
public:
	using CSlice::CSlice;

	bool executeSliceActions(CRound& round, CPlayer& opponentPlayer, CPlayer& currentPlayer) override {

		int letterCount = this->revealLetter(round, opponentPlayer, currentPlayer);

		if (letterCount > 0) {
			opponentPlayer.totalBank *= 0.5;
			currentPlayer.totalBank += opponentPlayer.totalBank;
			cout << currentPlayer.name << " steals " << opponentPlayer.totalBank << " from " << opponentPlayer.name << endl;
			return false;
		}
		return true;
	}
};

class CJackpotSlice : public CSlice {
public:
	using CSlice::CSlice;

	bool executeSliceActions(CRound& round, CPlayer& opponentPlayer, CPlayer& currentPlayer) override {

		int letterCount = this->revealLetter(round, opponentPlayer, currentPlayer);

		if (letterCount > 0) {
			
			currentPlayer.totalBank *= 2;
			cout << currentPlayer.name << " doubles banked money" << endl;
			return false;
		}
		return true;
	}
};

class CBankruptSlice : public CSlice {
public:
	using CSlice::CSlice;

	bool executeSliceActions(CRound& round, CPlayer& opponentPlayer, CPlayer& currentPlayer) override {

		currentPlayer.currentRoundBank = 0;
		currentPlayer.secondChanceTokens = 0;
		return true;
	}
};

class CBankruptPlusSlice : public CBankruptSlice {
public:
	using CBankruptSlice::CBankruptSlice;

	bool executeSliceActions(CRound& round, CPlayer& opponentPlayer, CPlayer& currentPlayer) override {
		
		CBankruptSlice::executeSliceActions(round, opponentPlayer, currentPlayer);

		
		currentPlayer.totalBank = 0;  
		cout << currentPlayer.name << " rolls Bankrupt+ and loses everything" << endl;
		return true;
	}
};

class CSecondChance : public CSlice {
public:
	using CSlice::CSlice;

	bool executeSliceActions(CRound& round, CPlayer& opponentPlayer, CPlayer& currentPlayer) override {

		int letterCount = this->revealLetter(round, opponentPlayer, currentPlayer);

		if (letterCount > 0) {
			currentPlayer.secondChanceTokens += 1;
			return false;
		}
		return true;
	}
};


class Game {
private:
	vector<unique_ptr<CSlice>> SlicesArray;
	vector<unique_ptr<CRound>> RoundsArray;
	shared_ptr<CPlayer> firstPlayer;
	shared_ptr<CPlayer> secondPlayer;

	weak_ptr<CPlayer> currentPlayer;
	weak_ptr<CPlayer> opponentPlayer;
	int currentSliceIndex;
	int numberOfSlices;

public:

	int ReadFileWithRounds(string filename, vector<unique_ptr<CRound>>& array) {
		ifstream inputFile(filename);

		if (!inputFile.is_open()) {
			cerr << "Error opening the file" << endl;
			return 1;
		}

		string line;

		// Read each line from the file
		while (getline(inputFile, line)) {
			istringstream iss(line);

			array.push_back(make_unique<CRound>(&iss));
		}
		inputFile.close();
		return 0;
	}

	int ReadFileWithSlices(string filename, vector<unique_ptr<CSlice>>& array) {
		ifstream inputFile(filename);

		if (!inputFile.is_open()) {
			cerr << "Error opening the file" << endl;
			return 1;
		}

		string line;

		// Read each line from the file
		while (getline(inputFile, line)) {		
			
			if (line[0] == '1') {
				array.push_back(make_unique<CRegularSlice>(&line));
			} else if (line[0] == '2') {
				array.push_back(make_unique<CLoseTurnSlice>(&line));
			} else if (line[0] == '3') {
				array.push_back(make_unique<CBankruptSlice>(&line));
			} else if (line[0] == '4') {
				array.push_back(make_unique<CBankruptPlusSlice>(&line));
			} else if (line[0] == '5') {
				array.push_back(make_unique<CSecondChance>(&line));
			} else if (line[0] == '6') {
				array.push_back(make_unique<CStealSlice>(&line));
			} else if (line[0] == '7') {
				array.push_back(make_unique<CJackpotSlice>(&line));
			}
			else {
				cerr << "Could not create a slice";
				exit(1);
			}

		}

		inputFile.close();
		return 0;
	}

	Game() {
		currentSliceIndex = 0;
		ReadFileWithSlices("wheel.txt", SlicesArray);
		ReadFileWithRounds("rounds.txt", RoundsArray);
		numberOfSlices = SlicesArray.size();
		firstPlayer = make_shared<CPlayer>("John");
		secondPlayer = make_shared<CPlayer>("Maria");
		currentPlayer = firstPlayer;
		opponentPlayer = secondPlayer;
	}

	void SetNextPlayer() {
		
		currentPlayer = (currentPlayer.lock() == firstPlayer) ? secondPlayer : firstPlayer;
		opponentPlayer = (currentPlayer.lock() == firstPlayer) ? secondPlayer : firstPlayer;

	}

	void StartGame() {
		cout << "Welcome to WheelOfFortune-ish" << endl;

		int roundIndex = 1;

		for (auto& round : RoundsArray) {
			cout << "Round " << roundIndex << ": " << round->word << endl;

			//Determine the first player for this round
			currentPlayer = (roundIndex % 2 == 0) ? secondPlayer : firstPlayer;
			int index = 0;
			
			while (!round.get()->isFinished) {

				//Rolling the next slice
				int rollNumber = Random(numberOfSlices);
				currentSliceIndex += rollNumber;
				//Wrapping around the wheel
				if (currentSliceIndex > numberOfSlices - 1) {
					currentSliceIndex -= numberOfSlices;
				}
				cout << currentPlayer.lock()->name << " rolls " << rollNumber << endl;
				

				bool isNextPlayerTurn = SlicesArray.at(currentSliceIndex)->executeSlice(*round, *opponentPlayer.lock(), *currentPlayer.lock());
				
				
				if (isNextPlayerTurn) {
					SetNextPlayer();
				}

			}
			cout << "Game Over" << endl;;
			cout << currentPlayer.lock()->name << " wins the round and banks " << currentPlayer.lock()->currentRoundBank << endl;

			currentPlayer.lock()->totalBank += currentPlayer.lock()->currentRoundBank;
			opponentPlayer.lock()->totalBank += opponentPlayer.lock()->currentRoundBank;
			cout << currentPlayer.lock()->name << "'s total banked amount is " << currentPlayer.lock()->totalBank << endl;
			cout << opponentPlayer.lock()->name << "'s total banked amount is " << opponentPlayer.lock()->totalBank << endl;
			currentPlayer.lock()->currentRoundBank = 0;
			opponentPlayer.lock()->currentRoundBank = 0;

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