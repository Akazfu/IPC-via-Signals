# CMPUT379 Assignment 1
## TOPIC: IPC via Signals
## zfu@ualberta.ca
## Group47, I submitted and completed this assignment alone due to the missing partner issue.

### Knowing issue: might crash if stress tested with SINGLE mode,  adjust the TIMESLICE to a higher number if not stable at 5000.
### Instruction:
### 1. make covertsigs.double(for double version) or make covertsigs.single(for single version)
### 2. ./covertsigs to run two seperated processes on two terminals
### 3. run test

Q1 Signals generally can't carry message(str) during transportation.  So the idea for get message from another process is we use binary to store the chars of a message. and we edit the sigaction handler, so that for every signal we recieved, we can get the information of a bit, for every 8 bit we get a byte, and for many bytes, we can get the message.  As for SINGLE mode message delivery, the stratege is try to use the advantage of time functions.  Since we are missing one signal to represent '0' and '1'(What we did with the double mode).  What I did is to setup a TIMESLICE as a unit(5000 in my case), and once the SIGUSR1 is sent, we can count the time interval until the next SIGUSR1 to be recieved, and we divide the time interval with TIMESLICE, if its round to 3, than this bit is a '1', or when its round to 2 this bit is a '0'.  Thus for every 8bits, we can get a byte, and for all the bytes we can convert it back to char[], therefore, we performed IPC with one signal.

Q2 Pretty similar idea with SINGLE signal.  However in DOUBLE mode we have an extra SIGUSR2, so we don't have to deal with time interval, and we use SIGUSR1 to represent '0', SIGUSR2 to represent '1'.  In the double case, use signal to perform IPC is more efficient, since we don't have to wait to distinguish '0' and '1'.

Q3 Technically speaking, both mode does not have a message boundarie.  We can do whatever size of a message we want, if we have enough buffer. However to set the boundaries, I used '\n' at end of each message to confirm it is possibly an end of the text, thus output the char string.

Q4 For SIG errors, I generally put if statement, to report sigaction errors or sig errors.  For situation that we lost signals or signal deliverd in wrong order, so it caused wrong message. My stratege is we check for every 8bits, if the byte is in range of ASCII standard(32~127), if it is not ASCII, we print '?' at the front.

Q5 If we lost a set of bits(some 8x bits), and the rest bits can be recognized in ASCII, than we can not catch the error.
However, this wound be a very unlucky situation.  I hope the signal is more reliable than this.

Q6 I used a int is_busy as a flag during main function. when a process is still recieving message, it can not input message.  So we can avoid interleaving.

Q7 No, the older version used SIGALARM, but we don't need it for now.

Q8 Actually no idea.  However, it looks like if we use SIGALARM we might save some CPU cyles, but as we use usleep directly we can't.