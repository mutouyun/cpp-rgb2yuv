.PHONY: clean all install

EXE_NAME = $(notdir $(CURDIR)).out
COMPILE_PREFIX ?= 

CFLAGS  = -I.
CFLAGS += -g

LDFLAGS  = 

SRC_DIRS = test

OBJS_C = $(addsuffix .o, $(wildcard $(addsuffix /*.c, $(SRC_DIRS))))
OBJS_CPP = $(addsuffix .o, $(wildcard $(addsuffix /*.cpp, $(SRC_DIRS))))

all: $(EXE_NAME)

$(EXE_NAME): $(OBJS_C) $(OBJS_CPP)
	@$(COMPILE_PREFIX)g++ $^ $(LDFLAGS) -o $(EXE_NAME)
	@echo [LD] $@

$(OBJS_C): %.c.o: %.c
	@$(COMPILE_PREFIX)gcc -c $(CFLAGS) $< -o $@
	@echo [CC] $<

$(OBJS_CPP): %.cpp.o: %.cpp
	@$(COMPILE_PREFIX)g++ -c $(CFLAGS) $< -o $@
	@echo [CXX] $<

clean:
	rm -f $(EXE_NAME)
	rm -f $(OBJS_C) $(OBJS_CPP)
	rm -f *.yuv *.txt