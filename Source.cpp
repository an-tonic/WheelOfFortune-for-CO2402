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
	string mWord;
	string mCopyOfWord;
	bool mIsFinished;
	string mAlphabet = "abcdefghijklmnopqrstuvwxyz";

	CRound(istringstream* stream) {

		*stream >> this->mWord;
		mCopyOfWord = mWord;
		mIsFinished = false;
	}

};

class CPlayer {
public:
	string mName;
	int mTotalBank;
	int mCurrentRoundBank;
	int mSecondChanceTokens;

	CPlayer(string newName) {
		mTotalBank = 0;
		mCurrentRoundBank = 0;
		mSecondChanceTokens = 0;
		mName = newName;
	}
};

class CSlice {
public:
	int mType;
	int mAmount;
	string mName;

	CSlice(string* line) {
		istringstream stream(*line);

		stream >> mType >> mAmount >> mName;
		if (stream.fail())
		{
			cerr << "Error: Failed to parse the string." << endl;
		}

	}

	/**
	* Prints which player rolls which slice and then executes the slice
	*
	* @return If the player should be switched to the other one
	*/
	bool ExecuteSlice(CRound& round, CPlayer& opponentPlayer, CPlayer& currentPlayer) {
		cout << currentPlayer.mName << " rolls " << this->mName << endl;
		return ExecuteSliceActions(round, opponentPlayer, currentPlayer);
	}


	virtual bool ExecuteSliceActions(CRound& round, CPlayer& opponentPlayer, CPlayer& currentPlayer) {
		return false;
	}

	/**
	* Counts occurencies of a letter in a string and deletes them. Also, finishes the round if the string is empty
	*
	* @return Number of letters deleted
	*/
	int RevealLetter(CRound& round, CPlayer& opponentPlayer, CPlayer& currentPlayer) {

		int chosenInt = Random(round.mAlphabet.length()) - 1;
		char chosenLetter = round.mAlphabet[chosenInt];
		round.mAlphabet.erase(chosenInt, 1);

		cout << currentPlayer.mName << " guesses " << chosenLetter << endl;
		//Need first to count letters and remove them (even the duplicates)
		int letterCount = count(round.mCopyOfWord.begin(), round.mCopyOfWord.end(), chosenLetter);
		round.mCopyOfWord.erase(remove(round.mCopyOfWord.begin(), round.mCopyOfWord.end(), chosenLetter), round.mCopyOfWord.end());

		cout << currentPlayer.mName << " reveals " << letterCount << " letter" << ((letterCount > 1) || letterCount == 0 ? "s" : "") << endl;

		//Ending the round
		if (round.mCopyOfWord.empty())
		{

			round.mIsFinished = true;
		}

		return letterCount;
	}

};

class CRegularSlice : public CSlice {
public:

	using CSlice::CSlice;
	/**
	* If letter is revealed puts the amount into current bank for this round
	*
	* @return If the player should be switched to the other one
	*/
	bool ExecuteSliceActions(CRound& round, CPlayer& opponentPlayer, CPlayer& currentPlayer) override {

		int letterCount = this->RevealLetter(round, opponentPlayer, currentPlayer);

		if (letterCount > 0)
		{
			int sliceBank = this->mAmount * letterCount;
			currentPlayer.mCurrentRoundBank += sliceBank;
			cout << currentPlayer.mName << " earns " << sliceBank << endl;
			return false;
		}

		cout << currentPlayer.mName << " loses turn due to inappropriate letter choice" << endl;
		return true;
	}
};

class CLoseTurnSlice : public CSlice {
public:
	using CSlice::CSlice;
	/**
	* Current player losses the turn
	*
	* @return The player must be switched to the other one
	*/
	bool ExecuteSliceActions(CRound& round, CPlayer& opponentPlayer, CPlayer& currentPlayer) override {

		cout << currentPlayer.mName << " loses turn" << endl;
		return true;
	}
};

class CStealSlice : public CSlice {
public:
	using CSlice::CSlice;
	/**
	* Halves opponent toatal bank, adds the same amount to the current player
	*
	* @return If the player should be switched to the other one
	*/
	bool ExecuteSliceActions(CRound& round, CPlayer& opponentPlayer, CPlayer& currentPlayer) override {

		int letterCount = this->RevealLetter(round, opponentPlayer, currentPlayer);

		if (letterCount > 0)
		{
			opponentPlayer.mTotalBank *= 0.5;
			currentPlayer.mTotalBank += opponentPlayer.mTotalBank;
			cout << currentPlayer.mName << " steals " << opponentPlayer.mTotalBank << " from " << opponentPlayer.mName << endl;
			return false;
		}
		return true;
	}
};

