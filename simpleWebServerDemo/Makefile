
MAKEFILE      = Makefile

EQ            = =

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_QML_DEBUG
CFLAGS        = -pipe -g -Wall -Wextra -fPIC $(DEFINES)
CXXFLAGS      = -pipe -g -std=gnu++1z -Wall -Wextra -fPIC $(DEFINES)
INCPATH       = -I.
DEL_FILE      = rm -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p
COPY          = cp -f
COPY_FILE     = cp -f
COPY_DIR      = cp -f -R
INSTALL_FILE  = install -m 644 -p
INSTALL_PROGRAM = install -m 755 -p
INSTALL_DIR   = cp -f -R
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
TAR           = tar -cf
COMPRESS      = gzip -9f
LINK          = g++
LFLAGS        = 
LIBS          = $(SUBLIBS) -lpthread -lrt   
AR            = ar cqs
RANLIB        = 
SED           = sed
STRIP         = strip

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = ipc.cpp \
		main.cpp \
		some_busy_job.cpp \
		task_transfer.cpp \
		thread_pool.cpp 
OBJECTS       = ipc.o \
		main.o \
		some_busy_job.o \
		task_transfer.o \
		thread_pool.o
DIST          = ipc.h \
		some_busy_job.h \
		task_transfer.h \
		thread_pool.h ipc.cpp \
		main.cpp \
		some_busy_job.cpp \
		task_transfer.cpp \
		thread_pool.cpp
QMAKE_TARGET  = simpleWebServerDemo
DESTDIR       = 
TARGET        = simpleWebServerDemo


first: all
####### Build rules

simpleWebServerDemo:  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)


all: simpleWebServerDemo

dist: distdir FORCE
	(cd `dirname $(DISTDIR)` && $(TAR) $(DISTNAME).tar $(DISTNAME) && $(COMPRESS) $(DISTNAME).tar) && $(MOVE) `dirname $(DISTDIR)`/$(DISTNAME).tar.gz . && $(DEL_FILE) -r $(DISTDIR)

distdir: FORCE
	@test -d $(DISTDIR) || mkdir -p $(DISTDIR)
	$(COPY_FILE) --parents $(DIST) $(DISTDIR)/


clean: compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


distclean: clean 
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) .qmake.stash
	-$(DEL_FILE) Makefile


####### Sub-libraries

check: first

benchmark: first

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: 

####### Compile

ipc.o: ipc.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ipc.o ipc.cpp

main.o: main.cpp thread_pool.h \
		task_transfer.h \
		ipc.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

some_busy_job.o: some_busy_job.cpp some_busy_job.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o some_busy_job.o some_busy_job.cpp

task_transfer.o: task_transfer.cpp task_transfer.h \
		ipc.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o task_transfer.o task_transfer.cpp

thread_pool.o: thread_pool.cpp thread_pool.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o thread_pool.o thread_pool.cpp

####### Install

install:  FORCE

uninstall:  FORCE

FORCE:

