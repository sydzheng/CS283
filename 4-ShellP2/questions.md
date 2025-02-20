1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**: Since execvp alone only replaces the current process, once you call it in the main process your running, the parent process would no longer exist and would be replaced by the new command. fork creates a new child process that is an exact copy of the parent. If execvp is executed after, the child process is replaced with the new command instead and the parent shell can continue running.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If fork fails, no child process is created and my implementation prints an error message out and returns an error code before execvp can be reached. 

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp searched the PATH variable locations for binaries. 

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  Calling wait() in the parent process after forking allows the parent process wait for its children processes to complete before terminating. This allows the parent to get the exit status of the child process and ensure that the child process does not end in a resource leak.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**: WEXITSTATUS() provides the exit status of the child. This is important for error handling as the WEXITSTATUS macro will extract errno. Depending on the exit status of the child process, you can decide what you want to do next. 

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation of build_cmd_buff() uses two pointers to keep track of the beginning and end of an argument. It first will remove any leading spaces and then set the begPoint to the first char that is not a space. Then the iteratePoint will be set to the begPoint and then I iterate the iteratePoint until either the end of the command is reached or a space is reached. Then the length of the argument can be found by subtracting the iteratePoint from the begPoint. After the first word, there is an if condition that checks if the begPoint if a quote, and if it is, I move the begPoint up one to make sure the quote is not recorded in the output, and then I iterate the iteratePoint until it reaches another quote (which is the end quote). This is necessary because everything inside quotes is one argument and if it not parsed correctly, it may give unexpected results. 

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  The biggest change I made was that I manually parsed the command using pointers instead of using strtok. There were quite a few unexpected challenges in refractoring my old code. I essentially had to completely rewrite the parsing logic because the strtok would not handle quoted arguments well while the pointers gave a more secure way of handling. 

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  A signal in a Linus system is a message that notifies a process that an event of some type has occurred in the system. They provide a way to make visible the occurence of low level hardware exceptions to user processes. Signals differ from other forms of interprocess communication because they are asynchronous and limited, typically used in POSIX-compliant operating systems. 

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGINT is a signal that's generated when a user presses Control-C. This is typically used to terminate a program from the terminal. One specific use case is if there is an infinite loop running and Control-C is used to terminate. SIGABRT is a signal that's generated when a process executes the abort function. SIGKILL is a signal sent to a process to cause it to terminate at once. A process can forcibly terminate another process by sending it a SIGKILL signal. 

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process receives SIGSTOP, it will be paused. It can't be caught or ignored because its default actions cannot be changed. 
