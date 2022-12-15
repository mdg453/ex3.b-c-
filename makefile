snake: snakes_and_ladders.o markov_chain.o linked_list.o
	gcc snakes_and_ladders.o markov_chain.o linked_list.o -o snakes_and_ladders

tweets: tweets_generator.o markov_chain.o linked_list.o
	gcc  tweets_generator.o markov_chain.o linked_list.o -o tweets_generator

tweets_generator.o: tweets_generator.c markov_chain.o linked_list.o
	gcc -c tweets_generator.c

snakes_and_ladders.o: snakes_and_ladders.c markov_chain.o linked_list.o
	gcc -c  snakes_and_ladders.c

markov_chain.o: markov_chain.c markov_chain.h linked_list.o
	gcc -c markov_chain.c


linked_list.o: linked_list.c linked_list.h
	gcc -c linked_list.c