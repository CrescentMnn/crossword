#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <wait.h>
#include <stdbool.h>

#define MAX_SIZE 20
#define TIMER 30

//counter for time and board_num to check what board goes next
int counter, current_board, status_B, status_C;

//process num
pid_t pid_B, pid_C;

//mutex
pthread_mutex_t counter_mutex;

//word bank
char *word_bank[4][6] = {
    {"leon", "lona", "agua", "grado", "canguro", "nieve"},
    {"radiador", "diametro", "recursivo", "computadora", "procesador", "memoria"},
    {"telefono", "lapiz", "azar", "ozono", "rotacion", "natacion"},
    {"programacion", "presidente", "ramificacion", "francia", "anecdota", "torre"}
    
};

//horizontal or vertical, horizontal = true , else false
bool word_placement[4][6] = {
    {true, false, true, false, true, false},
    {false, true, false, true, false, true},
    {false, true, false, true, true, false},
    {true, false, false, true, true, false}
};

//hints
char *hint_bank[4][6] = {
    
    {"mamifero conciderado rey de la jungla", "tejido pesado utilizado para publicidad", "escencial para la vida, h2o", "unidad de medicion, 90 y 180", "animal australiano conocido por su habilidad de boxeo", "Precipitación congelada"},
    {"parte del sistema de enfriamiento del coche", "segmento de recta que pasa por el centro y une dos puntos opuestos de una circunferencia.", "se repite o se aplica a si mismo", "dispositivo tecnologico el cual estas usando ahora mismo", "unidad de procesamiento principal de una computadora", "facultad para retener y recordar eventos del pasasdo"},
    {"dispositivo el cual nos permite hacer llamdadas y comunicarnos", "utencilio el cual se usa para escribir", "casualidad, oportunidad, lo hice al ___", "la tierra esta rodeada por la capa de ___", "acción que ejerce un cuerpo para dar un giro", "deporte acuatico"},
    {"proceso de crear un conjunto de instrucciones que le dicen a una computadora como realizar algún tipo de tarea.", "lider de un pais democrata", "Conjunto de consecuencias necesarias de algún hecho", "pais conocido por la torre eiffel", "Relato breve de un hecho curioso pasado", "Construcción fortificada, más alta que ancha, utilizada para defender una ciudad o plaza."}
    
};

//boards
char first_board[MAX_SIZE][MAX_SIZE] = {
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '1', '1', '1', '1', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '2', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '2', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '2', '3', '3', '3', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '4', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '4', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '4', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '5', '5', '5', '5', '5', '5', '4', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '6', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '6', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '6', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '6', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'}
};

char second_board[MAX_SIZE][MAX_SIZE] = {
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '1', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '1', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '1', '2', '2', '2', '2', '2', '2', '2', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '1', '.', '.', '.', '.', '.', '3', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '1', '.', '.', '.', '.', '.', '3', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '1', '.', '.', '.', '.', '.', '3', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '1', '.', '.', '.', '.', '.', '3', '.', '.', '.', '.'},
    {'4', '4', '4', '4', '4', '4', '4', '4', '4', '1', '4', '.', '.', '.', '.', '3', '.', '.', '.', '.'},
    {'.', '.', '.', '5', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '3', '.', '.', '.', '.'},
    {'.', '.', '.', '5', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '3', '.', '.', '.', '.'},
    {'.', '.', '.', '5', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '3', '.', '.', '.', '.'},
    {'.', '.', '6', '5', '6', '6', '6', '6', '6', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '5', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '5', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '5', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '5', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '5', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'}
};

char third_board[MAX_SIZE][MAX_SIZE] = {
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '1', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '1', '.', '.', '.', '3', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '1', '2', '2', '2', '2', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '1', '.', '.', '.', '3', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '1', '.', '.', '.', '3', '5', '5', '5', '5', '5', '5', '5', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '1', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '6', '.', '.', '.'},
    {'.', '.', '4', '4', '4', '1', '4', '.', '.', '.', '.', '.', '.', '.', '.', '.', '6', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '1', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '6', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '6', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '6', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '6', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '6', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'}
};

