/* University of Alberta.
 * CMPUT379 Assignment 1
 * -------------------------------------------------------------------------------------------
 * TOPIC: IPC via Signals
 * CSID: zfu
 * Group47, I submitted and completed this assignment alone due to the missing partner issue.
 * Knowing issue: might crash if stress tested with SINGLE mode,  adjust the TIMESLICE to a higher number if not stable at 5000.
 */


#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#define TIMESLICE 5000 //change the TIMESLICE if chasing for higher or lower bits/minute

#ifdef SINGLE
int SINGLEMODE = 1; //single mode
#else
int SINGLEMODE = 0; //double mode
#endif

int pid2;
int bitcount = 0;
int is_busy;
char received_char = 0;
char received_str[81] = {0};
int received_str_index = 1; //received_str[0]='!'or'?'
char send_str[81];
long int send_time_array[81][8];
long int last_receive_time, current_receive_time;
int is_second_signal; //1---1
struct sigaction sa;
struct itimerval it_new_value, it_old_value;

void doubleSigHandler(int signum);
void singleSigHandler(int signum);
void singleMode();
void doubleMode();
void sendSignalSingle();
long int getCurrentUsec();
void convertStr2TimeArray();

int main()
{
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_flags = SA_RESTART;

    printf("% covertsigs\nOwn PID:%d\n", getpid());
    scanf("%d", &pid2);
    getchar(); //remove"\n"

    if (SINGLEMODE)
    {
        singleMode();
    }
    else
    {
        doubleMode();
    }
}

void singleMode()
{
    sa.sa_handler = singleSigHandler;
    if ((sigaction(SIGUSR1, &sa, NULL) < 0))
    {
        printf("Sigaction error!\n");
        exit(1);
    }
    received_str[0] = '!';
    while (1)
    {
        if (is_busy) //is_busy is a checker to prevent interleaving
        {
            while (1)
                ;
        }
        memset(send_str, 0, 81);
        fgets(send_str, 83, stdin);
        if (send_str[0] == '.') //Terminate if input '.'
        {
            printf("%%\n");
            exit(0);
        }
        memset(send_time_array, -1, 81);
        convertStr2TimeArray();
        sendSignalSingle();
    }
}

void doubleMode()
{
    sa.sa_handler = doubleSigHandler;
    if ((sigaction(SIGUSR1, &sa, NULL) < 0) ||
        (sigaction(SIGUSR2, &sa, NULL) < 0))
    {
        printf("Sigaction error!\n");
        exit(1);
    }

    char c;

    received_str[0] = '!';
    while (1)
    {
        if (is_busy)
        {
            while (1)
                ;
        }
        char send_str[81] = {0};
        fgets(send_str, 83, stdin);
        if (send_str[0] == '.')
        {
            printf("%%\n");
            exit(0);
        }
        int i, bit, ch_index;
        for (ch_index = 0; ch_index < 80; ch_index++)
        {
            if (send_str[ch_index] == 0)
            {
                break;
            }
            c = send_str[ch_index];
            for (i = 7; i >= 0; i--)
            {
                bit = c & (0x01 << i); //for every bit of a byte use & to check if its 0 or 1(for the input str)
                if (kill(pid2, (bit == 0) ? SIGUSR1 : SIGUSR2) == -1)
                {
                    printf("Error: can't send signal to process %d\n", pid2);
                    exit(1);
                }
                usleep(1);
            }
        }
    }
}

void singleSigHandler(int signo)
{
    if (signo == SIGUSR1)
    {
        // printf("get%d\n",t_count++);
        last_receive_time = current_receive_time; //reset time after every SIGUSR1 Recieved
        current_receive_time = getCurrentUsec();
        if (is_second_signal)
        {
            long int interval = current_receive_time - last_receive_time;

            // printf("last:%ld\n",last_receive_time);
            // printf("current:%ld\n",current_receive_time);
            // printf("interval:%ld\n",interval);
            // printf("round:%lf\n",round((double)(interval) / TIMESLICE));
            if (round((double)(interval) / TIMESLICE) == 2.0) //2*TIMESLICE = '0'
            {
                received_char = received_char << 1;
                bitcount++;
                // printf("0");
                // fflush(stdout);
            }
            else if (round((double)(interval) / TIMESLICE) == 3.0) //3*TIMESLICE = '1'
            {
                received_char = (received_char << 1) + 1;
                bitcount++;
                // printf("1");
                // fflush(stdout);
            }
            if (bitcount == 8)
            {
                // error handling: for every 8 bits if the byte not in range of ASCII and last char is not '\n', return error sign '?'
                if ((received_char < 32 || received_char > 127) && received_char != '\n') 
                { // is not ascii and not '\n'
                    received_str[0] = '?';
                }
                received_str[received_str_index++] = received_char;
                if (received_char == '\n')
                {
                    int i;
                    for (i = 0; i < received_str_index; i++)
                    {
                        printf("%c", received_str[i]); //print character
                    }
                    memset(received_str, 0, 81);
                    received_str[0] = '!';
                    received_str_index = 1;
                    is_busy = 0;
                }
                bitcount = 0;
                received_char = 0;
            }
        }
        is_second_signal = ~is_second_signal;
    }
}

void doubleSigHandler(int signum)
{
    is_busy = 1;
    if (signum == SIGUSR1)
    {
        received_char = received_char << 1; //SIGUSR1 = '0'
        bitcount++;
    }
    if (signum == SIGUSR2)
    {
        received_char = (received_char << 1) + 1; //SIGUSR1 = '1'
        bitcount++;
    }
    if (bitcount == 8)
    {
        if ((received_char < 32 || received_char > 127) && received_char != '\n')
        { //is not ascii and not '\n'
            received_str[0] = '?';
        }
        received_str[received_str_index++] = received_char;
        if (received_char == '\n')
        {
            int i;
            for (i = 0; i < received_str_index; i++)
            {
                printf("%c", received_str[i]);
            }
            memset(received_str, 0, 81);
            received_str[0] = '!';
            received_str_index = 1;
            is_busy = 0;
        }
        bitcount = 0;
        received_char = 0;
    }
}

void sendSignalSingle()
{
    int i = 0, j = 0;
    for (i = 0; i < strlen(send_str); i++)
    {
        for (j = 0; j < 8; j++) //for every bit of every byte
        {
            if (send_time_array[i][j] == -1)
            {
                return;
            }

            if (kill(pid2, SIGUSR1) == -1)
            {
                printf("Error: can't send signal to process %d\n", pid2);
                exit(1);
            }
            usleep(send_time_array[i][j]); //usleep interval of each bit.
            if (kill(pid2, SIGUSR1) == -1)
            {
                printf("Error: can't send signal to process %d\n", pid2);
                exit(1);
            }
            usleep(1);
        }
    }
}

void convertStr2TimeArray()
{
    int i;
    for (i = 0; i < strlen(send_str); i++)
    {
        char ch = send_str[i];
        int j;
        for (j = 7; j >= 0; j--)
        {
            int bit = (ch & (0x01 << j)) == 0 ? 0 : 1;
            if (bit)
            {
                send_time_array[i][7 - j] = TIMESLICE * 3; //3TIMESLICE if bit = '1'
            }
            else
            {
                send_time_array[i][7 - j] = TIMESLICE * 2; //2TIMESLICE if bit = '0'
            }
        }
    }
}

long int getCurrentUsec()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long int now = tv.tv_sec * 1000000 + tv.tv_usec;
    return now;
}