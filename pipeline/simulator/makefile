GCC:= g++
Flag:= -std=c++11

main: main.cpp Error.o Env.o Inst.o Im.o Preg.o Stage.o sim.o
	$(GCC) $(Flag) Stage.o Preg.o Error.o Env.o Inst.o IM.o main.cpp -o pipeline.exe

Error.o: Error.cpp Error.h
	$(GCC) $(Flag) -c Error.cpp -o Error.o

Env.o: Env.cpp Env.h Error.o Preg.o Stage.o
	$(GCC) $(Flag) -c Env.cpp -o Env.o

Inst.o: Instruction.h Instruction.cpp Error.o
	$(GCC) $(Flag) -c Instruction.cpp -o Inst.o

Im.o: IMemory.h IMemory.cpp Inst.o Error.o
	$(GCC) $(Flag) -c IMemory.cpp -o Im.o

Preg.o: PipelineReg.h PipelineReg.cpp Inst.o Error.o
	$(GCC) $(Flag) -c PipelineReg.cpp -o Preg.o

Stage.o: Stage.h Stage.cpp
	$(GCC) $(Flag) -c Stage.cpp -o Stage.o

sim.o: simulate.cpp env.o
	$(GCC) $(Flag) -c simulate.cpp -o Sim.o

run:main
	pipeline.exe

clean:
	del *.o *.exe
