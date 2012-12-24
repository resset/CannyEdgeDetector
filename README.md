CannyEdgeDetector
=================

Disclaimer
----------

This is my student project. Some parts of code may be influenced by various
examples found on internet. It is licensed freely on GNU General Public License
v2: http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

As this project was written some time ago, I may not remember nor recognize
sources I have been working with. As I recall, most if not all of the code
was written from scratch. I you find here recognizable part of your work, please
contact me either through GitHub or by e-mail which you will find in sources.

About
-----

This is (very) naive implementation of Canny Edge Detection algorithm
(http://en.wikipedia.org/wiki/Canny_edge_detector). All this code was written
some time ago, long forgotten and only recently found. I made few changes,
translated comments to English, cleaned up code a bit and that's it. Don't
expect brilliant programming techniques, efficiency and resource savings.
It compiles and have not crashed for some time now. This is all you get.
Probably the most urgent changes should be those dealing with memory
consumption. However, am not eager to change this, at least at the moment.

Application consist of two C++ classes, EdgeApp and CannyEdgeDetector.
First is just an example app written with use of wxWidgets. It displays simple
window with preview of loaded image and after clicking on button, another window
with algorithm result. It uses CannyEdgeDetector by putting into it an RGB
array of image pixels. EdgeApp acts as a wrapper, it can be easily dropped.

CannyEdgeDetector simply performs all steps of algorithm in one method,
ProcessImage. Although it is written in C++ it does not require an fancy
libraries, only one used is math.h for atan2(). Thus, it should be easily
portable to any platform and language.

Simple Makefile allows to quickly build application in any Unix with GCC and
wxWidgets installed.
