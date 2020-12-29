#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <math.h>
#include <string>
#include <fstream>
using namespace std;

// Life command programmed by Robert Brodin for CS3013, Professor Wills, A Term 2020.

#define RANGE 1
#define ALLDONE 2
#define GO 3
#define GENDONE 4
#define STOPGEN 6

struct msg {
	int iSender;
	int type;
	int value1;
	int value2;
};

struct mailbox {
	msg message;
};

struct addemStruct {

	int threadVectorIndex;
	int total;
	int lowerBound;
	int upperBound;
};
// mailboxVector is used to store all of the mailboxs
vector<mailbox> mailboxVector;
// semSendMailVector is used to store producer semaphores in a vector
vector<sem_t> semSendMailVector;
// semReceiveMailVector is used to store consumer 
vector<sem_t> semReceiveMailVector;
// threadVector holds all of the initialized threads
vector<pthread_t> threadVector;

// Sends a message to a specified mailbox once the semaphore is available (ie empty and not full). After the message is sent, the consumer semaphore is notified.
void SendMsg(int iTo, struct msg* pMsg){

	sem_wait(&semSendMailVector[iTo]);
	mailboxVector[iTo].message = *pMsg;
	sem_post(&semReceiveMailVector[iTo]);
}

// Receive a message from a specified mailbox once the semaphore is available (ie full and not empty). After the message is received, the producer semaphore is notified.
void RecvMsg(int iTo, struct msg* pMsg){

	sem_wait(&semReceiveMailVector[iTo]);
	*pMsg = mailboxVector[iTo].message;
	sem_post(&semSendMailVector[iTo]);
}

// Function takes a vector, returns void, and simply prints out the board.
void printBoard(vector<vector<int>> board, int xSize, int ySize){
	for(int y = 0; y < ySize; y++){
		cout << "\n";
		for(int x = 0; x < xSize; x++){
			cout << board[y][x] << " ";
		}
	}
}

// class lifeBlockInformation is used to store the current game board. I couldn't figure out a good way to do this with message passing and converting a class to a void pointer, so this was my solution.
class lifeBlockInformation{
	public:
	// Contains the current gameboard in a generation
	vector<vector<int>> board;
	// Contains the next generation gameboard
	vector<vector<int>> newBoard;
	// Contains the last generation's gameboard (used to check for a difference between generations)
	vector<vector<int>> lastBoard;
	// Contains an empty gameboard (used to check if the current gameboard is empty)
	vector<vector<int>> emptyBoard;
	// Lower and upper are the lower and upper boundaries of the range of rows to be played.
	int lower;
	int upper;
	// xSize and ySize are the dimensions of the gameboard
	int xSize;
	int ySize;
	// Function to set the current gameboard by reference.
	void setBoard(vector<vector<int>>& boardA){
		board = boardA;
	}
};

// IMPORTANT: lifeB is a global variable used to keep track of the current gameboard
lifeBlockInformation lifeB;

