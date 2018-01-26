CXXFLAGS := -I include -O2 -Wall -Werror -flto -fno-fat-lto-objects -g
LDLIBS := -lGL -lX11

outName := test
objs := $(patsubst src/%.cpp, obj/%.o, $(wildcard src/*.cpp))

${outName}: ${objs}
	${CXX} ${CXXFLAGS} ${LDLIBS} -o $@ ${objs}

${objs}: obj/%.o: src/%.cpp
	${CXX} -c ${CXXFLAGS} -o $@ $<

GLWM_Hdeps := GLWindowManager main
$(patsubst %, obj/%.o, ${GLWM_Hdeps}): include/GLWindowManager.hpp

.PHONY: clean

clean:
	rm -rf ${objs} ${outName}
