GCC:= g++
Flag:= -std=c++11

main: main.cpp Error.o Env.o Inst.o IM.o
	$(GCC) $(Flag) Error.o Env.o Inst.o IM.o main.cpp -o single_cycle.exe

Error.o: Error.cpp Error.h
	$(GCC) $(Flag) -c Error.cpp -o Error.o

Env.o: Env.cpp Env.h Error.o
	$(GCC) $(Flag) -c Env.cpp -o Env.o

Inst.o: Instruction.h Instruction.cpp Env.o Error.o
	$(GCC) $(Flag) -c Instruction.cpp -o Inst.o

IM.o: IMemory.h IMemory.cpp Inst.o Error.o
	$(GCC) $(Flag) -c IMemory.cpp -o IM.o


run:main
	single_cycle.exe

clean:
	rm *.o