char fourth_board[MAX_SIZE][MAX_SIZE] = {
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '2', '.', '.', '.', '3', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '2', '.', '.', '.', '3', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '2', '.', '.', '.', '3', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '2', '.', '.', '.', '3', '4', '4', '4', '4', '4', '4', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '2', '.', '.', '.', '3', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '2', '.', '.', '.', '3', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '2', '.', '.', '.', '3', '5', '5', '5', '5', '5', '5', '5', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '2', '.', '.', '.', '3', '.', '.', '.', '.', '.', '6', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '2', '.', '.', '.', '3', '.', '.', '.', '.', '.', '6', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '3', '.', '.', '.', '.', '.', '6', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '3', '.', '.', '.', '.', '.', '6', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'}
};

//shows correct answer on board
void show_answer(int num, char board[MAX_SIZE][MAX_SIZE], int current_board){
    
    //gets char at [x] position of word bank
    int board_char = 0, get_int = num - 1;
    
    for(int i=0; i<MAX_SIZE; i++){
        for(int j=0; j<MAX_SIZE; j++){
            if(num + '0' == board[i][j]){
                board[i][j] = word_bank[current_board][get_int][board_char];
                board_char+=1;
            }else if(board[i][j] != num + '0' && board[i+1][j] == num + '0' && board[i-1][j] == num + '0' && board[i][j] != '.'){
                if(word_placement[current_board][num-1] == false){
                    board[i][j] = word_bank[current_board][get_int][board_char];
                    board_char+=1;
                }
            }else if(board[i][j] != num + '0' && board[i][j+1] == num + '0' && board[i][j-1] == num + '0' && board[i][j] != '.'){
                if(word_placement[current_board][num-1] == true){
                    board[i][j] = word_bank[current_board][get_int][board_char];
                    board_char+=1;
                }
            }else if(board[i][j] != num + '0' && board[i+1][j] == num + '0' && board[i][j] != '.'){
                if(word_placement[current_board][num-1] == false){
                    board[i][j] = word_bank[current_board][get_int][board_char];
                    board_char+=1;
                }
            }else if(board[i][j] != num + '0' && board[i][j+1] == num + '0' && board[i][j] != '.'){
                if(word_placement[current_board][num-1] == true){
                    board[i][j] = word_bank[current_board][get_int][board_char];
                    board_char+=1;
                }
            }
        }
    }
}

//gets rand num for a board
void choose_board(){
    srand(time(NULL));
    current_board = rand() % 4;
}

//prints given board based on introduced num
void print_board(int board_num){
    
    if(board_num == 0){
        for(int i=0; i<MAX_SIZE; i++){
            printf("\t\t\t");
            for(int j=0; j<MAX_SIZE; j++){
                printf("%c ", first_board[i][j]);
            }
            printf("\n");
        }
    }else if(board_num == 1){
        for(int i=0; i<MAX_SIZE; i++){
            printf("\t\t\t");
            for(int j=0; j<MAX_SIZE; j++){
                printf("%c ", second_board[i][j]);
            }
            printf("\n");
        }
    }else if(board_num == 2){
        for(int i=0; i<MAX_SIZE; i++){
            printf("\t\t\t");
            for(int j=0; j<MAX_SIZE; j++){
                printf("%c ", third_board[i][j]);
            }
            printf("\n");
        }
    }else if(board_num == 3){
        for(int i=0; i<MAX_SIZE; i++){
            printf("\t\t\t");
            for(int j=0; j<MAX_SIZE; j++){
                printf("%c ", fourth_board[i][j]);
            }
            printf("\n");
        }
    }
    
}

//prints hint bank based on global var
void print_hints(){
    
    printf("\n");
    for(int i=0; i<6; i++){
        printf("%d. %s ", (i+1), hint_bank[current_board][i]);
        printf("\n\n");
    }
    
}

void update_board(){
    for(int i=0; i<10; i++){
        printf("\n");
    }
    print_board(current_board);
    print_hints();
}

