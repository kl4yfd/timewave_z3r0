all: twz-generator twz-generator-threaded twz-point
	
	
twz-generator: twz-generator.o
	@gcc -w -g -O3 twz-generator.o -o twz-generator -lm -lpthread -msse2 -mfpmath=sse -mmmx -march=native
	@printf " + Compilation successful!\n"
	@ ls -l --color twz-generator
	@echo
	
twz-generator.o: twz-generator.c
	gcc -c twz-generator.c -lm -lpthread -O3 -msse2 -mfpmath=sse -mmmx -march=native
	
	
twz-generator-threaded: twz-generator-threaded.o
	@gcc -w -g -O3 twz-generator-threaded.o -o twz-generator-threaded -lm -lpthread -msse2 -mfpmath=sse -mmmx -march=native
	@printf " + Compilation successful!\n"
	@ ls -l --color twz-generator-threaded
	@echo
	
twz-generator-threaded.o: twz-generator-threaded.c
	gcc -c twz-generator-threaded.c -lm -lpthread -O3 -msse2 -mfpmath=sse -mmmx -march=native
	
	
	
twz-point: twz-point.o
	@gcc -w -g -O3 twz-point.o -o twz-point -lm -msse2 -mfpmath=sse -mmmx -march=native
	@printf " + Compilation successful!\n"
	@ls -l --color twz-point
	@echo
	
twz-point.o: twz-point.c
	gcc -c twz-point.c -lm -O3 -msse2 -mfpmath=sse -mmmx -march=native
	
	

clean:
	rm -rf *.o twz-generator twz-generator-threaded twz-point
