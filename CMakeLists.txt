cmake_minimum_required (VERSION 2.6)
project (little_engine)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(Curses REQUIRED)

add_executable(engine main.c)

target_link_libraries(engine
	${CURSES_LIBRARIES}
	${OPENGL_gl_LIBRARY}
	${GLUT_LIBRARIES}
	m
)

include_directories(
	${CURSES_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIR}
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
)

