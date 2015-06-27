# simple_video_server
A simple video server, written in C using linux's v4l2 capture. It also performs few tasks over video. ex- OSD imprint, saving a JPEG snapshot, etc. Please refer docs/ directory for details.

Dependencies
------------
	> This application depends on following libraries-
		1. SDL2 (Simple Directmedia Layer 2) Library
			$ sudo apt-get install libsdl2-dev
		2. JPEG Library
			$ sudo apt-get install libjpeg-dev
	> Above libraries should be installed before compiling app.

Build and Installation
----------------------
	> make
	> make install

	> After this bin/ directory should contain following binaries:
		1. video_server	:	video server application, this will be installed 
							to filesystem /usr/local/bin by default.
		2. cli_app		:	A test application(CLI interface) use to communicate 
							with video server at run-time, APIs are mentioned in
							docs/.
		3. test_player	:	A simple raw video player to test recorded raw video.

	> After successful installation, video_server app can be searched and executed 
	from User's Application list.

Running from command-line
-------------------------
	> All the binaries can also be run from command-line interface.
	> Use '-h' with the binaries to get the usage.

