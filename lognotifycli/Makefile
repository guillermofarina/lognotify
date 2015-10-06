#Generic Makefile. You can change CC to specify a different compiler, CFLAGS for a diferent set of compiling flags, LDFLAGS for a different set of linking flags. You should list all source files on SOURCE, and name the executable output file on EXECUTABLE.
#Potentially useful extra warnings: -Wall -Wextra -Wfloat-equal -Wundef -Wcast-align -Wwrite-strings -Wlogical-op -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual

#Compiler settings
CC = g++
INCLUDE = `pkg-config --libs --cflags glib-2.0 libnotify`
CFLAGS = -std=c++11 -Wall -Wextra $(INCLUDE) 
LDFLAGS = $(INCLUDE) -pthread

#Directory tree (relative to the Makefile placement; . for same location, ../foo for another at the same level)
SRCDIR = src
OBJDIR = obj
BINDIR = bin

#Files
SOURCES = lognotifycli.cpp cliente_de_notificaciones.cpp centro_de_notificaciones.cpp servidor.cpp historial.cpp filtro.cpp regla.cpp condicion.cpp
EXECUTABLE = lognotifycli

#File paths
PSOURCES = $(patsubst %,$(SRCDIR)/%,$(SOURCES))
POBJECTS = $(patsubst %,$(OBJDIR)/%,$(SOURCES:.cpp=.o))
PEXEC = $(patsubst %,$(BINDIR)/%,$(EXECUTABLE))

#Old Rules
.PHONY: all
all: $(PEXEC)
	
$(PEXEC): $(POBJECTS) 
	$(CC) $(LDFLAGS) -o $@ $(POBJECTS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -rf $(POBJECTS) $(PEXEC)