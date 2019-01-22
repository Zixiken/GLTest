CXXFLAGS := -I include -O2 -Wall -Werror -flto -fno-fat-lto-objects -g
LDLIBS := -lGL -lX11

outName := test
objs := $(patsubst src/%.cpp, obj/%.o, $(wildcard src/*.cpp))

${outName}: ${objs}
	${CXX} ${CXXFLAGS} ${LDLIBS} -o $@ ${objs}

${objs}: obj/%.o: src/%.cpp
	${CXX} -c ${CXXFLAGS} -o $@ $<

MatrixFactory_Hdeps := MatrixFactory main
$(patsubst %, obj/%.o, ${defs_Hdeps}): include/MatrixFactory.hpp

defs_Hdeps := defs MatrixFactory main 
$(patsubst %, obj/%.o, ${defs_Hdeps}): include/defs.hpp

GLES_Hdeps := GLExtensionScanner GLWindowManager 
$(patsubst %, obj/%.o, ${GLES_Hdeps}): include/GLExtensionScanner.hpp

GLWM_Hdeps := GLWindowManager main
$(patsubst %, obj/%.o, ${GLWM_Hdeps}): include/GLWindowManager.hpp

.PHONY: clean

clean:
	rm -rf ${objs} ${outName}
