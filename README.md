# Pointify
3rd year group project at Imperial

Pointify is an open-source system which allows for real time 3D reconstruction using multiple Kinect v2 sensors simultaneously.

Pointify is based off LiveScan3D, taken from [LiveScan3D](https://github.com/MarekKowalski/LiveScan3D)

Pointify is different to LiveScan3D since it uses OpenCV with ArUco for the calibration as well as using cross-platform libraries so works on Linux, Windows and Mac OS X.

Example of point cloud obtained from two Kinect V2 sensors:

![Merged Point Clouds](/report/images/mergedpointclouds.png)

#How to run
##WebApp
Inside the webapp directory, run `npm install` to install server dependencies. Run `bower install` to install front end dependencies. Run `gulp serve` to run the server and point browser to [localhost](localhost:9000) to see the viewer.

##Kinect Client
Install the appropriate dependencies from [libfreenect2](https://github.com/OpenKinect/libfreenect2). Download platform dependent library files from ... and put them in /kinect\_client/lib/{platform\_name} . Inside the kinect\_client directory run `cmake CMakeLists.txt`. After this generates the OS dependent MakeFile, run `make`.


#Authors
Jacek Burys
Adam Hosier
Kabeer Vohra
Liu Rui

