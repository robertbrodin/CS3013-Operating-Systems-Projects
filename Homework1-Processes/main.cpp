/* myfork.C */

#include <iostream>
using namespace std;
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <time.h>
#include <cctype>
#include <vector>
#include <fstream>
#include <typeinfo>

/*
Programmed by Robert Brodin, A Term 2020-2021, for CS3013 (Operating Systems), with Professor Wills.
The homework assignment consists of three parts, part one is to write a program to run a given command in terminal.
Part two is to build a custom shell to run three custom commands and all other commands. The three custom commands are cd, set prompt, and exit.
Part three is to make processes run in the background. I got about halfway through part three, which was frustrating. I hope that you will at least consider giving me maybe a half a point,
because I have the framework pretty much there (in keeping track of jobs), I just couldn't finish process communication in time.

How my shell compares to regular shell:
For one, it is obviously less complex and has less capability than a normal shell. In addition, my shell for some reason glitches out when running commands such as a ; b, or a | b. 
I could easily add functionality for this in the future, but alas, I did not have the time or mental energy.


*/

main(int argc, char *argv[])
     /* argc -- number of arguments */
     /* argv -- an array of strings */
{
    int pid;
    int i;

    struct rusage rUsage;
    struct timeval startTimeTrack;
    clock_t cpuClockStart = clock();
    gettimeofday(&startTimeTrack, NULL);

    int test = argc;
    /* print out the arguments */
    cout << "There are " << argc << " arguments:\n";

    //for (i = 0; i < argc; i++)
//	cout << argv[i] << "\n";

    if ((pid = fork()) < 0) {
	cerr << "Fork error\n";
        exit(1);
    }
    else if (pid == 0) {
        /* child process */
	//wait(0);
	if(test == 1){
	cout << "\nNo arguments passed! Entering shell";
	cout << "\n----------------------------------------\n";
	cout << "               CUSTOM SHELL                 ";
	cout << "\n----------------------------------------\n";

	// How do I make it come back?
/*
So for the custom shell, do I need to use recursion?
Basically while command does not equal exit(), use fork

if argumentVector[0] is exit, don't run the while loop
otherwise, until the first argument given is exit, keep running the shell
ie: print out the prompt, fork off a process, run the command, print how well the time does, then repeat if the first argument isn't exit
*/
	bool isCommandExit = false;
        string prompt = "\n==> ";
        int globalPID;
	while(!isCommandExit){
		string command;
		cout << "\n" << prompt;
		//cin >> command;
		getline(cin, command);
		string current = "";
		//cout << "\nCommand1: " << command;
		//cout << "\nLength of command1: " << command.length();

		vector<string> argumentVector;
		for(int x=0;x < command.length();x++){

			//cout << "\n" << command[x];
			if(isspace(command[x]) or (x == command.length() - 1)){
				if(x == command.length() - 1){
					current = current + command[x];
				}
				argumentVector.push_back(current);
				current = "";
			}
			else{
				current = current + command[x];
				//cout << "\nCurrent is: " << current;
			}
		}

    bool runBackground = false;
    if(argumentVector[argumentVector.size() - 1] == "&"){
      cout << "\nFound an ampersand!";
      command.pop_back();
      runBackground = true;
    }
    char** argument;
    char* in = (char *) command.c_str();
    char* allMem[128];

    int index = 0;
    char* charLine = strtok(in, " ");
    while (charLine != NULL) {
        allMem[index] = charLine;
        index++;
        charLine = strtok(NULL, " ");
    }

    allMem[index] = NULL;
    // Switching from C++ array to C array (or rather a pointer)
    argument = &(allMem[0]);

    globalPID = getpid();
    // If command given is exit
		if(argumentVector[0] == "exit"){
			//isCommandExit = true;
			cout << "\nExiting shell!";
      exit(0);
}
    // If command given is cd
		else if(argumentVector[0] == "cd"){
			int ch = chdir(argument[1]);
			cout << "\nSwitched the directory! The current directory is now: " << system("pwd");
      if(ch == -1){
        cout << "\n Chdir failed!";
      }
}
    // If command given is set prompt
    else if(argumentVector[0] == "set" && argumentVector[1] == "prompt"){
      string newPrompt = "";
      for(int vectorIterator = 2; vectorIterator < argumentVector.size(); vectorIterator++){
        newPrompt = newPrompt + argumentVector[vectorIterator];
      }
      prompt = newPrompt + " ";
    }
    else if(argumentVector[0] == "jobs"){
      string text;
      ifstream t("test.txt");

      while(getline(t, text)){
        cout << "\nJob: " << text;
      }
      t.close();
    }
		else{
			//cout << "\nFirst command is: " << argumentVector[0];
			int shellPID;
			if ((shellPID = fork()) < 0) {
				cerr << "Fork error\n";
				exit(1);
			}
		    else if (shellPID == 0) {
			// Child process
				//cout << "\nRunning child process";
        cout << "\nCurrent process id: " << getpid();
        string text;
        ifstream t("test.txt");
        ofstream d("not.txt");

        vector<string> toWrite;
        while(getline(t, text)){

          d << "\n" << text;
        }
        d << "\n" << getpid() << " " << argument[0];
        t.close();

        d.close();
        remove("test.txt");
        rename("not.txt", "test.txt");

        // Could fork off if &, and then write to a file when it is completed
				int exec = execvp(argument[0], argument);
				if(exec == -1){
					cout << "Execvp failed! Command not run!";
					exit(1);
				}
				exit(0);
			}
			else{
			// Parent process
      // Maybe I need to add all of the running processes to a list? I could do that or a text file. Text file might be the easiest.
        wait(0);
	if(!runBackground){
          // Need to fork off another process
              int status;
              //waitpid(globalPID, &status, 0);
              int wait(0);
              cout << "\nProcess " << test << " finished!";

              // Remove job from job file here
              string text;
              ifstream t("test.txt");
              ofstream d("not.txt");

              vector<string> toWrite;
              while(getline(t, text)){

                if(!(text == (to_string(test) + " " + argumentVector[0]))){
                  //cout << "\nFound the job!";
                  d << "\n" << text;
                }
              }
              t.close();
              d.close();
              remove("test.txt");
              rename("not.txt", "test.txt");

        }
				//cout << "\nChild process finished!";
        runBackground = false;
				//exit(0);
			}
}
}
}


	//cout << argv;
	//const char *path = argv[1];

	int runningCode = execvp(argv[1], argv+1);
	if(runningCode == -1){
		cout << "\nExecvp failed! Meaning, the command did not run!\n";
		exit(1);
}
        exit(0);
    }
    else {
        /* parent */
	wait(0);
	// Wait for child process to finish, and then once it is finished calculate all of the statistics (specifically with time variables)
	getrusage(RUSAGE_CHILDREN, &rUsage);
        struct timeval endTimeTrack;
        gettimeofday(&endTimeTrack, NULL);
	// WALL CLOCK TIME, need to convert to Milliseconds (ie seconds * 1000)
	cout << "\n-----------Command Statistics-----------";
	cout << "\nWall time: " << ((endTimeTrack.tv_sec - startTimeTrack.tv_sec) + ((endTimeTrack.tv_usec - startTimeTrack.tv_usec) / 1e6)) * 1000 << setprecision(10) << " ms";
	//cout << "\nSystem:" << (usageTrack.ru_stime.tv_sec + (usageTrack.ru_stime.tv_usec / 1e6)) * 1000 << setprecision(10);
	cout << "\nUser specific: " << (((rUsage.ru_utime.tv_sec * 1e6) + rUsage.ru_utime.tv_usec) / 1000 ) << " ms" << setprecision(8);
	cout << "\nSystem specific: " << (((rUsage.ru_stime.tv_sec * 1e6) + rUsage.ru_stime.tv_usec) / 1000 ) << " ms" << setprecision(8);
	clock_t cpuClockEnd = clock();
	//double cpuTime = (((double) cpuClockEnd - cpuClockStart) / CLOCKS_PER_SEC) * 1000;
	//cout << "\nCPU Time:" << cpuTime << setprecision(10) << " ms";
	cout << "\nInvoluntary: " << rUsage.ru_nvcsw;
	cout << "\nVoluntary: " << rUsage.ru_nivcsw;
	cout << "\nMajor page faults: " << rUsage.ru_majflt;
	cout << "\nMinor page faults: " << rUsage.ru_minflt;
	cout << "\n----------------------------------------\n";
	exit(0);

    }
}
