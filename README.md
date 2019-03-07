# Readme
Comprehensive instructions on how to execute the files and make your own ROSaria robots listen to you.
This project was adapted for Ubuntu 18.04, previously written for Ubuntu 16.04.

This project was updated 9 February 2019 for adaption to Raspbian Stretch.

# Required Setup'

## Robot Control

### Install ROS
You're going to need ROS to connect to the robot and use the ARIA libraries.

[ROS download link](http://wiki.ros.org/ROS/Installation)

#### For Ubuntu

This requires ROS Melodic for support on Ubuntu 18.04. Do NOT use Kinetic on Ubuntu, it will cause conflicts later down the line when you need to import software packages required for connecting to the robot.

#### For Raspbian

You must install ROS Kinetic manually, although Raspbian is like Debian. Apt-get will not recognize the packages.

### Install ARIA
[Link.](http://wiki.ros.org/ROSARIA/Tutorials/How%20to%20use%20ROSARIA)

ROSaria uses the ARIA libraries to connect to Adept MobileRobots. To install the latest version of ARIA, download it from [here.](http://robots.mobilerobots.com/wiki/Aria).*

#### For Ubuntu
If you are using Ubuntu 16.04 or later, install the package indicated for Ubuntu 16 or later. If you are using a previous version of Ubuntu (12.04 or later), install the package indicated for Ubuntu 12.

If you are running Ubuntu 32-bit OS, make sure you have the 32-bit ARIA package (i386) installed. If you are running Ubuntu 64-bit OS, make sure the 64-bit ARIA package (amd64) is installed. If building rosaria on a 64-bit OS, but the 32-bit libAria.so library is installed, it will not be able to link to libAria.so (resulting in a warning about incompatibly libAria.so, then failure to link)

Install the ARIA package by opening it in the Ubuntu GUI, or use the dpkg tool on the command line with its -i option, for example:

  sudo dpkg -i libaria_2.9.1+ubuntu12_i386.deb
or

  sudo dpkg -i libaria_2.9.1+ubuntu16_i368.deb
etc.

NOTE: Packages indicated for Ubuntu 12.04 can be used on any version of Ubuntu up to 16.04. Ubuntu 16.04 and later includes GCC 5+ in which the C++ standard library ABI has changed. To use an Ubuntu 12.04 ARIA package on Ubuntu 16.04, you must recompile ARIA. After installing the Ubuntu 12 package, enter the ARIA directory and rebuild ARIA:

  cd /usr/local/Aria
  make clean
  make -j4
If an Ubuntu 16 package is available, however, use that instead.

#### For Raspbian
*As of 9 February 2019, robots.mobilerobots.com has gone offline. Instead, download Aria manually from [here](https://github.com/moshulu/aria-legacy). It is recommended to clone the repository in /usr/local/, and use the mv command to rename the cloned library to "Aria". That way, you can follow the tutorials provided without changing Aria to aria-legacy.

You **must** do:
```
sudo make
```
...in /usr/local/Aria to install the library properly. Then, edit the LD_LIBRARY_PATH to include /usr/local/Aria/lib (see below for more instructions about that).

#### Editing the LD_LIBRARY_PATH

For installations on Raspbian, you **must** link the libraries together. Otherwise, when you try to compile the .cpp program with the makefile, it won't recognize the libraries. Here are the steps:

```
echo $LD_LIBRARY_PATH
```

If there's no output, you must add Aria's library path to the machine's default path.

```
LD_LIBRARY_PATH=/usr/local/Aria/lib
```

If you echo the library path variable again, you should see "/usr/local/Aria/lib".

Then, export (publish) the library path so the machine recognizes it.

```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/Aria/lib
```

#### Other Notes
Rosaria's CMake build script detects the location of the ARIA library when catkin runs cmake. If you have already tried building rosaria prior to installing the ARIA library (either via rosdep or manually), you must force catkin to re-run cmake for rosaria with catkin_make --force-cmake. Do this if you get errors while building rosaria such as "could not find such file or directory : Aria/Aria.h" or similar.

## Voice Recognition

### Install CMUSphinx and PocketSphinx
[Link.](http://jrmeyer.github.io/asr/2016/01/09/Installing-CMU-Sphinx-on-Ubuntu.html)

This is a GREAT tutorial on how to install Sphinx, but you don't need to follow all of it. Do only the CMUSphinx portion (this is the base required to run the other libraries of Sphinx) and PocketSphinx (this is the mobile recognition library). It must be noted that you MUST follow all of his instructions exactly, especially adding the libraries to the recognized files (see the "nano /etc/ld.so.conf" command in the tutorial to add the libraries). I had a problem with adding both CMUSphinx and PocketSphinx to the list of recognized libraries, so make sure to put them in the correct directories. Structure of these within your project folder should be

	- parent directory
		|- Other files (your .cpp file to run, makefile, bash file)
		|- sphinx-source (this is the file that should be seen by the libraries, so that pocketsphinx and sphinxbase's source code isn't messed up by what can be seen and what is not seen)
			| - pocketsphinx
			| - sphinxbase

# Compiling a program
### Understanding Makefile
So you've made it this far, great! Now let's see if you can actually run something related to written code!

If you haven't already, I suggest trying to run something from the ARIA/examples directory, to make sure you have the flow of things down. I also highly suggest you read the README.md in the highest level ARIA directory. ARIA is usually stored in the /usr/local/Aria directory, if you haven't saved it somewhere else while you were installing.

If you've done that, you must realize that you need a couple things to run your project. Your
- Makefile (this compiles the program into a bash command)
- Code (this is a .cpp file)
- Bash command (autogenerated after you "make" (compile) your code)

Step 1:
Write your makefile. It's saved as "Makefile" with no file extension. A makefile compiles your code and generates it into a bash command to be run in a terminal, executing your .cpp file. If your code doesn't compile, it tries to tell you what's wrong with your code in order to compile it. Pretty neat, huh?

Aria uses gpp to compile c++ code. Here's what the generic makefile looks like. You might need to change it for the libraries to point in the correct direction:

  ```php
  CFLAGS=-fPIC -g -Wall
  
  SPHINX_LINK=-L/usr/local/lib -lpocketsphinx -lsphinxbase -lsphinxad -llapack -lpthread -lm
  ARIA_LINK=-L/usr/local/Aria/lib -lAria -lpthread -ldl -lrt

  ARIA_INCLUDE=-I/usr/local/Aria/include
  SPHINX_INCLUDE=-I/usr/local/include/sphinxbase
  POCKET_INCLUDE=-I/usr/local/include/pocketsphinx

  %: %.cpp
	  $(CXX) $(CFLAGS) $(ARIA_INCLUDE) $(SPHINX_INCLUDE) $(POCKET_INCLUDE) $< -o $@ $(ARIA_LINK) $(SPHINX_LINK)
```

So what does this all mean?
- CFLAGS defines the flags that compiles this as a bash command
- SPHINX_LINK defines where pocketsphinx and sphinxbase are located (in my case, they're located in the same place)
- ARIA_LINK defines where the ARIA libraries are located
- ARIA_INCLUDE defines where the Aria header files are
- SPHINX_INCLUDE defines where the SphinxBase header files are
- POCKET_INCLUDE defines where the PocketSphinx header files are
- %: %.cpp compiles the command into a bash command. Optionally we have an output file (-o) that we can define at runtime, if we really wanted to.

## Modifying the voice recognition libraries
In order for main.cpp not to parse through 60,000 words, we need to limit the amount of words that Pocketsphinx/Sphinxbase actually recognizes.
1. Go to /usr/local/share/pocketsphinx/model/en-us/
2. Open cmudict-en-us.dict in leafpad
3. Delete all contents (it's okay, I promise)
4. Type in:
```
left L EH F T
right R AY T
backward B AE K W ER D
forward F AO R W ER D
stop s t aa p
mary M EH R IY
```
This limits the library to these words. You can add/delete words as you wish. Just take note: it takes longer to parse through longer lists. Duh.

## Let's do this.
Alright, you're all ready to go. Let's rock and roll. \m/
1. Open a terminal (Ctrl+Alt+t)
2. cd to the directory with your .cpp file and Makefile.
3. Type "make your-file-name.cpp"
..* If this doesn't compile, edit your code/makefile. Repeat this step until it DOES compile. It might help to do "make -d your-file-name.cpp to get a more verbose output. It lists literally what "make" is doing to your code.
4. Type "ls". A bash file should have been created called "your-file-name".
5. Type "./your-file-name" to execute the bash file. This should execute your .cpp file. If you want to make changes to the .cpp file, you must re "make" the file and then execute again.

You're all done! Congrats! All of these steps, and that was just to run something...eep. Good luck!