class CJackpotSlice : public CSlice {
public:
	using CSlice::CSlice;
	/**
	* Doubles current player's total money
	*
	* @return If the player should be switched to the other one
	*/
	bool ExecuteSliceActions(CRound& round, CPlayer& opponentPlayer, CPlayer& currentPlayer) override {

		int letterCount = this->RevealLetter(round, opponentPlayer, currentPlayer);

		if (letterCount > 0)
		{
			currentPlayer.mTotalBank *= 2;
			cout << currentPlayer.mName << " doubles banked money" << endl;
			return false;
		}
		return true;
	}
};

class CBankruptSlice : public CSlice {
public:
	using CSlice::CSlice;
	/**
	* If letter is revealed zeroes bank earning during this round and all tokens
	*
	* @return If the player should be switched to the other one
	*/
	bool ExecuteSliceActions(CRound& round, CPlayer& opponentPlayer, CPlayer& currentPlayer) override {

		currentPlayer.mCurrentRoundBank = 0;
		currentPlayer.mSecondChanceTokens = 0;
		return true;
	}
};

class CBankruptPlusSlice : public CBankruptSlice {
public:
	using CBankruptSlice::CBankruptSlice;
	/**
	* If letter is revealed zeroes bank earning during this round, total bank and all tokens
	*
	* @return If the player should be switched to the other one
	*/
	bool ExecuteSliceActions(CRound& round, CPlayer& opponentPlayer, CPlayer& currentPlayer) override {

		CBankruptSlice::ExecuteSliceActions(round, opponentPlayer, currentPlayer);


		currentPlayer.mTotalBank = 0;
		cout << currentPlayer.mName << " rolls Bankrupt+ and loses everything" << endl;
		return true;
	}
};

class CSecondChance : public CSlice {
public:
	using CSlice::CSlice;
	/**
	* If letter is revealed increases the token by one
	*
	* @return If the player should be switched to the other one
	*/
	bool ExecuteSliceActions(CRound& round, CPlayer& opponentPlayer, CPlayer& currentPlayer) override {

		int letterCount = this->RevealLetter(round, opponentPlayer, currentPlayer);

		if (letterCount > 0)
		{
			cout << "Number of SecondChance tokens earned: " << 1 << endl;
			currentPlayer.mSecondChanceTokens += 1;
			return false;
		}
		return true;
	}
};


class Game {
private:
	vector<unique_ptr<CSlice>> mSlicesArray;
	vector<unique_ptr<CRound>> mRoundsArray;
	shared_ptr<CPlayer> mpFirstPlayer;
	shared_ptr<CPlayer> mpSecondPlayer;

	weak_ptr<CPlayer> mpCurrentPlayer;
	weak_ptr<CPlayer> mpOpponentPlayer;
	int mCurrentSliceIndex;
	int mNumberOfSlices;

public:
	/**
	* Puts words that need to be played during rounds into a vector
	*/
	void ReadFileWithRounds(string filename, vector<unique_ptr<CRound>>& array) {
		ifstream inputFile(filename);

		if (!inputFile.is_open())
		{
			cerr << "Error opening the file" << endl;
			exit(1);
		}

		string line;

		// Read each line from the file
		while (getline(inputFile, line))
		{
			istringstream iss(line);

			array.emplace_back(make_unique<CRound>(&iss));
		}
		inputFile.close();
	}
	/**
	* Puts information about slice(type, name, amount) into a vector 
	*/
	void ReadFileWithSlices(string filename, vector<unique_ptr<CSlice>>& array) {
		ifstream inputFile(filename);

		if (!inputFile.is_open())
		{
			cerr << "Error opening the file" << endl;
			exit(1);
		}

		string line;

		// Read each line from the file
		while (getline(inputFile, line))
		{

			if (line[0] == '1')
			{
				array.emplace_back(make_unique<CRegularSlice>(&line));
			} else if (line[0] == '2')
			{
				array.emplace_back(make_unique<CLoseTurnSlice>(&line));
			} else if (line[0] == '3')
			{
				array.emplace_back(make_unique<CBankruptSlice>(&line));
			} else if (line[0] == '4')
			{
				array.emplace_back(make_unique<CBankruptPlusSlice>(&line));
			} else if (line[0] == '5')
			{
				array.emplace_back(make_unique<CSecondChance>(&line));
			} else if (line[0] == '6')
			{
				array.emplace_back(make_unique<CStealSlice>(&line));
			} else if (line[0] == '7')
			{
				array.emplace_back(make_unique<CJackpotSlice>(&line));
			} else
			{
				cerr << "Could not create a slice";
				exit(1);
			}

		}

		inputFile.close();
	}
	/**
	* Reads a seed from file, and seeds the random function with it
	*/
	void ReadFileWithSeed(string filename) {
		ifstream inputFile(filename);

		if (!inputFile.is_open())
		{
			cerr << "Error opening the file" << endl;
			exit(1);
		}

		string line;

		while (getline(inputFile, line))
		{
			int seed = stoi(line);
			srand(seed);
		}
		inputFile.close();
	}

