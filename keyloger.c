#include <stdio.h>
#include <linux/input.h> 

int main()
{
    FILE *file; 
    struct input_event hist;

    file = fopen ("/dev/input/event4", "r");  

    //Обработка ситуации, когда адрес файла с историей команд указанн не верно.
    if (file == NULL)
        printf("File not found. Please, specify your address jf the file with the keysttike history in my progrem\n"); 
      
    /* Что бы запустить программу нужно нажать "Enter". Но в файле event4 это нажатие тоже фиксируется и программа работает неверно.
    Чтобы избавиться от учета этого "Enter", введкм эти 3 строчик */
    fread (&hist, sizeof(struct input_event), 1, file); 
    fread (&hist, sizeof(struct input_event), 1, file);
    fread (&hist, sizeof(struct input_event), 1, file);
    
    long double begin_sec = hist.time.tv_sec, begin_usec = hist.time.tv_usec; //begin_sec - фиксрует сеунды в началe обработки символов; begin_usec - анлогичнао, но только для микросекунд.
    long double time_s, time_us; // time_s - фиксрует секунды в конце обработки символов; time_us - анлогично, но только для микросекунд.
    int counter; // counter - счетчик
    
    // hist.code - код события. Программа должна завершаться при нажатии "Enter", по этому код событием "Ввод Enter" (28) делаю условием завершение.
    for ( counter = 1; hist.code != 28 ; counter++){
    fread (&hist, sizeof(struct input_event), 1, file);
    time_s = hist.time.tv_sec;
    time_us = hist.time.tv_usec;
    }
   
    counter /= 6; //На нажатие одной клавиши приходиться несколько итераций цикла.
    long double spend_sec = time_s - begin_sec; // spend_sec - секунды потраченные на ввод.
    long double spend_usec = (time_us - begin_usec)/1000000; // spend_usec - доли секунды потраченные на ввод.

    /*spend_usec может получиться трицательным, из-за превращения микросекунд в секунды (это вызывает обнуление переменной spend_usec).
    Обработаем эту ситуацию*/
    if (spend_usec < 0) { 
    spend_usec = 1 + spend_usec;
    } 

    printf ("\n");
    printf ("Presses = %d\n", counter); //Кол-во нажатий
    printf ("Second = %Lf\n", spend_sec);
    printf ("Usecond = %Lf\n\n",spend_usec );

    long double spead = counter/(spend_sec + spend_usec);

    printf("Spead is %Lf keys/second", spead); //скорость в клавишах в секунду

    return 0;
}