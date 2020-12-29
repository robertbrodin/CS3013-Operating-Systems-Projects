#!/bin/bash

read -p "Press enter to run the first test case, which runs part one of the homework assigment displaying the number each system call was invoked (using ./traceanal < ls.slog)."

./traceanal < ls.slog

echo ""
echo "" 
read -p "Press enter to continue to part two of the homework assignment, where I display the subsequent system calls. (using ./traceanal seq < ls.slog)"

./traceanal seq < ls.slog
