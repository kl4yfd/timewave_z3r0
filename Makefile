all: twz-generator twz-generator-threaded twz-point datapoints-watkins
	
	
twz-generator: twz-generator.o
	@gcc -w -g -O3 twz-generator.o -o twz-generator -lm -lpthread -msse2 -mfpmath=sse -mmmx -march=native
	@printf " + Compilation successful!\n"
	@ ls -l twz-generator
	@echo
	
twz-generator.o: twz-generator.c
	gcc -c twz-generator.c -lm -lpthread -O3 -msse2 -mfpmath=sse -mmmx -march=native
	
	
twz-generator-threaded: twz-generator-threaded.o
	@gcc -w -g -O3 twz-generator-threaded.o -o twz-generator-threaded -lm -lpthread -msse2 -mfpmath=sse -mmmx -march=native
	@printf " + Compilation successful!\n"
	@ ls -l twz-generator-threaded
	@echo
	
twz-generator-threaded.o: twz-generator-threaded.c
	gcc -c twz-generator-threaded.c -lm -lpthread -O3 -msse2 -mfpmath=sse -mmmx -march=native
	
	
	
twz-point: twz-point.o
	@gcc -w -g -O3 twz-point.o -o twz-point -lm -msse2 -mfpmath=sse -mmmx -march=native
	@printf " + Compilation successful!\n"
	@ls -l twz-point
	@echo
	
twz-point.o: twz-point.c
	gcc -c twz-point.c -lm -O3 -msse2 -mfpmath=sse -mmmx -march=native

	
datapoints-watkins: datapoints-watkins.o
	@gcc -w -g -O3 datapoints-watkins.o -o datapoints-watkins -lm -msse2 -mfpmath=sse -mmmx -march=native
	@printf " + Compilation successful!\n"
	@ls -l datapoints-watkins
	@echo
	
datapoints-watkins.o: datapoints-watkins.c
	gcc -c datapoints-watkins.c -lm -O3 -msse2 -mfpmath=sse -mmmx -march=native
	
	

clean:
	rm -rf *.o datapoints-watkins twz-generator twz-generator-threaded twz-point
