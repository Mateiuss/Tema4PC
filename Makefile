build: 
	gcc spam_detector.c functions.c -o spam_detector -lm

clean:
	rm spam_detector statistics.out prediction.out logs.txt