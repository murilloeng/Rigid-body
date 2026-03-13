#compiler
CXX = g++
INCS += -I .. -I /usr/include/freetype2
WARS += -Wall -Wno-unused-variable -Wno-unused-result
WARS += -Wno-format-security -Wno-return-type -Wno-unused-function -Wformat-overflow=0
LIBS += -l gmsh -l umfpack -l lapack -l quadrule -l GLEW -l GL -l glut -l freetype -l fftw3 -l glfw
CXXFLAGS += -std=c++20 -fPIC -pipe -fopenmp -MT $@ -MMD -MP -MF $(subst .o,.d, $@) $(DEFS) $(INCS) $(WARS)

#mode
ifneq ($(m), r)
	mode = debug
	CXXFLAGS += -ggdb3
else
	mode = release
	CXXFLAGS += -Ofast
endif

#ouput
out = dist/$(mode)/rigid-body.out

#sources
src := $(sort $(shell find -path './src/*.cpp'))

#objects
obj := $(sort $(subst ./src/, build/$(mode)/, $(addsuffix .o, $(basename $(src)))))

#dependencies
dep := $(subst .o,.d, $(obj))

#rules
all : $(out)
	@echo 'build($(mode)): complete!'

run : $(out)
	@./$(out)

debug : 
	@gdb $(out) -x gdb.txt

math :
	+@cd ../Math && $(MAKE) -f Makefile m=$m

canvas :
	+@cd ../Canvas && $(MAKE) -f Makefile m=$m

$(out) : math canvas $(obj)
	@echo 'executable($(mode)): $@'
	@mkdir -p $(dir $@) && rm -rf $@
	@$(CXX) -fopenmp -o $(out) $(obj) ../Math/Math/dist/$(mode)/libmath.so ../Canvas/Canvas/dist/$(mode)/libcanvas.so $(LIBS)

build/$(mode)/%.o : src/%.cpp build/$(mode)/%.d
	@echo 'compiling($(mode)): $<'
	@mkdir -p $(dir $@) && rm -rf $@
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(dep) : ;

-include $(dep)

clean :
	@rm -rf dist/$(mode)
	@rm -rf build/$(mode)
	@echo 'clean($(mode)): complete!'

print-% :
	@echo $* = $($*)

.PHONY : all run gui fea clean print-%