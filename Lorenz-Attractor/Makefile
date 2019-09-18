EXE = lorenz
all: $(EXE)


# MinGW
ifeq "$(OS)" "Windows_NT"
SHELL=cmd
CFLG=-O3 -Wall
LIBS=-lglut32cu -lglu32 -lopengl32
CLEAN=del *.exe *.o *.a
else
# OSX
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations
LIBS=-framework GLUT -framework OpenGL
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif

# %:%.c
# 	gcc $(CFLG) -o $@ $^ $(LIBS)


.c.o:
	gcc -c $(CFLG) $<
.cpp.o:
	g++ -c $(CFLG) $<

#  Link
lorenz:lorenz.o
	gcc -O3 -o $@ $^   $(LIBS)
#  Clean
clean:
	$(CLEAN)
