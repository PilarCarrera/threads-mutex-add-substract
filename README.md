# threads-mutex-add-substract
Program with threads and mutex in C++.
This project was my final project for the class "Parallel and real time" where you had 2 inputs that the user could write trough the console: 
the type of function to execute (addition or subtraction) and the size of the array.
This program uses  as many threads as the size of the array.
We first dynamically create the array of elements with malloc, saving the space for the array and building initially with numbers from 0 to the num of elements.
In this code we are going to separate the number of elements in different blocks of elements to pass to another function called “funcionGeneral” so it does the add or the subs on each block individually with the mutex lock and at the end the output is an only number which is the add or the sub of all of the numbers in the initial array. 
