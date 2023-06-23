# Byteshell

Byteshell is a simple shell program implemented in C. It provides a command-line interface where users can enter commands and execute them. It supports various built-in functions and maintains command history.

## Features

- **Command Execution**: Users can enter commands, and the shell will execute them using the `execvp` system call. If the command is not a built-in function, it will be executed as an external command.
- **Built-in Functions**: Byteshell provides several built-in functions that can be executed directly from the shell prompt, including:
  - `cd`: Change the current directory.
  - `help`: Display information about available built-in functions.
  - `exit`: Terminate the shell.
  - `history`: Display the command history.
  - `echo`: Print the provided arguments to the console.
  - `export`: Set the value of a variable in the shell's map.
  - `math`: Perform arithmetic operations on variables in the shell's map.
  - `get`: Get the value of a variable from the shell's map.
- **Command History**: Byteshell maintains a stack-based command history, allowing users to access and repeat previously executed commands.
- **Map Implementation**: The shell uses a map data structure to store variables and their corresponding values. It provides functions to get and set values in the map.

## Implementation Details

- The shell reads input from the user using the `Read_Line` function, which dynamically allocates memory to store the input line.
- The input line is then parsed into individual arguments using the `Split_Line` function. The arguments are stored in a dynamically allocated array.
- The shell maintains a stack (`Hist`) to store the command history. Each command is represented by a `Command` struct, which contains the arguments and the size of the arguments array.
- The `Execute` function is responsible for executing commands. It checks if the command is a built-in function and calls the corresponding function, or launches an external command using `execvp`.
- The shell provides utility functions like `concatenateStrings` to concatenate arguments into a single string with spaces between them.
- The `cd` function uses the `chdir` system call to change the current directory.
- The `export` and `get` functions interact with the map data structure to set and retrieve variable values.
- The `Arithmetic` function performs basic arithmetic operations on variables in the map.

## Getting Started

To compile and run the Byteshell program, follow these steps:

1. Open a terminal and navigate to the directory containing the `byteshell.c` file.
2. Compile the code using the following command: `gcc byteshell.c -o byteshell`.
3. Run the shell by executing the compiled binary: `./byteshell`.
4. You will see a shell prompt (`>`) where you can enter commands and execute them.
