#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#define TIMESLICE 20000 //us

//just one signal

struct itimerval new_value, old_value;
char send_str[81] = {0}; //
char send_char = '0';    // is one element of send_str
int send_str_index = -1;  // send_char=send_str[send_str_index],if send_str_index==count-1,than the str finished sending
                            //is also the start and end flag,=-1 means send message start boundry,
                            //=counts means send message end boundry
int send_char_index = 7; // bit=send_char[send_char_index],from 7 to 0,
                         //if send_char_index<0,means the current char has finished sending
int counts = 0;          // count str's char number
int is_busy;             //flag,0:free,1:busy.If is sending message now,you cannot input new message
int last_received_time;  //last received SIGUSR1 time,namely bit 1
long int timeslice;
long int comming_usec[80 * 8]; //record usecond that SIGUSR1 coming
int usec_index = 0;
int flag_send_start, flag_send_end;
int flag_receive_start,flag_receive_end;
int count_send_boundry_bit;
int count_receive_bits;
long int last_receive_usec,current_receive_usec;

int pid2;

void setTimer()
{
    new_value.it_value.tv_sec = 0;
    new_value.it_value.tv_usec = 1;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_usec = TIMESLICE;
    setitimer(ITIMER_REAL, &new_value, &old_value);
}

void stopTimer()
{
    new_value.it_value.tv_sec = 0;
    new_value.it_value.tv_usec = 0;
    new_value.it_interval = new_value.it_value;
    setitimer(ITIMER_REAL, &new_value, &old_value);
}

long int getUsec()
{
    struct timeval start, end;
    gettimeofday(&start, NULL);
    long int now = start.tv_sec * 100000 + start.tv_usec;
    return now;
}

void init_comming_usec()
{
    memset(comming_usec, -1, 80 * 8 * sizeof(int));
}

void handleUsecArray()
{ //convert the timestamp array to bit stream
    int i;
    int bits[80];
    int bits_index = 0;
    long int intervals[usec_index - 1];
    memset(bits, -1, 80);
    memset(intervals, -1, usec_index - 1);
    for (i = 0; i < usec_index; i++)
    {
        long int interval = comming_usec[i + 1] - comming_usec[i];
        //printf("%d\n", interval);
        printf("%d:%ld\n", i, comming_usec[i]);
    }
    for (i = 0; i < usec_index - 1; i++)
    {
        long int interval = comming_usec[i + 1] - comming_usec[i];
        intervals[i] = interval;
        printf("%d~%d:%ld\n", i, i + 1, interval);
    }

    bits[bits_index++] = 1;
    for (i = 0; i < usec_index - 1; i++)
    {
        //printf("in\n");
        int count_bit_0 = round((double)(intervals[i]) / TIMESLICE) - 1;
        printf("count_bit_0:%d\n", count_bit_0);
        int j;
        for (j = 1; j <= count_bit_0; j++)
        {
            bits[bits_index++] = 0;
            // printf("%d:%d",bits_index-1,0);
            // fflush(stdout);
        }
        bits[bits_index++] = 1;
    }

    for (i = 0; i < bits_index; i++)
    {
        printf("%d", bits[i]);
        fflush(stdout);
    }
}

void sendMsgBoundary()
{
    //0xff
    kill(pid2, SIGUSR1);
    count_send_boundry_bit++;
    if (count_send_boundry_bit == 8)
    {
        send_str_index = 0;
        count_send_boundry_bit = 0;
        return;
    }
}

void signalHandler(int signo)
{
    switch (signo)
    {
    case SIGALRM:
        if (send_str_index==-1)
        {
            kill(pid2, SIGUSR1);
            printf("mb1 ");
            fflush(stdout);
            count_send_boundry_bit++;
            if (count_send_boundry_bit == 8)
            {
                send_str_index = 0;
                count_send_boundry_bit = 0;
            }
            return;
        }
        if(send_str_index==counts)
        {
            kill(pid2, SIGUSR1);
            printf("mb2 ");
            fflush(stdout);
            count_send_boundry_bit++;
            if (count_send_boundry_bit == 8){
                send_str_index = 0;
                count_send_boundry_bit = 0;
                stopTimer();
                printf("stop timer\n");
                is_busy = 0;
            }
            return;
        }
        else if (send_char_index < 0)
        {
            send_char = send_str[++send_str_index];
            send_char_index = 7;
        }
        else
        {
            int bit = (send_char & (0x01 << send_char_index)) == 0 ? 0 : 1;
            //printf("bit:%d,count:%d,current_index:%d\n", bit, count, current_index);
            send_char_index--;
            if (bit)
            {
                kill(pid2, SIGUSR1);
            }
            printf("%d", bit);
            fflush(stdout);
        }
        break;
    case SIGUSR1:
        is_busy=1;
        //last_receive_usec=current_receive_usec;
        //current_receive_usec=getUsec();
        comming_usec[usec_index++] = getUsec();
        if (usec_index == 18)
        {

            handleUsecArray();
          //  return;
        }
        break;
    }
}

int main()
{
    // struct sigaction sa;
    // sa.sa_handler = signalHandler;
    // sigemptyset(&sa.sa_mask);
    // sa.sa_flags = 0;
    // sa.sa_flags = SA_RESTART;

    /*if ((signal(SIGUSR1, &signalHandler)) == SIG_ERR) {
        printf("error\n");
        exit(1);
    }*/

    signal(SIGALRM, &signalHandler);
    signal(SIGUSR1, &signalHandler);

    char c;

    printf("Own pid:%d\n", getpid());
    scanf("%d", &pid2);
    getchar(); //remove"\n"

    while (1)
    {
        if (is_busy)
        {
            while (1)
                ;
        }
        memset(send_str, 0, sizeof(char) * 81);
        fgets(send_str, 83, stdin);
        counts = (strlen(send_str) - 1); // * 8; //remove '\n',one char has 8 bit
        printf("counts:%d\n", counts);

        send_char_index = 7;
        send_str_index = -1;
        send_char = send_str[0];

        //flag_send_start = 1;
        setTimer();
        is_busy = 1;
    }
}