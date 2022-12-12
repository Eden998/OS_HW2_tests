#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cassert>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

int main() {
	 int num = 5;
	 int r = syscall(335); // get_weight
	 std::cout << "get_weight returned " << r << std::endl;
	 assert(r == 0);
	 
	 r = syscall(334, -num); // set_weight
	 std::cout << "set_weight returned " << r << std::endl;
	 assert(r == -1);
	 
	 r = syscall(334, num); // set_weight
	 std::cout << "set_weight set to " << num << std::endl;
	 std::cout << "set_weight returned " << r << std::endl;
	 assert(r == 0);
	 
	 r = syscall(335); // get_weight
	 std::cout << "get_weight returned " << r << std::endl;
	 assert(r == 5);
	 
	 r = syscall(336);
	 std::cout << "get_leaf_children_sum returned " << r << std::endl;
	 assert(r == -1);
	 
	 pid_t child1_pid;
	 pid_t child2_pid;
	 pid_t heaviest;
	 child1_pid = fork();

	 if(child1_pid != 0){
		 r = syscall(336); // get_leaf_children_sum
		 std::cout << "get_leaf_children_sum returned " << r << std::endl;
		 assert(r == 5);
		 wait(NULL);
	 }
	 else{
		 sleep(1);
		 r = syscall(336); // get_leaf_children_sum
		 std::cout << "get_leaf_children_sum returned " << r << std::endl;
		 assert(r == -1);
		 exit(0);
	 }
	 
	 pid_t* heaviest_pid_pointer;
	 int shm_fd;
	 int msize = sizeof(pid_t);
	 const char *name = "HEAVIEST_PID";
	 shm_fd = shm_open (name, O_CREAT | O_EXCL | O_RDWR, S_IRWXU | S_IRWXG);
	 if (shm_fd < 0) {
		 std::cout << "Error in shm_open()" << std::endl;
	 }
	 ftruncate(shm_fd, msize);
	 heaviest_pid_pointer = (pid_t*) mmap(NULL, msize, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	 
	 r = syscall(334, 7); // set_weight
	 child1_pid = fork();
	 if(child1_pid == 0){ // Child
		 r = syscall(334, 13); // set_weight
		 child2_pid = fork();
		 if(child2_pid == 0){ // Child2
			 sleep(1);
			 r = syscall(334, 5); // set_weight
			 heaviest = syscall(337); // get_heaviest_ancestor
			 std::cout << "get_heaviest_ancestor returned " << heaviest << std::endl;
			 assert(heaviest == *heaviest_pid_pointer);
			 exit(0);
		 }
		 *heaviest_pid_pointer = getpid();
		 wait(NULL);
		 heaviest = syscall(337); // get_heaviest_ancestor
		 std::cout << "get_heaviest_ancestor returned " << heaviest << std::endl;
		 assert(heaviest == *heaviest_pid_pointer);
		 exit(0);
	 }
	 wait(NULL);
	 *heaviest_pid_pointer = getpid();
	 heaviest = syscall(337); // get_heaviest_ancestor
	 std::cout << "get_heaviest_ancestor returned " << heaviest << std::endl;
	 assert(heaviest == *heaviest_pid_pointer);
	 shm_unlink("HEAVIEST_PID");
	 
	 std::cout << "=========SUCCESS=========" << std::endl;
	 return 0;
}
