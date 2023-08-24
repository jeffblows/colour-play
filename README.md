# colour-play
Sample code to use screen colours with ncurses. Also includes some samples of 

* command line porecessing
* threads
* mutex lock
* interupt handling

Examples of threads and mutex locks for both pthread and C11 threads.

Two lines of text are displayed with one thread updating the top line and a second updating the bottom line on the screen. Sending an interrupt to the running program will change the verbose level which allows for more detailed and verbose error messages.
