GCC:= g++
Flag:= -std=c++11

main: main.cpp Error.o Env.o Inst.o IM.o PREG.o
	$(GCC) $(Flag) PREG.o Error.o Env.o Inst.o IM.o main.cpp -o pipeline.exe

Error.o: Error.cpp Error.h
	$(GCC) $(Flag) -c Error.cpp -o Error.o

Env.o: Env.cpp Env.h Error.o PREG.o Stage.o
	$(GCC) $(Flag) -c Env.cpp -o Env.o PREG.o Stage.o

Inst.o: Instruction.h Instruction.cpp Error.o
	$(GCC) $(Flag) -c Instruction.cpp -o Inst.o

IM.o: IMemory.h IMemory.cpp Inst.o Error.o
	$(GCC) $(Flag) -c IMemory.cpp -o IM.o

PREG.o: PipelineReg.h PipelineReg.cpp Inst.o Error.o
	$(GCC) $(Flag) -c PipelineReg.cpp -o PREG.o

Stage.o: Stage.h Stage.cpp
	$(GCC) $(Flag) -c Stage.cpp -o Stage.o

run:main
	pipeline.exe

clean:
	del *.o *.exe
