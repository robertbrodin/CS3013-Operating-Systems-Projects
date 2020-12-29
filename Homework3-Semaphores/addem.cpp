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
using namespace std;

// Addem command programmed by Robert Brodin for CS3013, Professor Wills, A Term 2020.
#define RANGE 1
#define ALLDONE 2

struct msg {
	int iSender;
	int type;
	int value1;
	int value2;
};

struct mailbox {
	msg message;
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

// addem takes an void*, which SHOULD BE an int in this case, which is the ID of the mailbox that the function should look for mail in.
void* addem(void* args){
	// void* arg is a struct containing a message, val 1 is the lower bound, val 2 is the upper bound
	int* id = (int*) args;
	msg n;

	int type = 0;
	// Checking if the type of the message is RANGE.
	while(type != RANGE){
	// Wait on a message from the parent
		RecvMsg(*id, &n);
		type = n.type;
	}

	// Sum the values in the given range.
	int t = 0;
	for(int k = n.value1; k <= n.value2; k++){
		t = t + k;
	}

	msg* messageToParent = (struct msg*) malloc(sizeof(struct msg));
	messageToParent->iSender = *(id);
	messageToParent->type = ALLDONE;
	// Pass the total into value1 of the message
	messageToParent->value1 = t;
	// Finally, send the total back to the parent thread
	SendMsg(0, messageToParent);
}


main(int argc, char* argv[]){

// Check if there are less than two arguments, if so, exit.
cout << "\nArg:" << argv[1];
if(argc < 3){
	cout << "\nPlease specify two arguments!\n";
	exit(1);
}


// Read the number of threads and check to make sure they don't exceed ten! Exit if so.
int numberOfThreads = stoi(argv[1]);
int maxThread = 10;
if(numberOfThreads > maxThread){
	cout << "\nThe maximum number of threads is: " << maxThread << ", you tried to use " << numberOfThreads << "!\n";
	exit(1);
}

// Code below divides up the given range into chunks.
int numberToAdd = stoi(argv[2]);
cout << "\nThe number of threads: " << numberOfThreads << " - and number to sum to: " << numberToAdd;
int numberOfBlocks = numberOfThreads;
int blockSize = floor(numberToAdd / numberOfBlocks);
// initial block size I should subtract one?
int rem = numberToAdd % numberOfBlocks;
int initialBlock = blockSize - 1;
int currentTotal = 0;

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

// Loop below iterates through the number of threads and creates a range of numbers stored in a message (value1, value2), for the addem function.
vector<msg> mV;
for(int j = 1; j < numberOfThreads + 1; j++){
	struct msg m;
	m.iSender = 0;
	m.type = RANGE;
	m.value1 = currentTotal;
	m.value2 = currentTotal + blockSize - 1;

	// If we are at the last index, remember to add the remainder!
	if(j == numberOfThreads){
		m.value2 = currentTotal + blockSize + rem;
	}
	
	currentTotal = currentTotal + blockSize;
	//cout << "\nThe current total is: " << currentTotal;
	mV.push_back(m);
}

// Loop through the number of threads, create each thread and have them run on the addem function.
for(int k = 1; k < numberOfThreads + 1; k++){
	int* num = (int*) malloc(sizeof(int));
	*num = k;
	if(pthread_create(&(threadVector[k]), NULL, addem, (void*) num) != 0){
		perror("pthread_create");
		exit(1);
	}
}
// Loop through the number of threads, Send a message to each thread containing the range of numbers to iterate through, and then receive a message with the total summation.
int addemCount = 0;
for(int m = 1; m < numberOfThreads + 1; m++){
	msg z;
	SendMsg(m, &mV[m - 1]);


	int type = 0;
	while(type != ALLDONE){
		RecvMsg(0, &z);
		type = z.type;
		//cout << "\nChecking if all done!";
	}
	//cout << "\nValue in loop: " << z.value1;
	addemCount = addemCount + z.value1;
}

// Finally, print out the calculated total.
cout << "\n\nThe total is: " << addemCount << "\n";

// Waiting for threads to finish
for(int n = 1; n < numberOfThreads + 1; n++){
      (void)pthread_join(threadVector[n], NULL);
}

// Deleting semaphores as they are not needed.
for(int x = 0; x < semSendMailVector.size(); x++){
	(void)sem_destroy(&semSendMailVector[x]);
}
for(int y = 0; y < semReceiveMailVector.size(); y++){
	(void)sem_destroy(&semReceiveMailVector[y]);
}




return 0;
}





