all:myqueue Receiver Sender

Receiver: 
	gcc -o Receiver Receiver.c myqueue.o -Wall -g -O0

Sender: 
	gcc -o Sender Sender.c -Wall -g -O0

myqueue: 
	gcc myqueue.c -c -Wall -g -O0

clean:
	rm -f Receiver Sender