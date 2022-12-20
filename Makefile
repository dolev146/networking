all:myqueue mystack Receiver Sender server 

server:
	gcc -o Receiver Receiver.o myqueue.o mystack.o -Wall 	

Receiver:
	gcc Receiver.c -c -Wall 

Sender:
	gcc -o Sender Sender.c -Wall 

myqueue:
	gcc myqueue.c -c -Wall 

clean:
	rm -f *.o Sender Receiver

mystack:
	gcc mystack.c -c -Wall