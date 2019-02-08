#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#ifdef SINGLE
int SINGLEMODE = 1;
#else
int SINGLEMODE = 0;
#endif

int bitcount = 0;
char r_ch = 0;
char r_str[80] = {0};
int is_first_letter = 1; //received message flag,0:no,1:yes
int pid2;
struct sigaction sa;
char receive_str[81]={0};
int receive_str_index=1;//receive_str[0]='!'or'?'
int is_busy;


void double_sig_handler(int signum);
void single_mode();
void double_mode();


int main()
{
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_flags = SA_RESTART;

    printf("Own pid:%d\n", getpid());
    scanf("%d", &pid2);
    getchar(); //remove"\n"

    if (SINGLEMODE)
    {
        single_mode();
    }
    else
    {
        double_mode();
    }
}

void double_sig_handler(int signum)
{
    is_busy=1;
    if (signum == SIGUSR1)
    {
        r_ch = r_ch << 1;
        bitcount++;
    }
    if (signum == SIGUSR2)
    {
        r_ch = (r_ch << 1) + 1;
        bitcount++;
    }
    if (bitcount == 8)
    {
        if((r_ch<32||r_ch>127)&&r_ch!='\n'){ //is not ascii and not '\n'
            receive_str[0]='?';
        } 
        receive_str[receive_str_index++]=r_ch;
        if (r_ch == '\n')
        {
            is_first_letter = 1;
            int i;
            for(i=0;i<receive_str_index;i++){
                printf("%c",receive_str[i]);
            }
            memset(receive_str,0,81);
            receive_str[0]='!';
            receive_str_index=1;
            is_busy=0;
        }
        bitcount = 0;
        r_ch = 0;
    }
}

void single_mode()
{
}

void double_mode()
{
    sa.sa_handler = double_sig_handler;
    if ((sigaction(SIGUSR1, &sa, NULL) < 0)||
        (sigaction(SIGUSR2,&sa,NULL)<0)) {
		printf("Sigaction error!\n");
		exit(1);
	}

    char c;

    receive_str[0]='!';
    while (1)
    {
        if(is_busy){
            while(1);
        }
        char send_str[81] = {0};
        fgets(send_str, 83, stdin);
        if (send_str[0] == '.')
        {
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
                bit = c & (0x01 << i);
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