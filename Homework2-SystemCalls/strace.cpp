#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string.h>
using namespace std;

/*
Programmed by Robert Brodin for Operating Systems at WPI, A term 2020, with Professor Wills.
Project 2 focuses on investigating a program dump of system call files, and investigating the number of times a system call was invoked, and for part two of the project, what call after the system call is invoked, and how often.
*/

void addNewSystemCall(map<string, int>* systemMap, vector<string>* systemKeyVector, string newSystemCall){
	(*systemMap)[newSystemCall] = 1;
	//cout << "\nInitialied the new value to zero!";
	(*systemKeyVector).push_back(newSystemCall);
	//cout << "\nPushed the new system call to the vector!";
}

main(int argc, char **argv){
	      /*string lineInput;
	      while(getline(cin, lineInput)){
	      		cout << "\n" << lineInput;
	      }*/

              // Remove job from job file here
              string text;
              //ifstream t("ls.slog");
              //ofstream d("not.txt");

              vector<string> toWrite;
	      // All of their keys with values for the number of system calls are stored here.
	      map<string, int> systemCallMap;
	      // All of the keys are stored in a vector, for easy iteration through the hashmap. O(1), I would hope...
	      vector<string> systemCallKeys;

	      // New hashmap created if the second argument of the given command is "seq", meaning that we want to count the system call after the last system call in another hashmap.
	      map<string, map<string, int>> systemCallSeqMap;
	  
	      map<string, vector<string>> systemCallSeqKeys;
	      //map["close"] = 0;
	      int total = 0;
	      string previousSystemCall = "firstSystemCall";
	      bool secondArgSeq = false;
	      //cout << "\nArg 1 is: " << argv[1];
	      if(argc > 1 && strcmp(argv[1], "seq") == 0){
	      		secondArgSeq = true;
			cout << "\nHit this statement!";
		}
              if(secondArgSeq){
		      while(getline(cin, text)){

		        //cout << "\n" << text;
			string systemCall = "";
			//cout << "\nHere: " << text << " " << total;
			for(int charLoop = 0; charLoop < text.length(); charLoop++){
				//cout << "\nChar: " << text[charLoop];
				char a = text[charLoop];
				if(a == '('){
					//cout << "\nFound call: " << systemCall;
					// If the system call already exists in the map, just add one. 
					if(systemCallMap.count(systemCall)){
						systemCallMap[systemCall] = systemCallMap[systemCall] + 1;
					}
					// Otherwise, initialize the new system call in the hashmap.
					// Data is off by one
					else{
						addNewSystemCall(&systemCallMap, &systemCallKeys, systemCall);
						// Initialize the new hashmap that the seq map contains
						map<string, int> temp;
						systemCallSeqMap[systemCall] = temp;
						vector<string> tempKeys;
						systemCallSeqKeys[systemCall] = tempKeys;
						//systemCallKeys.push_back(systemCall);
					}
                                        
					// Now, we have to look at the previous systemCall, and add it to the systemCallSeqMap.
				        // First, check if it is empty, if not, add one to the previous system call.
					if(systemCallSeqMap[systemCall][previousSystemCall]){
						systemCallSeqMap[systemCall][previousSystemCall] = systemCallSeqMap[systemCall][previousSystemCall] + 1;
						//cout << "\nUpdated the specific system call!";
					}
					else{
						systemCallSeqMap[systemCall][previousSystemCall] = 1;
						systemCallSeqKeys[systemCall].push_back(previousSystemCall);
					}
					previousSystemCall = systemCall;
					break;
				}
				else{
					systemCall = systemCall + text[charLoop];
				}

			}
			total++;
		      }
		      for(int i = 0; i < systemCallKeys.size(); i++){
				string a = systemCallKeys[i];
				cout << "\n" << a << " " << systemCallMap[a];
				for(int j = 0; j < systemCallSeqKeys[systemCallKeys[i]].size(); j++){

	/*
		      map<string, int> systemCallMap;
		      // All of the keys are stored in a vector, for easy iteration through the hashmap. O(1), I would hope...
		      vector<string> systemCallKeys;

		      // New hashmap created if the second argument of the given command is "seq", meaning that we want to count the system call after the last system call in another hashmap.
		      map<string, map<string, int>> systemCallSeqMap;
		  
		      map<string, vector<string>> systemCallSeqKeys;
	*/
					string b = systemCallSeqKeys[a][j];
					cout << "\n" << a << ":" << b << ": " << systemCallSeqMap[a][b];
					//cout << "\nWhat is my life: " << " systemCallSeqMap[a][b] << " " << systemCallSeqKeys["close"][j] << " " << systemCallSeqMap["close"]["mmap"];

	//[sysCallSeqKeys[sysCallKeys[i]][j]]
				}
				cout << "\n";
		      }

	      }
              else{
		      while(getline(cin, text)){

		        //cout << "\n" << text;
			string systemCall = "";
			//cout << "\nHere: " << text << " " << total;
			for(int charLoop = 0; charLoop < text.length(); charLoop++){
				//cout << "\nChar: " << text[charLoop];
				char a = text[charLoop];
				if(a == '('){
					//cout << "\nFound call: " << systemCall;
					// If the system call already exists in the map, just add one. 
					if(systemCallMap.count(systemCall)){
						systemCallMap[systemCall] = systemCallMap[systemCall] + 1;
					}
					// Otherwise, initialize the new system call in the hashmap.
					// Data is off by one
					else{
						addNewSystemCall(&systemCallMap, &systemCallKeys, systemCall);
					}
					break;
				}
				else{
					systemCall = systemCall + text[charLoop];
				}

			}
			total++;
		      }
		      int count = 0;
		      for(int sysCallKeyLoop = 0; sysCallKeyLoop < systemCallKeys.size(); sysCallKeyLoop++){
				cout << "\n" << systemCallKeys[sysCallKeyLoop] << ": " << systemCallMap[systemCallKeys[sysCallKeyLoop]];
			 	count = count + systemCallMap[systemCallKeys[sysCallKeyLoop]];
			}
	      }
              //t.close();

	      //cout << "\nThe total is: " << total << " - " << count;









return 0;
}