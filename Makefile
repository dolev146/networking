all: Receiver Sender

Receiver: 
	gcc -o Receiver Receiver.c -Wall

Sender: 
	gcc -o Sender Sender.c -Wall

clean:
	rm -f Receiver Sender