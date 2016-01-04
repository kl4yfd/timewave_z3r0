all: twz twz-threaded twz-point


twz: twz.o
	@gcc -w -g -Ofast -Wfatal-errors twz-generator.o -o twz -lm 
	@printf " + Compilation successful!\n"
	@ls -l --color twz
	@echo
	
twz.o: twz-generator.c
	gcc -c twz-generator.c -lm -mfpmath=both
	
	
	
twz-threaded: twz-generator-threaded.o
	@gcc -w -g -Ofast -mno-mmx -Wfatal-errors twz-generator-threaded.o -o twz-threaded -lm -lpthread
	@printf " + Compilation successful!\n"
	@ ls -l --color twz-threaded
	@echo
	
twz-generator-threaded.o: twz-generator-threaded.c
	gcc -c twz-generator-threaded.c -lm -lpthread -Ofast
	
	
	
twz-point: twz-point.o
	@gcc -w -g -Ofast -Wfatal-errors twz-point.o -o twz-point -lm 
	@printf " + Compilation successful!\n"
	@ls -l --color twz-point
	@echo
	
twz-point.o: twz-point.c
	gcc -c twz-point.c -lm -O3
	
	

clean:
	rm -rf *o twz twz-threaded twz-point
