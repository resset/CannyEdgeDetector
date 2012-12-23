all:
	g++ EdgeApp.cpp CannyEdgeDetector.cpp `wx-config --libs` `wx-config --cxxflags` -Wall -Wextra -o EdgeApp