// void* Function to be used to play a given number of generations (determined by the message the function receives), and is passed a void* args containing the ID of the mailbox the function should send and receive messages from.
void* lifeBlockVoid(void* args){

msg n;
int* id = (int*) args;

int type = 0;
// Checking if the type of the message is RANGE.
while(type != GO){
	RecvMsg(*id, &n);
	type = n.type;
}

// Setting the gameboard up as well as the new board, to be used to play the game of life.
vector<vector<int>> board = lifeB.board;
vector<vector<int>> newBoard = lifeB.newBoard;
// lower and upper row range is received from the message and set equal to lower and upper.
int lower = n.value1;
int upper = n.value2;
int xSize = lifeB.xSize;
int ySize = lifeB.ySize;

int count = 0;
	// Algorithm for playing the game of life. The algorithm doesn't really need to be explained super in depth, just that there is a way of looping through the rows cleverly...
	// ... by checking if the cell is at the edge of the board or in the middle.
	// The function counts the number of adjacent cells, and then moves forward and creates the new generation vector.
	// Before the message is sent back to the parent, make sure to check if the new board is empty, if it is, send the STOP message back to the parent, otherwise...
	// ...A message is sent back to parent that the GENDONE.
	for(int a = lower - 1; a <= upper - 1; a++){

	int yLoopLower = -1;
	int yLoopUpper = 1;
	// These say: if the row is 0, don't index out of bounds, and if the row is the max, don't index out of bounds
	if(a == 0){
		yLoopLower = 0;
		//cout << "\nRow is 0";
	}
	else if(a == (ySize - 1)){
		yLoopUpper = 0;
		//cout << "\nRow is max!";
	}

	for(int b = 0; b <= xSize - 1; b++){
		int xLoopLower = -1;
		int xLoopUpper = 1;
		//cout << "\nRan here!!";
		if(b == 0){
			xLoopLower = 0;
			//cout << "\nCol is 0";
		}
		else if(b == (xSize - 1)){
			xLoopUpper = 0;
			//cout << "\nCol is max!";
		}	
		int cellsAround = 0;
		for(int aLoop = yLoopLower; aLoop <= yLoopUpper; aLoop++){
			//cout << "\nRunning in here";
			for(int bLoop = xLoopLower; bLoop <= xLoopUpper; bLoop++){
				// If the cell value is 1, add one to cellsAround
				//cout << "\nAlso running in here" << " a: " << aLoop + a << " b: " << b + bLoop << " " << board[a + aLoop][b + bLoop];
				//cellsAround = board[a + aLoop][b + bLoop]
				
				if(board[a + aLoop][b + bLoop] == 1){
					cellsAround++;
					//cout << "\nMade it in";
				}

			}
		}
		// If the current cell being checked exists, make sure that it isn't included in the cells around count. 
		if(board[a][b] && cellsAround > 0){
			//cout << "Ran here";
			cellsAround--;
		}
		

		// If the cell already is dead and has three cells around, keep it alive.
		if((board[a][b] == 0) && (cellsAround == 3)){
			//cout << "\nAdded a living being!";
			newBoard[a][b] = 1;
		}
		// Otherwise, it's dead.
		else if ((board[a][b] == 0) && (cellsAround != 3)){
			newBoard[a][b] = 0;
		}
		// If the cell is living, and the number of cells around are two or three, keep it alive.
		else if((board[a][b] == 1) && (cellsAround == 2 || cellsAround == 3)){
			//cout << "\nhit this else";
			newBoard[a][b] = 1;
		}
		// Otherwise, it's dead.
		else{
			newBoard[a][b] = 0;
		}
	}
}
bool empty = true;
// Check if the rows played in this thread are empty
for(int x = lower - 1; x <= upper - 1; x++){
	if(lifeB.emptyBoard[0] != newBoard[x]){
		empty = false;
	}
}
// update lifeB.board with the new values calculated.
lifeB.board = board;
lifeB.newBoard = newBoard;
msg m;
m.type = GENDONE;
if(empty){
	m.type = STOPGEN;
}
SendMsg(0, &m);
}

