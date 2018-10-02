/**
a1mon

The a1mon program is invoked by the command line:
"a1mon target pid [interval]", where:
• target pid is the pid of some process running on the same workstation 
(e.g., an a1jobs process) to be monitored
• interval is an integer that specifies a time interval in seconds. 
This argument is optional. If omitted from the command line it assumes
a default value of 3 seconds

@author Ryan Furrer (rfurrer)
*/


#include <stdio.h>      /* printf */
#include <stdlib.h>     /* strtol, stroi */
#include <tuple>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <sys/resource.h>
#include <signal.h>
#include <zconf.h>

static const int BUFF_SIZE = 128;

typedef std::vector< std::tuple<std::string, std::string, std::string, std::string, std::string, std::string> > Process_list;

typedef std::tuple<std::string, std::string, std::string, std::string, std::string, std::string> Process;


Process split(const std::string &s, char delim){
    std::stringstream ss(s);
    std::string item;
    Process out;
    int i = 0;
    
    while (std::getline(ss, item, delim)) {
        if (item.length()){
            if (i==0){std::get<0>(out) = item;}
            else if (i==1){std::get<1>(out) = item;}
            else if (i==2){std::get<2>(out) = item;}
            else if (i==3){std::get<3>(out) = item;}
            else if (i==4){std::get<4>(out) = item;}
            else if (i==5){std::get<5>(out) = item;}
            i++;
        }
    }
    return out;
}


//1. Use setrlimit() to set a limit on the CPU time (e.g., 10 minutes).
void safeguard() {
    rlimit my_rlimit;
    my_rlimit.rlim_cur = 600;
    my_rlimit.rlim_max = 600;
	setrlimit(RLIMIT_CPU, &my_rlimit );
}


void terminate(std::vector<std::tuple<std::string, std::string>> children) {
    for (int i = 0, size = children.size(); i < size; ++i) {
        kill(std::stoi(std::get<0>(children[i])), SIGKILL);
    }
}


int find_pid(Process_list ps_data, std::string tpid){
    for (int i = 0, size = ps_data.size(); i < size; ++i) {
        if (std::get<1>(ps_data[i]).compare(tpid) == 0) {
            return 1;
        }
    }
    return 0;
}


//returns a vector of all the pids and cmds of the 
//children to the parent process, ppid
std::vector<std::tuple<std::string, std::string>> get_children(Process_list ps_data, std::string ppid){
    std::vector<std::tuple<std::string, std::string>> base;
    printf("Monitored process list:\n");
    for (int i = 0, size = ps_data.size(); i < size; ++i) {
        if (std::get<2>(ps_data[i]).compare(ppid) == 0) {
            printf("%lu: %s, %s", base.size(), std::get<1>(ps_data[i]).c_str(), std::get<5>(ps_data[i]).c_str());
            base.push_back(make_tuple(std::get<1>(ps_data[i]), std::get<5>(ps_data[i])));
        }
    }
    
    return base; // (pid, cmd)
}




Process_list run_ps() {
    Process_list data;
    FILE *stream;
    char buffer[BUFF_SIZE];

    //(b) Use popen to execute the ps program in the background:
    stream = popen("ps -u $USER -o user,pid,ppid,state,start,cmd --sort start", "r");
    if(stream) {
        //(c) Read and display each line produced by popen. 
        while (fgets(buffer, BUFF_SIZE, stream) != NULL) {
            Process process = split(buffer, ' ');
            data.push_back(process);
            printf("%s", buffer);
        }
        // After reading all lines, the program uses function pclose() to close the pipe.
        pclose(stream);
    }
    return data;
}


int main(int argc, char *argv[]) {

	// 1. provide some safeguard against a buggy process that may run forever.
	safeguard();

    unsigned int interval = 0;
    unsigned int counter = 0;
    pid_t pid = getpid();
    
    char *target_pid;
    Process_list ps_data;
    std::vector<std::tuple<std::string, std::string>> children; //pid, cmd
    
    
    if (argc < 2){
        printf("Missing arguments\n");
        return 1;
    }
    
    target_pid = argv[1];
    
    if (argc == 3) {
        interval = (unsigned int) strtol(argv[2], (char **) NULL, 10);

    } else {
        interval = 3;
    }
    
    if (argc > 3) {
        printf("Too many arguments\n");
        return 1;
    }
    printf("Loop\n");

	// 2. Run the main loop
	for (;;) {
        //(a) Increments an iteration counter, and prints a header like the following one:
        //a1mon [counter= 9, pid= 3228, target_pid= 3114, interval= 3 sec]:
        counter++;
        printf("\na1mon[counter= %u, pid= %i, target_pid=%s, interval= %u sec]\n", counter, pid, target_pid, interval);
        
        ps_data = run_ps();
        //process each line produced by popen
        
        children = get_children(ps_data, target_pid);
        
        //display children;
        
        //(d) Based on the data obtained during the iteration, the 
        // program decides if the target process is still running.         
        if (find_pid(ps_data, target_pid) == 0) {
//             If terminated, the program terminates each
//             process in the process tree rooted at the target process.
            terminate(children);
//             The program then terminates.
            return 0;
        }
        
        
        //(e) The next iteration, is delayed by interval seconds
        sleep(interval);
	}

}