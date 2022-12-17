all:myqueue Receiver Sender

Receiver: 
	gcc -o Receiver Receiver.c myqueue.o -Wall

Sender: 
	gcc -o Sender Sender.c -Wall

myqueue: 
	gcc myqueue.c -c -Wall

clean:
	rm -f Receiver Sender