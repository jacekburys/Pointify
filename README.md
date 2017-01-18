# Pointify
3rd year group project at Imperial

Pointify is an open-source system which allows for real time 3D reconstruction using multiple Kinect v2 sensors simultaneously.

Pointify is based off [LiveScan3D](https://github.com/MarekKowalski/LiveScan3D), but is different since it uses [OpenCV](http://opencv.org/) with [ArUco](https://www.uco.es/investiga/grupos/ava/node/26) for the calibration. Pointify is also cross-platform and works on Linux, Windows and Mac OS X.

An example point cloud obtained from two Kinect V2 sensors using Pointify:

![Merged Point Clouds](/report/images/mergedpointclouds.png)

#How to run
##WebApp
Inside the webapp directory, run `npm install` to install server dependencies. Run `bower install` to install front end dependencies. Run `gulp serve` to run the server and point browser to [localhost](localhost:9000) to see the viewer.

##Kinect Client
Install the appropriate dependencies from [libfreenect2](https://github.com/OpenKinect/libfreenect2). Download platform dependent library files from [here](https://drive.google.com/open?id=0B9ToIasbTYasRjdkQmptUE9URkE) and put them in /kinect\_client/lib/{platform\_name} . Inside the kinect\_client directory run `cmake CMakeLists.txt`. After this generates the OS dependent MakeFile, run `make`.

When this is done simply run `./client --ip={ip address of server}`. Note this may require an elevated `sudo` request on linux.

#Authors
Jacek Burys

Adam Hosier

Kabeer Vohra

Liu Rui