	Game() {
		mCurrentSliceIndex = 0;
		ReadFileWithSlices("wheel.txt", mSlicesArray);
		ReadFileWithRounds("rounds.txt", mRoundsArray);
		ReadFileWithSeed("seed.txt");
		mNumberOfSlices = mSlicesArray.size();
		mpFirstPlayer = make_shared<CPlayer>("John");
		mpSecondPlayer = make_shared<CPlayer>("Maria");
		mpCurrentPlayer = mpFirstPlayer;
		mpOpponentPlayer = mpSecondPlayer;
	}
	/**
	* Swaps players with each other
	*/
	void SetNextPlayer() {

		mpCurrentPlayer = (mpCurrentPlayer.lock() == mpFirstPlayer) ? mpSecondPlayer : mpFirstPlayer;
		mpOpponentPlayer = (mpCurrentPlayer.lock() == mpFirstPlayer) ? mpSecondPlayer : mpFirstPlayer;

	}

	/**
	* Starts the whole game
	*/
	void StartGame() {
		cout << "Welcome to WheelOfFortune-ish" << endl;

		int roundIndex = 1;

		for (auto& round : mRoundsArray)
		{
			cout << "Round " << roundIndex << ": " << round->mWord << endl;

			//Determine the first player for this round
			mpCurrentPlayer = (roundIndex % 2 == 0) ? mpSecondPlayer : mpFirstPlayer;
			int index = 0;

			while (!round.get()->mIsFinished)
			{

				//Rolling the next slice
				int rollNumber = Random(mNumberOfSlices);

				mCurrentSliceIndex = (mCurrentSliceIndex + rollNumber) % mNumberOfSlices;

				cout << mpCurrentPlayer.lock()->mName << " rolls " << rollNumber << endl;

				bool isNextPlayerTurn = mSlicesArray.at(mCurrentSliceIndex)->ExecuteSlice(*round, *mpOpponentPlayer.lock(), *mpCurrentPlayer.lock());

				if (mpCurrentPlayer.lock()->mSecondChanceTokens > 0 && isNextPlayerTurn)
				{
					isNextPlayerTurn = false;
					mpCurrentPlayer.lock()->mSecondChanceTokens--;
					cout << mpCurrentPlayer.lock()->mName << " uses SecondChance token" << endl;
					cout << "Remaining SecondChance tokens: " << mpCurrentPlayer.lock()->mSecondChanceTokens << endl;

				}

				if (isNextPlayerTurn)
				{
					SetNextPlayer();
				}

			}
			cout << "Game Over" << endl;;
			cout << mpCurrentPlayer.lock()->mName << " wins the round and banks " << mpCurrentPlayer.lock()->mCurrentRoundBank << endl;

			mpCurrentPlayer.lock()->mTotalBank += mpCurrentPlayer.lock()->mCurrentRoundBank;
			mpOpponentPlayer.lock()->mTotalBank += mpOpponentPlayer.lock()->mCurrentRoundBank;
			cout << mpCurrentPlayer.lock()->mName << "'s total banked amount is " << mpCurrentPlayer.lock()->mTotalBank << endl;
			cout << mpOpponentPlayer.lock()->mName << "'s total banked amount is " << mpOpponentPlayer.lock()->mTotalBank << endl;
			mpCurrentPlayer.lock()->mCurrentRoundBank = 0;
			mpOpponentPlayer.lock()->mCurrentRoundBank = 0;

			roundIndex++;
		}
	}
};



int main() {


	unique_ptr<Game> pGame = make_unique<Game>();
	pGame->StartGame();

	//Needs to be here, otherwise the pGame pointer does not go out of scope and leaks memory
	pGame.reset();


	_CrtDumpMemoryLeaks();




}