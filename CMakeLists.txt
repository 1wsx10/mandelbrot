cmake_minimum_required (VERSION 2.6)
project (mandlebrot)

#find_package(OpenGL REQUIRED)
#find_package(GLUT REQUIRED)
#find_package(Curses REQUIRED)

add_executable(draw
	main.c
	constants.c
	vector.c
	matrixd.c
	complex.c
	mandle.c
	lib/write_screen/drawlib.c
)

set(CMAKE_CXX_STANDARD_11)
set(CMAKE_CXX_FLAGS
	"-Wall -pedantic -g"
)
set(CMAKE_C_FLAGS
	"-Wall -pedantic -g"
)

target_link_libraries(draw
	#${OPENGL_gl_LIBRARY}
	#${GLUT_LIBRARIES}
	#GLU
	m
)

include_directories(
	#${OPENGL_INCLUDE_DIR}
	write_screen
)

set(ALL_LIBS
	#${OPENGL_LIBRARY}
)

