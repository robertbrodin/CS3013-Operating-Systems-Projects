/* fileio.C */

#include <iostream>
using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>


// Programmed by Robert Brodin, A Term 2020, CS 3013, Operating Systems, Professor Wills
// The goal of this assignment was to use two different ways of iterating through files and finding which pieces of each file were printable.
// Iterating through the files is done in two ways, either with the read() system call, or with the mmap() system call.

main(int argc, char *argv[])
{

    // BUFSIZE (ie the size of the chunks to iterate through), is default set to 1024, this is changed if the third argument given on the command line is a number.
    int BUFSIZE = 1024;
    int fdIn, cnt, i;

    // If less than two arguments are given, than exit.
    if (argc < 2) {
	cout << "\nPlease specify more than two arguments! Format: ./proj4 srcfile [mmap|size]\n";
	exit(1);
    }
    else if ((fdIn = open(argv[1], O_RDONLY)) < 0) {
	cerr << "file open\n";
	exit(1);
    }

    // useMMAP and changeSize are booleans used to see if mmap() should be used instead of read, and if a different buffer size is given on the command line.
    // Both of these booleans are changed if an PROPER (ie mmap or a number) argument is given on the command line.
    bool useMMAP = false;
    bool changeSize = true;
    // If more than two arguments are given, move forward.
    if(argc > 2){
        // If the third command line argument is mmap, set useMMAP to true.
	if(strcmp(argv[2], "mmap") == 0){
		useMMAP = true;
		changeSize = false;
	}
        // If the third command line argument is a number, then set BUFSIZE to the number given on the command line.
        // The code written below might seem a little odd because it is. I couldn't figure out how to check if an entire string was a number so I used atoi() to check if the first char is a number, and if not, then loop through the rest of the string to figure out if there are any non numbers. If not, then changeSize is set to true.
	else if(atoi(argv[2]) != 0){
		for(int t=0;t<strlen(argv[2]);t++){
			if(isdigit(argv[2][t]) == 0){
				changeSize = false;
			}
		}
		if(changeSize){

			BUFSIZE = atoi(argv[2]);

  			// Check if BUFSIZE is less than 8K (8192 bits), if it is not, than exit!
			if(BUFSIZE > 8192){
				cout << "\nBUT... the chunk size has to be less than 8192 (8K) bytes!" << flush;
				exit(1);
			}

			
		}
	}
        // A third argument is given but it is incorrect.
	else{
		cout << "\nGiven a third argument that is not mmap or a chunk size number!" << flush;
		exit(1);
	}
}

    // The buffer array is defined as the buffer size, which might have been changed by the input.
    char buf[BUFSIZE];

    // if we are not using mmap(), use read().
    if(!useMMAP){
	    // copy input to stdout
	    // each chunk is default 1024 byte chunks (ie 1024 characters)
	    int totalPrintable = 0;
	    int total = 0;
	    int numBlocks = 0;
	    // Read in the chunks and keep track of readable and printable characters.
	    while ((cnt = read(fdIn, buf, BUFSIZE)) > 0) {
		int counter = 0;
		for(int x = 0; x < cnt; x++){
			//cout << "\n" << buf[x];
			if(isprint(buf[x]) || isspace(buf[x])){
				totalPrintable++;
			}
			total++;
		}
		numBlocks++;
	    }

            // Calculate the ratio of printable characters to total characters
	    double ratio = ((double)totalPrintable) / ((double)total);
            // Display that ratio and the total number of bytes read!
	    cout << "\n" << totalPrintable << " out of " << total << " bytes, " << ratio * 100 << "%" << setprecision(10) << "\n";

 	    // Lastly, close the file.
	    if (fdIn > 0){
		close(fdIn);
		}
    } // end of the read() conditional
    else{
	    int fd;
	    struct stat sb;
	    // Open the file: 
	    if(argc < 2) {
		cerr << "Usage: ", argv[0], " <input>\n";
		exit(1);
	    }
	    
	    /* Map the input file into memory */
	    if ((fd = open (argv[1], O_RDONLY)) < 0) {
		perror("Could not open file");
		exit (1);
	    }

	    if(fstat(fd, &sb) < 0){
		perror("Could not stat file to obtain its size");
		exit(1);
	    }
	    // Allocate the memory char* pchFile with the needed number of bytes. pchFile is the pointer to the entire contents of the file.
	    char *pchFile = (char*) malloc(sizeof(char) * (sb.st_size / sizeof(char)));
	   

	    if ((pchFile = (char *) mmap (NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0)) 
		== (char *) -1)	{
		perror("Could not mmap file");
		exit (1);
	    }

	    // pchFile is now a pointer to the entire contents of the file...
	    // do processing of the file contents here
	    // each chunk is default 1024 byte chunks (ie 1024 characters)
	    int totalPrintableMMAP = 0;
	    int totalMMAP = 0;
	    // sizeOfData calculates the amount of characters in the char* mapped to virtual memory (ie the size of the data used divided by the size of the  .
	    int sizeOfData = (sb.st_size / sizeof(char));
	    // Count the number of printable characters
	    for(int x = 0; x < sizeOfData; x++){
		if(isprint(pchFile[x]) || isspace(pchFile[x])){
			totalPrintableMMAP++;
		}
		totalMMAP++;
	    }
            // Calculate the ratio of printable characters to total characters
	    double ratioMMAP = ((double)totalPrintableMMAP) / ((double)totalMMAP);
            // Display that ratio and the total number of bytes read!
	    cout << "\n" << totalPrintableMMAP << " out of " << totalMMAP << " bytes, " << ratioMMAP * 100 << "%" << "\n";


	    // Now clean up
	    if(munmap(pchFile, sb.st_size) < 0){
		perror("Could not unmap memory");
		exit(1);
	    }
            // Display that ratio and the total number of bytes read!
	    close(fd);
    } // end of the mmap() conditional



}
