all: twz-generator twz-generator-threaded twz-point
	
	
twz-generator: twz-generator.o
	@gcc -w -g -Ofast twz-generator.o -o twz-generator -lm -lpthread
	@printf " + Compilation successful!\n"
	@ ls -l --color twz-generator
	@echo
	
twz-generator.o: twz-generator.c
	gcc -c twz-generator.c -lm -lpthread -Ofast
	
	
twz-generator-threaded: twz-generator-threaded.o
	@gcc -w -g -Ofast twz-generator-threaded.o -o twz-generator-threaded -lm -lpthread
	@printf " + Compilation successful!\n"
	@ ls -l --color twz-generator-threaded
	@echo
	
twz-generator-threaded.o: twz-generator-threaded.c
	gcc -c twz-generator-threaded.c -lm -lpthread -Ofast
	
	
	
twz-point: twz-point.o
	@gcc -w -g -Ofast twz-point.o -o twz-point -lm 
	@printf " + Compilation successful!\n"
	@ls -l --color twz-point
	@echo
	
twz-point.o: twz-point.c
	gcc -c twz-point.c -lm -Ofast
	
	

clean:
	rm -rf *.o twz-generator twz-generator-threaded twz-point
