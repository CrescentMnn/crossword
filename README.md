## crossword puzzle

This project's primary goal was to create a dynamic crossword puzzle application that made use of topics i learned in my Operating Systems course, such as mutex, processes, signals, and
threads. The project's objective was to give players a demanding and engaging gaming experience while showcasing their ability to apply course principles to real-world problem-solving.

The project consists of:

    • There must be a minimum of three vertical and three horizontal words in the
    crossword puzzle.
    • Every word must cross paths with a minimum of one other word.
    • After a specific period of time, the entire board will change:
    • The word intersections are saved.

## Processes and Thread layout

The image below shows how the program is designed, the process B handles all of the board functions, while process C handles all the threads, which are used to
modify a global variable named ‘counter’ which is ultimately used to change the board. 

Process B’s only function is to call the functions that print, update, and receive the answers for the board and its questions accordingly.

![image](https://github.com/CrescentMnn/crossword/assets/169625322/26dfb661-1ba6-426c-b9d5-f407b1648367)

![image](https://github.com/CrescentMnn/crossword/assets/169625322/ea7d63ce-2f99-4e0a-91da-d01de115e220)