main(int argc, char* argv[]){

// Check if four arguments are given.
if(argc < 4){
	cout << "\nNot enough arguments given! Please give more than " << (argc - 1) << " arguments!\n";
	exit(1);
}

string fileName(argv[2]);

ifstream file(fileName);
// Check if the given file name exists.
if(!file){
	cout << "\nFile does not exist!";
	exit(1);
}

// Read from the text file, and keep track of the dimensions of the file and produce a 2d vector from the file.
string str;
vector<vector<int>> gB;
int outsideCounter = 0;
int insideCounter = 0;
while(getline(file, str)){
	//cout << "\nLine: " << str;
	insideCounter = 0;
	outsideCounter++;
	vector<int> t;
	for(int x = 0; x < str.size(); x++){
		//cout << "\nline: " << str[x];
		if(str[x] == '1' or str[x] == '0'){
			insideCounter++;
			if(str[x] == '1'){
				t.push_back(1);
			}
			else{
				t.push_back(0);
			}
		}
	}
	gB.push_back(t);
}
// Making a vector to try to use for the gameboard, instead of a 2d array.
vector<vector<int>> gameBoardVector;
gameBoardVector = gB;
// Update xSize and ySize (the board dimensions), with the dimensions calculated by reading text file.
int ySize = outsideCounter;
int xSize = insideCounter;

// Get the number of threads and number of generations
int numberOfThreads = stoi(argv[1]);
int numberOfGenerations = stoi(argv[3]);
bool printBoardVariable = false;

// Check if we should print our each board (default is no)
if(argc >= 5){
	//cout << "\nChecking here";
	if(argv[4] == string("y")){
		printBoardVariable = true;
	}
}
// Check if we should wait for inout (default is no)
bool waitForInput = false;
if(argc == 6){
	if(argv[5] == string("y")){
		waitForInput = true;
	}
}

// Code below is used to divide the dimensions of the boards by the number of threads, so there are an equal amount of rows for each thread to play.
int boardBlock = floor(ySize / numberOfThreads);
int blockRem = ySize % numberOfThreads;
if(boardBlock == 0){	
	numberOfThreads = ySize;
	boardBlock = 1;
	blockRem = 0;
}
int initialBlock = boardBlock - 1;
int currentTotal = 1;

vector<msg> mV;

// For each number in range 0 to numberOfThreads + 1, create a producer semaphore, consumer semaphore, and thread.
for(int i = 0; i < numberOfThreads + 1; i++){
	mailbox temp;
	mailboxVector.push_back(temp);

	// Init each producer semaphore as 1 and each consumer as 0
	sem_t tempSemSend;
	sem_t tempSemReceive;
	sem_init(&tempSemSend, 0, 1);
	sem_init(&tempSemReceive, 0, 0);
	semSendMailVector.push_back(tempSemSend);
	semReceiveMailVector.push_back(tempSemReceive);
	pthread_t tempThread;
	threadVector.push_back(tempThread);
}

// Loop below iterates through the number of threads and creates a range of numbers stored in a message (value1, value2), for the voidBlock function.
for(int j = 1; j < numberOfThreads + 1; j++){
	struct msg m;
	m.iSender = 0;
	m.type = GO; 
	m.value1 = currentTotal;
	m.value2 = currentTotal + boardBlock - 1;
	//cout << "\n" << currentTotal << " " << boardBlock << " " << blockRem;

	// If we are at the last index, remember to add the remainder!
	if(j == numberOfThreads){
		m.value2 = currentTotal + boardBlock + blockRem - 1;
	}
	//cout << "\nRow " << m.value1 << " to " << m.value2;
	
	currentTotal = currentTotal + boardBlock;
	//cout << "\nThe current total is: " << currentTotal;
	mV.push_back(m);
}


int gen = 1;
// Print out generation zero
cout << "\nGeneration 0";
printBoard(gameBoardVector, xSize, ySize);

// Loop through the number of given generations
for(gen = 1; gen <= numberOfGenerations; gen++){
vector<vector<int>> printBoardVector;

// Create an empty board to push to.
vector<vector<int>> newBoardVector;
for(int i = 0; i < ySize; i++){
	vector<int> tV;
	for(int j = 0; j < xSize; j++){
		tV.push_back(0);
		//cout << "\n" << t[j];
	}
	// TODO: Have to change this to game board vector
	newBoardVector.push_back(tV);
}
lifeB.emptyBoard = newBoardVector;

lifeB.board = gameBoardVector;
lifeB.newBoard = newBoardVector;

semSendMailVector.clear();
semReceiveMailVector.clear();
threadVector.clear();

// Again, create producer and consumer semaphores for each loop, they are deleted every time and recreated, this was the only way I could get my program to work.
for(int i = 0; i < numberOfThreads + 1; i++){
	mailbox temp;
	mailboxVector.push_back(temp);

	// Init each producer semaphore as 1 and each consumer as 0
	sem_t tempSemSend;
	sem_t tempSemReceive;
	sem_init(&tempSemSend, 0, 1);
	sem_init(&tempSemReceive, 0, 0);
	semSendMailVector.push_back(tempSemSend);
	semReceiveMailVector.push_back(tempSemReceive);
	pthread_t tempThread;
	threadVector.push_back(tempThread);
}

// Create all of the threads.
for(int k = 1; k < numberOfThreads + 1; k++){
	int* num = (int*) malloc(sizeof(int));
	*num = k;
	if(pthread_create(&(threadVector[k]), NULL, lifeBlockVoid, (void*) num) != 0){
		perror("pthread_create");
		exit(1);
	}
}
int zeroCount = 0;
for(int t = 0; t < numberOfThreads; t++){
	lifeB.xSize = xSize;
	lifeB.ySize = ySize;
	//lifeB.newBoard = newBoardVector;

	SendMsg(t + 1, &mV[t]);
	msg z;
	int type = 0;
	// Checking if the type of the message is RANGE.
	while(type != GENDONE){
		RecvMsg(0, &z);
		type = z.type;
		if(type == STOPGEN){

			type = GENDONE;
			zeroCount++;
		}
	}

} // thread loop ends here
if(zeroCount == numberOfThreads){
	cout << "\nThe board is empty!";
	exit(1);
}
if(printBoardVariable && gen != numberOfGenerations){
cout << "\n\nGeneration " << gen;
printBoard(lifeB.newBoard, xSize, ySize);
}
//printBoard(newBoardVector, xSize, ySize);
//gameBoard = newBoard;
// Making it so we can do multiple generations easily.

gameBoardVector = lifeB.newBoard;
lifeB.lastBoard = lifeB.board;
lifeB.board = lifeB.newBoard;
if(gen > 1){
	if(lifeB.lastBoard == lifeB.board){
		cout << "\nThe game hasn't changed from one generation to another! Exiting!\n";
		exit(1);
	}
}
newBoardVector.clear();
//memmove(gameBoardVector, newBoardVector, sizeof(gameBoardVector));
for(int n = 1; n < numberOfThreads + 1; n++){
      (void)pthread_join(threadVector[n], NULL);
}

if(waitForInput && gen != numberOfGenerations){
cout << "\nHit enter to continue!";
cin.get();

}

}
cout << "\nThe game ended after " << (gen - 1) << " generations!";
printBoard(lifeB.newBoard, xSize, ySize);
cout << "\n";

// Deleting semaphores as they are not needed.
for(int x = 0; x < semSendMailVector.size(); x++){
	(void)sem_destroy(&semSendMailVector[x]);
}
for(int y = 0; y < semReceiveMailVector.size(); y++){
	(void)sem_destroy(&semReceiveMailVector[y]);
}




return 0;
}






