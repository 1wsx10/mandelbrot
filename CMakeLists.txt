cmake_minimum_required (VERSION 2.6)
project (little_engine)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(Curses REQUIRED)

add_executable(engine
	main.c
	draw.c
	keyboard.c
	loop.c
	player.c
	constants.c
	vector.c
	matrixd.c
	complex.c
	mandle.c
)

set(CMAKE_CXX_STANDARD_11)
set(CMAKE_CXX_FLAGS
	"-Wall -pedantic -g"
)
set(CMAKE_C_FLAGS
	"-Wall -pedantic -g"
)

target_link_libraries(engine
	${OPENGL_gl_LIBRARY}
	${GLUT_LIBRARIES}
	GLU
	m
)

include_directories(
	${OPENGL_INCLUDE_DIR}
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
)

