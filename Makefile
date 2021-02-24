# Project: AM71113363
# Makefile for Explorer.exe XP-style


CC   = gcc.exe -s
WINDRES = windres.exe
RES  = main.res
OBJ  = WinMain.o main.o menubar.o toolbar.o addressbar.o statusbar.o treeview.o listview.o operations.o $(RES)
LINKOBJ  = WinMain.o main.o menubar.o toolbar.o addressbar.o statusbar.o treeview.o listview.o operations.o $(RES)
LIBS =  -L"C:/Dev-Cpp/lib" -mwindows -lcomctl32  
INCS =  -I"C:/Dev-Cpp/include" 
BIN  = ExplorerXp.exe
CFLAGS = $(INCS)   -fshort-double
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before ExplorerXp.exe all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o "ExplorerXp.exe" $(LIBS)

WinMain.o: WinMain.c
	$(CC) -c WinMain.c -o WinMain.o $(CFLAGS)

main.o: main.c
	$(CC) -c main.c -o main.o $(CFLAGS)

menubar.o: menubar.c
	$(CC) -c menubar.c -o menubar.o $(CFLAGS)

toolbar.o: toolbar.c
	$(CC) -c toolbar.c -o toolbar.o $(CFLAGS)

addressbar.o: addressbar.c
	$(CC) -c addressbar.c -o addressbar.o $(CFLAGS)

statusbar.o: statusbar.c
	$(CC) -c statusbar.c -o statusbar.o $(CFLAGS)

treeview.o: treeview.c
	$(CC) -c treeview.c -o treeview.o $(CFLAGS)

listview.o: listview.c
	$(CC) -c listview.c -o listview.o $(CFLAGS)

operations.o: operations.c
	$(CC) -c operations.c -o operations.o $(CFLAGS)

main.res: WinMain.rc 
	$(WINDRES) -i WinMain.rc --input-format=rc -o main.res -O coff 
