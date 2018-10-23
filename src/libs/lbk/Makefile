DEBUG_OPTIONS = -g -Wall
OPTIMIZE = -fPIC -O3

SINGLE_C_FILE_PROGRAMS =
SINGLE_CXX_FILE_PROGRAMS =
SINGLE_FILE_PROGRAMS = $(SINGLE_C_FILE_PROGRAMS) $(SINGLE_CXX_FILE_PROGRAMS)

TARGET_1 = liblbk.so
TARGET_2 =
TARGET_3 =

ADD_ONS_DIRS = \
	npi-oled-hat \
	vpd

OBJECTS_1 = \
	add-ons/LBPanelDevice.o \
	LBView.o \
	LBPageView.o \
	LBApplication.o \
	LBIconData.o \
	LBScopeItem.o \
	LBScopeHandler.o \
	LBAlertView.o \
	LBListItem.o \
	LBListView.o \
	LBMenuItem.o \
	LBMenuView.o

OBJECTS_2 =
OBJECTS_3 =

TARGET = $(TARGET_1) $(TARGET_2) $(TARGET_3)
OBJECTS = $(OBJECTS_1) $(OBJECTS_2) $(OBJECTS_3)

CC = ${CROSS_COMPILE}gcc
CXX = ${CROSS_COMPILE}g++
LD = ${CROSS_COMPILE}ld
CFLAGS = $(DEBUG_OPTIONS) $(OPTIMIZE) $(shell lite-beapi-config --cflags) -I../../include -I../include
CXXFLAGS = $(DEBUG_OPTIONS) $(OPTIMIZE) $(shell lite-beapi-config --cflags) -I../../include -I../include
LDFLAGS =  $(shell lite-beapi-config --libs)

all: targets addons $(TARGET)

$(C_PROGRAM): $(C_PROGRAM).o
	$(CC) $(DEBUG_OPTIONS) $(OPTIMIZE) $^ -o $@ $(LDFLAGS)

$(CXX_PROGRAM): $(CXX_PROGRAM).o
	$(CXX) $(DEBUG_OPTIONS) $(OPTIMIZE) $^ -o $@ $(LDFLAGS)

targets:
	for f in $(SINGLE_C_FILE_PROGRAMS) .none; do \
		(test $$f = .none || CC=$(CC) $(MAKE) $(MAKEFLAGS) C_PROGRAM=$$f $$f) \
	done
	for f in $(SINGLE_CXX_FILE_PROGRAMS) .none; do \
		(test $$f = .none || CXX=$(CXX) $(MAKE) $(MAKEFLAGS) CXX_PROGRAM=$$f $$f) \
	done

addons:
	for f in $(ADD_ONS_DIRS) .none; do \
		(test $$f = .none || CC=$(CC) $(MAKE) $(MAKEFLAGS) -C add-ons/$$f) \
	done

$(TARGET_1): $(OBJECTS_1)
	$(CXX) $(DEBUG_OPTIONS) $(OPTIMIZE) -shared $^ -o $@ $(LDFLAGS)

$(TARGET_2): $(OBJECTS_2)
	$(CXX) $(DEBUG_OPTIONS) $(OPTIMIZE) $^ -o $@ $(LDFLAGS)

$(TARGET_3): $(OBJECTS_3)
	$(CXX) $(DEBUG_OPTIONS) $(OPTIMIZE) $^ -o $@ $(LDFLAGS)

clean:
	for f in $(ADD_ONS_DIRS) .none; do \
		(test $$f = .none || $(MAKE) $(MAKEFLAGS) clean -C add-ons/$$f) \
	done
	for f in $(TARGET) $(OBJECTS) .none; do \
		(test $$f = .none || test ! -e "$$f" || rm -f $$f); \
	done
	for f in $(SINGLE_FILE_PROGRAMS) $(TARGET) .none; do \
		(test $$f = .none || test ! -e "$$f" || rm -f $$f); \
		(test $$f = .none || test ! -e "$$f.o" || rm -f $$f.o); \
		(test $$f = .none || test ! -e "$$f.obj" || rm -f $$f.obj); \
		(test $$f = .none || test ! -e "$$f.exe" || rm -f $$f.exe); \
		(test $$f = .none || test ! -e "$$f.exe.stackdump" || rm -f $$f.exe.stackdump); \
		(test $$f = .none || test ! -e "$$f.exe.so" || rm -f $$f.exe.so); \
	done

.SUFFIXES: .c .cpp .cxx .cc .o
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

.cxx.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

.cc.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

