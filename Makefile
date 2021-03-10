CXXFLAGS := -I include -O2 -Wall -Werror -flto -fno-fat-lto-objects -g
LDLIBS := -lGL -lX11

outName := test
objs := $(patsubst src/%.cpp, obj/%.o, $(wildcard src/*.cpp))

${outName}: ${objs}
	${CXX} ${objs} ${CXXFLAGS} ${LDLIBS} -o $@

${objs}: obj/%.o: src/%.cpp
	${CXX} -c ${CXXFLAGS} -o $@ $<

ManagedMatrix_Hdeps := ManagedMatrix main
$(patsubst %, obj/%.o, ${ManagedMatrix_Hdeps}): include/ManagedMatrix.hpp

defs_Hdeps := defs ManagedMatrix main 
$(patsubst %, obj/%.o, ${defs_Hdeps}): include/defs.hpp

GLES_Hdeps := GLExtensionScanner GLWindowManager main
$(patsubst %, obj/%.o, ${GLES_Hdeps}): include/GLExtensionScanner.hpp

GLWM_Hdeps := GLWindowManager main
$(patsubst %, obj/%.o, ${GLWM_Hdeps}): include/GLWindowManager.hpp

CliManager_Hdeps := CliManager main
$(pathsubst %, obj/%.o, ${CliManager_Hdeps}): include/CliManager.hpp

.PHONY: clean

clean:
	rm -rf ${objs} ${outName}
