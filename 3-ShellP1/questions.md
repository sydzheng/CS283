1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**: fgets() is a good choice for this application because it allows us to control the size of the input and prevent buffer overflows. In this case, we have a limit for the size of user input so fgets allows us to read one line of input at a time, extracting as many characters that will fit in the buffer. 

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  Using malloc() to allocate memory for cmd_buff over allocating a fixed-size array was necessary because user input is not fixed. malloc() allows us to dynamically allocate memory to make sure there isn't unecessary memory being used while also ensuring that there is sufficient space to store the input. 


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  This is necessary because our shell might not recognize a command as a valid command if it had extra spaces. Trimming leading and trailing spaces from each command also ensures that empty commands are ignored, avoiding unpredictable behavior. The extra spaces could also cause the command to exceed the buffer limit even when it is within the limit. 

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  1) We should implement an input redirection so we can read commands from a file. There might be challenges in handling file paths that are missing or unreadable. We also need to ensure that we have the correct permissions to access the file. 2) We should implement an output redirection to redirect the output to a file. There might be challenges in leaving standard error connected to the terminal when this redirection is implemented so that error messages produced by the program in stdout can still be read. We would also need to make sure permissions are handled correctly and that files can be opened successfully. 3) We should implement a stdout to stderr redirection. There might be challenges in ensuring that stdout is redirected to a file descriptor rather than a file. We also need to ensure that the redirections are in the correct order so there isn't confusion on where errors are supposed to go.  

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection is used when you want to pass a command's input or output to or from either a file or stream while piping is used when you want to pass the stdout of one command to the stdin of another program or utility. 

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  It's important to keep these separate in a shell because stdout by design is for data in a pipe and can be used for further processing while stderr is for errors or other messages to the user. 

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  We could write a script that redirects the error message to a file of our choice using different redirection operators. If we want to merge stdout and stderr, we can use 2>&1.