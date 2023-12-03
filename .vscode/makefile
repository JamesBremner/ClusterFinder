#source file search paths
VPATH = ./src ../raven-set ../KMeans/src ../quadtree/src

	# compiler include search paths
INCS = -I./include \
	-I../windex/include \
	-I../raven-set \
	-I../KMeans/src \
	-I../quadtree/src \
	-I../../code_ext/boost/boost1_78 \

LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs \
	-L../../code_ext/boost/boost1_78/lib \
	-lboost_system-mgw11-mt-x64-1_78 \
	#-mwindows

static_link_flags = -static-libstdc++ -static-libgcc -static -lComctl32

# folders
ODIR = ./.vscode/obj

OPT = -O2
CCflags = -std=c++17 $(OPT) -D_mingw_

# sources
OBJ_starter = \
	$(ODIR)/main.o \
	$(ODIR)/cxy.o \
	$(ODIR)/KMeans.o \
	$(ODIR)/cDataPoint.o \
	$(ODIR)/quadtree.o \
	$(ODIR)/cRunWatch.o \

### COMPILE

$(ODIR)/%.o: %.cpp  
	g++ $< \
	-c -o $@  $(INCS) $(CCflags)

starter: $(OBJ_starter)
	g++ $(OPT) \
	-o bin/starter.exe $^  \
	$(static_link_flags) $(LIBS)

.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o

