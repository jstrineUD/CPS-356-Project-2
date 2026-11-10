CPS 356 - Operating Systems Project: Chef-Waiter Restaurant System and Dining Philosophers

PROJECT OVERVIEW

This project includes two C programs that demonstrate concurrency and synchronization using multiple threads.

PROBLEM 1: CHEF-WAITER RESTAURANT SYSTEM

The program simulates a restaurant where chefs prepare meals and waiters serve them. The chefs and waiters share a buffer with limited capacity.

The program uses pthreads to create multiple threads, a mutex to protect shared data, and condition variables to manage when chefs and waiters need to wait.

REQUIREMENTS

C compiler with pthread support

CLion or another C development environment

CMake (for building the CLion project)

HOW TO COMPILE AND RUN PROBLEM 1

Open the ChefWaiterSystem project in CLion.

Make sure main.c and CMakeLists.txt are in the project folder.

Select Build > Build Project.

Run the ChefWaiterSystem executable.

Enter the input values when prompted.

INPUT FORMAT

Enter four integers:

C W B M

C = Number of chefs (1-5) W = Number of waiters (1-5) B = Buffer capacity (5-20) M = Number of meals each chef prepares (1-20)

Example input: 2 2 5 3

EXPECTED RESULTS

Total meals prepared: 6 Total meals served: 6

The maximum buffer usage depends on thread execution but cannot exceed the specified buffer capacity.

TESTING

The following inputs were tested successfully:

2 2 5 3 - 6 meals prepared and served 3 2 5 4 - 12 meals prepared and served 5 5 20 20 - 100 meals prepared and served

PROBLEM 2: DINING PHILOSOPHERS SIMULATION

This program is being implemented separately.

Compilation, execution, and input instructions will be added after Problem 2 has been completed and tested.
