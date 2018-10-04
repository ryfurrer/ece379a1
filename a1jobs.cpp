/**
 a1jobs
 
 @author Ryan Furrer (rfurrer)
 */

#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

#include <unistd.h>

#include <sys/resource.h>

#include <sys/time.h>
#include <sys/times.h>


using std::vector;

#define MAXJOBS 32

struct job {
    uint index; 
    pid_t pid; 
    std::string cmd;
    uint running=1;
} ;

uint job_idx = 0;
typedef std::vector<job> Jobs;
Jobs jobs;

///////////////////////////////////////////////////////////
/*
Split function code taken from stackoverflow and written by Evan 
Teran.
https://stackoverflow.com/questions/236129/the-most-elegant-way-to-iterate-the-words-of-a-string#236803
https://stackoverflow.com/users/13430/evan-teran
*/
template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

vector<std::string> split(const std::string &s, char delim) {
    vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}
/////////////////////////////////////////////////////////////


void safeguard() {
	rlimit my_rlimit { .rlim_cur = 600, .rlim_max = 600 };
	setrlimit(RLIMIT_CPU, &my_rlimit );
}

job makeJob(pid_t p, std::string s){
    job j;
    j.index = job_idx;
    j.pid = p;
    j.cmd = s;
    return j;
}

/*
1. list: List all admitted jobs that have not been explicitly terminated by the user. Each entry of the listing contains (at least) the stored index, pid, and command line associated with the process.
*/
void list(){
    for (int i = 0, size = jobs.size(); i < size; ++i) {
        if (jobs[i].running) std::cout << jobs[i].index << ": (pid=" << jobs[i].pid << ", cmd=" << jobs[i].cmd << ")" << std::endl;
    }
}

/*
2. run pgm arg1 ... arg4: Fork a process to run the program specified by pgm, using the given arguments (at most 4 arguments may be specified). 
A job is considered admitted if the program is successfully executed. 
The process running pgm is the head process of the job. As mentioned above, program a1jobs admits at most MAXJOBS jobs (the count includes
jobs that the user has explicitly terminated).
*/
uint run (vector<std::string> items, std::string s) {
    
    if (job_idx < MAXJOBS) {
        pid_t c_pid = fork();
        if (c_pid == 0) {
            switch (items.size()) { // items contains the "run" command
                case 1:
                    std::cout << "ERROR: Missing pgm" << std::endl;
                    return -1;
                case 2:
                    execlp(items[1].c_str(), items[1].c_str(), (char *)NULL);
                    break;
                case 3:
                    execlp(items[1].c_str(), items[1].c_str(), items[2].c_str(), (char *)NULL);
                    break;
                case 4:
                    execlp(items[1].c_str(), items[1].c_str(), items[2].c_str(), items[3].c_str(), (char *)NULL);
                    break;
                case 5:
                    execlp(items[1].c_str(), items[1].c_str(), items[2].c_str(), items[3].c_str(), items[4].c_str(), (char *)NULL);
                    break;
                case 6:
                    execlp(items[1].c_str(), items[1].c_str(), items[2].c_str(), items[3].c_str(), items[4].c_str(), items[5].c_str(), (char *)NULL);
                    break;
                default:
                    std::cout << "ERROR: Too many args for run" << std::endl;
                    return -1;
            }
            
        } else {
            jobs.push_back(makeJob(c_pid, s));
            job_idx++;
            
        }
        
        
        return 0;
    }
    std::cout << "Too many jobs started" << std::endl;
    return -1;
}

/*
3. suspend jobNo: Suspend the job whose index is jobNo by sending signal SIGSTOP to
its head process.
*/
void suspend(uint jobNo) {
    if (jobNo < job_idx) { kill(jobs[jobNo].pid, SIGSTOP);}
    else { std::cout << "Invalid jobNo" << std::endl;}
}


/*
4. resume jobNo: Resume the execution of the job whose index is jobNo by sending signal
SIGCONT to its head process.
*/
void resume(uint jobNo) {
    if (jobNo < job_idx) { kill(jobs[jobNo].pid, SIGCONT);}
    else { std::cout << "Invalid jobNo" << std::endl;}
}

/*
5. terminate jobNo: Terminate the execution of the job whose index jobNo by sending
signal SIGKILL to its head process.
*/
void terminate (uint jobNo) {
    if (jobNo < job_idx) {
        std::cout << "Killing: " << jobs[jobNo].pid << std::endl;
        jobs[jobNo].running = 0;
        kill(jobs[jobNo].pid, SIGKILL);
    }
    else { std::cout << "Invalid jobNo" << std::endl;}
}

int main(int argc, char *argv[]) {
    

	// 1. provide some safeguard against a buggy process that may run forever.
	safeguard();

	// 2. Call function times() (see the table above) to record the user and CPU times for the current process (and its terminated children).
	tms st_cpu;
	static clock_t st_time = times(&st_cpu);

	pid_t pid = getpid();

	// 3. Run the main loop
	for (;;) {
		// prompts the user to enter a command line (using the prompt "a1jobs[pid]: "
		std::string cmd;
		std::cout << "a1jobs[" << pid << "]: ";
        std::getline(std::cin, cmd);
        
        vector<std::string> items = split(cmd, ' ');
        
		if (items.empty()) {
            std::cout << "ERROR: Missing command" << std::endl;
        } else if (items[0] == "list") {
            list();
        } else if (items[0] == "run") {
            std::cout << run(items, cmd) << std::endl;//items[1]);
        } else if (items[0] == "suspend") {
            suspend(atoi(items[1].c_str()));
        } else if (items[0] == "resume") {
            resume(atoi(items[1].c_str()));
        } else if (items[0] == "terminate") {
            terminate(atoi(items[1].c_str()));
        } else if (items[0] == "exit") {
            for (int i = 0, size = jobs.size(); i < size; ++i) {
                if (jobs[i].running) terminate(i);
            }
            break;
        } else if (items[0] == "quit") {
            std::cout << "Exiting a1jobs without terminating head processes" << std::endl;
            break;
        } else {
            std::cout << "ERROR: Invalid Input" << std::endl;
        }

	}

	// 4. Upon exiting the main loop, the program calls function times() again to obtain the user and system CPU times for itself and its terminated children.
    
	tms en_cpu;
	static clock_t en_time = times(&en_cpu);

    printf("real:        %li sec.\n", (long int)(en_time - st_time)/sysconf(_SC_CLK_TCK));
    printf("user:        %li sec.\n", (long int)(en_cpu.tms_utime - st_cpu.tms_utime)/sysconf(_SC_CLK_TCK));
    printf("sys:         %li sec.\n", (long int)(en_cpu.tms_stime - st_cpu.tms_stime)/sysconf(_SC_CLK_TCK));
    printf("child user:  %li sec.\n", (long int)(en_cpu.tms_cutime - st_cpu.tms_cutime)/sysconf(_SC_CLK_TCK));
    printf("child sys:   %li sec.\n", (long int)(en_cpu.tms_cstime - st_cpu.tms_cstime)/sysconf(_SC_CLK_TCK));
    exit(0);
}
