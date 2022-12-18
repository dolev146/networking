all:myqueue Receiver Sender

Receiver: 
	gcc -o Receiver Receiver.c myqueue.o -Wall -g

Sender: 
	gcc -o Sender Sender.c -Wall -g

myqueue: 
	gcc myqueue.c -c -Wall -g

clean:
	rm -f Receiver Sender