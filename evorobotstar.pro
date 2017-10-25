######################################
# Evorobot* makefile #################
######################################

TEMPLATE = app
TARGET = evorobotstar 
DEPENDPATH += ./source_files ./header_files ./resources
INCLUDEPATH += ./source_files ./header_files ./resources
DESTDIR += "../bin"

#######################################
#comment the next 4 lines 
#if you do not need the 3Dgraphics
#or you did not installed QGLViewer yet
#######################################
INCLUDEPATH += ../QGLViewer
CONFIG += qt opengl 
LIBS *= -L../QGLViewer -lQGLViewer2
QT+= opengl \
	 xml

# Input
HEADERS += mode.h \
           public.h \
           epuck.h \
           ncontroller.h \
           evolution.h \
		   environment.h \
           mainwindow.h \
           parameters.h \
           rend_param.h \
           rend_controller.h \
           rend_epuck.h \
           rend_robot3d.h \
	       rend_evolution.h \
           epuck_io.h \
		   mesh.h \
		   musclepair.h
SOURCES += evolution.cpp \
           mainwindow.cpp \
           ncontroller.cpp \
		   environment.cpp \
           gcontroller.cpp \
           parameters.cpp \
           rend_param.cpp \
           rend_controller.cpp \
           rend_epuck.cpp \
           rend_robot3d.cpp \
	       rend_evolution.cpp \
           epuck.cpp \
           epuck_sm.cpp \
           utility.cpp \
           epuck_io.cpp \
	       mesh.cpp \
		   musclepair.cpp

RESOURCES += application.qrc
