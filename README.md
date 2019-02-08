# CMPUT379A1
Signal IPC

I submitted and completed this assignment alone.  Moreover I would like to apply an extension on due day for one-week as the Prof mentioned.

Q1 This is a TODO.  However the idea might be use gettime,  since we just have SIGUSR1 we can assign '0' and '1' depends on wether endtime.tv_usec - start.tv_usec >= 60 or <=60, and do the same thing with two signals for rest implementation.

Q2 Basically I try to use SIGUSR1 as a '0' and SIGUSR2 as a '1' to represent a bit, and use each 8bit signals received to form a byte of binary code which can be converted to the corresponding character, and thus send and recieve the message between process.

Q3 Set to 80 char for now to test the efficiency.

Q4 Make error checks by using SIG_ERR etc, those errors can be catched.

Q5 Lost character info during sending, since signals are not reliable.

Q6 Just give it a bit of time to completely process, or I can try make respond signal once a process recieved the sent message.

Q7 might be SIGINT? since I use it default for terminal interrupt.

Q8 I'm not sure yet.