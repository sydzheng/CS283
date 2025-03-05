1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

_My implementation ensures that all child processes complete before the shell continues accepting user input by using the waitpid() on each child process. After all pipes are closed in the parent process, I loop through all the child PIDs and call waitpid() on each of them to ensure the shell continues to accept user input. If I forget to call waitpid() on all child processes, the child process could remain in a "zombie" state and eventually cause the system to run out of resources._

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

_It's necessary to close unused pipe ends after calling dup2() because dup2() copies the source FD onto the target, making the source no longer in use. Since descriptors are a limited resource, not closing the original pipes after copying with dup2() introduces a descriptor leak which could cause your program to run out of available file descriptors._

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

_cd is implemented as a built-in because this command modifies the current working directory and the current working directory is a property unique to each process. If cd were implemented as an external process, it would be ineffective because a new process is created to execute each command, so cd would have no effect on its parent shell because the child process would be promptly terminated._

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

_Instead of having a fixed number of piped commands, I would dynamically allocate memory to the array of commands to store the parsed commands using malloc(). Some trade-offs I would need to consider is careful memory management to make sure all the memory I allocate is freed properly. Performance can also be impacted with uptime of the program._
