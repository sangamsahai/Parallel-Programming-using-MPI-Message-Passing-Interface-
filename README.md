Average computation for big sized data using Parallel Programming through MPI(Message Passing Interface) in C.

This application uses the power of Parallel Programming to compute the average of rows in a large double dimensional Array.
The size which we have considered is 1200 * 1024.

This application leverages parallel programming to compute the desired result in substantially less time.
Message Passing Interface(MPI) is used to do acheive the computation between different cores in the processing unit.

This program has been written in language C.

Basically  , a Master process initializes and poppulates the array and then uses Slave processes ( running on different cores )
to complete the computation.

The command used to compile the code is ->   mpicc -o <executable name>  <file name>
The command used to run this code is    ->   mpirun -np <no of parallel processors> <executable name> 