void check_full_board(char arr[MAX_SIZE][MAX_SIZE]){

    int check_if_full = 0;
    
    for(int i = 0; i<MAX_SIZE; i++){
        for(int j = 0; j<MAX_SIZE; j++){
            
            if(arr[i][j] == '1' || arr[i][j] == '2' || arr[i][j] == '3' || arr[i][j] == '4' || arr[i][j] == '5' || arr[i][j] == '6'){
                check_if_full+=1;
            }
        }
    }
    if(check_if_full == 0){
        print_board(current_board);
        printf("\nYou Won!!");
        fflush(stdout);
        kill(pid_C, SIGUSR2);
        exit(25);
    }
    
}

//scanfs for qustion # and answer
void get_answers(){
    int question;
    char answer[20];
    printf("\nSelect a question number: ");
    scanf("%d", &question);
    if(question > 6 || question < 1){
        printf("Wrong number.....bye bye :)\n");
        kill(pid_C, SIGUSR2);
        exit(50);
    }
    printf("\n\n%s: ", hint_bank[current_board][(question-1)]);
    scanf("%s", answer);
    
    if(strcmp(answer, word_bank[current_board][(question-1)]) == 0){
        //answer is correct
        printf("Correct!\n");
        
        switch(current_board){
            case 0: 
                show_answer(question, first_board, current_board);
                check_full_board(first_board);
                update_board();
            break;
            
            case 1:
                show_answer(question, second_board, current_board);
                check_full_board(second_board);
                update_board();
            break;
            
            case 2: 
                show_answer(question, third_board, current_board);
                check_full_board(third_board);
                update_board();
            break;
            
            case 3:
                show_answer(question, fourth_board, current_board);
                check_full_board(fourth_board);
                update_board();
            break;
            
            default: printf("error getting current_board...\n"); break;
        }
        
    }else{
        printf("Incorrect answer...\n");
    }
}

//threads
void *counter_thread_func(){
    
    while(1){
        pthread_mutex_lock(&counter_mutex);
        counter+=1;
        //printf("\ncounter: %ds\n",  counter);
        pthread_mutex_unlock(&counter_mutex);
        sleep(1);
    }
    
}

void *counter_check_thread(){
    
    while(1){
        
        if(counter == TIMER){
            pthread_mutex_lock(&counter_mutex);
            counter=0;
            pthread_mutex_unlock(&counter_mutex);
            printf("\ntimer met: %d\n", counter);
            kill(pid_B, SIGUSR1);
        }
        sleep(1);
        
    }
    
}

void sigusr1_handler(int signum){
    //printf("\033[2J"); // Clears the screen
    
    for(int  i=0; i<3; i++){
        printf("\n");
    }
    
    choose_board();
    update_board();
    
}

void sigusr2_handler(int signum){
    //exits C process
    exit(50);
}

int main()
{
    //pid_t pid_B, pid_C;
    pthread_t counter_thread, counter_check;
    
    //tutorial/instructions
    
    printf("\t\t\tLa casa de hojas\n\n\n");
    printf("El crucigrama cambiara despues de %d segundos, las palabras que hayas adivinado\nse quedaran en su lugar\n", TIMER);
    sleep(5);
    
    pid_B = fork();
    
    if(pid_B < 0){ printf("Fork error...\n"); exit(25);}
    
    //b
    if(pid_B == 0){
        signal(SIGUSR1, sigusr1_handler);
        choose_board();
        
        while(1){
            update_board();
            get_answers();
        }
        
        exit(0);
    }
    
    pid_C = fork();
    
    if(pid_C < 0){ printf("Fork error...\n"); exit(25); }
    
    if(pid_C == 0){
        signal(SIGUSR2, sigusr2_handler);
        pthread_create(&counter_thread, NULL, &counter_thread_func, NULL);
        pthread_create(&counter_check, NULL, &counter_check_thread, NULL);
        
        pthread_join(counter_thread, NULL);
        pthread_join(counter_check, NULL);
        exit(0);
    }
    
    waitpid(pid_B, &status_B, 0);
    waitpid(pid_C, &status_C, 0);
    
    return 0;
}



