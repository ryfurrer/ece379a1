Objectives:
This programming assignment is intended to give experience in using Unix function calls that manage processes, like fork() and execlp().

Design Overview: 
In general, c++ code is used, where I code into functions for reuse with comments identifing its objective from that assignment.

safeguard function provides the setrlimit command.

1. a1jobs
    -struct created for a single job, a list of jobs uses a vector
    -all five commands seperated into their own functions

2. a1mon
    -split:         splits a line from ps into a tuple
    -terminate:     kills a process
    -find_pid:      searches for the target pid in order to know when it ends
    -get_children:  gets the children (and recurses for their children) of the target pid
    -run_ps:        runs the ps command
    -main:          sets up the program and provides main loop

Project Status:
All functionality delivered for valid input. Invalid inputs unhandled for cmd line arguments in a1mon and for some invalid run commands. I had difﬁculty storing commands as "const char*," so std::string was used. Also, I had difﬁculty handling improper programs for run commands. I could not use waitpid() to check the status of the run command, since it would suspend execution of the calling process. So incorret run programs they cause a1jobs to be forked and then the program needs to be "quit" twice if using "quit."


Testing and Results:
The programs were compared with the example inputs and the outputs were equivalent.
a1jobs was validated for if all the commands worked and it allows only 32 jobs.
Invalid inputs were tested for a1jobs. A problem in a1jobs with incorrect run command was identified. Incorrect arguments would cause the program to fork the a1jobs program instead of printing and error message.
a1mon was tested by running it with a1jobs and seeing it killed all processes after a1jobs quits.
times() never returns values for user and sys.

Acknowledgments:

a1jobs uses a split function code taken from stackoverflow and written by Evan 
Teran.
https://stackoverflow.com/questions/236129/the-most-elegant-way-to-iterate-the-words-of-a-string#236803
https://stackoverflow.com/users/13430/evan-teran

Class notes were used and the Makefile is adapted from the examples given on eclass.

The linux man pages were used to help understand times(), execlp(), popen() and setrlimit().
https://linux.die.net/man/
