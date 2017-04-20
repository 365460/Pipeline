GCC:= g++
Flag:= -std=c++11

main: main.cpp Error.o Env.o Inst.o Im.o Preg.o Stage.o Sim.o
	$(GCC) $(Flag) Sim.o Stage.o Preg.o Error.o Env.o Inst.o Im.o main.cpp -o pipeline

Error.o: Error.cpp Error.h
	$(GCC) $(Flag) -c Error.cpp -o Error.o

Env.o: Env.cpp Env.h Error.o Preg.o Stage.o Im.o
	$(GCC) $(Flag) -c Env.cpp -o Env.o

Inst.o: Instruction.h Instruction.cpp Error.o
	$(GCC) $(Flag) -c Instruction.cpp -o Inst.o

Im.o: IMemory.h IMemory.cpp Inst.o Error.o
	$(GCC) $(Flag) -c IMemory.cpp -o Im.o

Preg.o: PipelineReg.h PipelineReg.cpp Inst.o Error.o
	$(GCC) $(Flag) -c PipelineReg.cpp -o Preg.o

Stage.o: Stage.h Stage.cpp
	$(GCC) $(Flag) -c Stage.cpp -o Stage.o

Sim.o: simulate.cpp Simulate.h Env.o
	$(GCC) $(Flag) -c simulate.cpp -o Sim.o

run:main
	./pipeline

clean:
	rm *.o *.exe
