# Book database implementation
Simple implementation of a database to store book registers with form: (int)book_id|(char(16))ISBN|(char)title|(char)editorial

# Functions
Functions in order to manage the database:
- add a new book
- find a added book
- delete a book from the database
- exit and release the resources used by the program
- print the information contained in each register
- print the information contained in the index array
- print the information contained in the deleted registers array

# Tests
There are several tests to try out the program, but not all the functions are tested

# Function
This program stores the database and indices and deleted registers in binary files, so is important to know how to manage and see them

# Makefile
The makefile has several options to compile the database and to run it depending on the strategy you want to use: best_fit, worst_fit or first_fit

> _[!NOTE]_
> Done by Alejandro Castro Sánchez