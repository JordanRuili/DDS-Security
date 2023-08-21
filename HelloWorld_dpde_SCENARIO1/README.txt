
=====================
C Hello World w/ Dynamic Discovery Example
=====================

An example publication and subscription pair to send and receive simple strings.
Discovery of endpoints is done with the dynamic-endpoint discovery.

Purpose
=======

This example shows how to perform basic publish-subscribe communication.

This example differs from the HelloWorld example in that endpoint discovery is 
done dynamically: state of remote endpoints are propagated automatically by 
built-in discovery endpoints, and the user does not need to manually configure 
remote endpoint state.

For convenience, complete source code, a sample makefile for Linux and VxWorks, 
and a sample project for Windows has been provided.


Source Overview
===============

A simple "HelloWorld" type, containing a message string, is defined in 
HelloWorld.idl.

For the type to be useable by Connext Micro, type-support files must be 
generated that implement a type-plugin interface.  The example Makefile 
will generate these support files, by invoking rtiddsgen.  Note that rtiddsgen
can be invoked manually, with an example command like this:

    ${RTIMEHOME}/rtiddsgen/scripts/rtiddsgen -micro -language C HelloWorld.idl

The generated source files are HelloWorld.c, HelloWorldSupport.c, and 
HelloWorldPlugin.c.  Associated header files are also generated.
 
The DataWriter and DataReader of the type are managed in HelloWorld_publisher.c 
and HelloWorld_subscriber.c, respectively.  The DomainParticipant of each is 
managed in HelloWorldApplication.c  
  


Example Files Overview
======================

HelloWorldApplication.c:
This file contains the logic for creating an application.  This includes steps 
for configuring discovery and creating a DomainParticipant.  This file also 
includes code for registering a type with the DomainParticipant.

HelloWorld_publisher.c:
This file contains the logic for creating a Publisher and a DataWriter, and 
sending data.  

HelloWorld_subscriber.c:
This file contains the logic for creating a Subscriber and a DataReader, a 
DataReaderListener, and listening for data.

HelloWorldPlugin.c:
This file creates the plugin for the HelloWorld data type.  This file contains 
the code for serializing and deserializing the HelloWorld type, creating, 
copying, printing and deleting the HelloWorld type, determining the size of the 
serialized type, and handling hashing a key, and creating the plug-in.

HelloWorldSupport.c
This file defines the HelloWorld type and its typed DataWriter, DataReader, and 
Sequence.

HelloWorld.c
This file contains the APIs for managing the HelloWorld type.




How to Compile and Run
======================

-------------------
Compiling for Linux
-------------------
Before compiling, make sure that the desired version of compiler and linker
is in your PATH environment variable.

Running rtiddsgen requires the JREHOME environment variable to be defined, 
as the path to a Java Runtime Environment (minimum version 1.7). For example:

    setenv JREHOME /usr/java/jdk1.7.0_04

If compiling on a supported platform, the architecture will automatically be 
determined, so just run:

    gmake

Otherwise, make sure the environment variable RTIMEHOME is defined and that it
points to Connext Micro installation directory before compiling the example.
You can then compile the example:
  
    gmake RTIMEARCH=i86Linux2.6gcc4.4.5

Replace i86Linux2.6gcc4.4.5 with the specific architecture you wish to compile.

-----------------
Running for Linux
-----------------
Run the subscriber by typing:

objs/i86Linux2.6gcc4.4.5/HelloWorld_subscriber -domain <Domain_ID> -peer <address> -sleep <sleep_time> -count <seconds_to_run>

and run the publisher by typing:

objs/i86Linux2.6gcc4.4.5/HelloWorld_publisher -domain <Domain_ID> -peer <address> -sleep <sleep_time> -count <seconds_to_run>


---------------------
Compiling for Windows
---------------------
Before compiling, set environment variables RTIMEARCH to i86Win32VS2010, and
RTIMEHOME to the Connext Micro installation directory. 

Open the Visual Studio 2010 solution (Hello-i86Win32VS2010.sln).
 
Select the desired build configuration (Debug, Debug DLL, Release, Release DLL).

Build the solution, or build the HelloPublisher and HelloSubscriber projects 
individually.  
 
 
-------------------
Running for Windows
-------------------
If you compiled a project using a non-statically linked configuration (Debug
DLL or Release DLL), you will need to first add the Connext Micro libraries to
the PATH environmental variable to be able to run the examples:

set PATH=%PATH%;%RTIMEHOME%\lib\%RTIMEARCH%

From a command prompt, run the subscriber by typing

objs\i86Win32VS2010\HelloSubscriber.exe -domain <Domain_ID> -peer <address> -sleep <sleep_time> -count <seconds_to_run>

and run the publisher by typing:

objs\i86Win32VS2010\HelloPublisher.exe -domain <Domain_ID> -peer <address> -sleep <sleep_time> -count <seconds_to_run>


---------------------
Compiling for VxWorks
---------------------
Before compiling, make sure that the environment variables WIND_HOME, WIND_BASE, 
and WIND_USR are set, and that the desired version of compiler and linker
is in your PATH environment variable.

Change the arguments in publisher_main in HelloWorld_publisher.c and subscriber_main in HelloWorld_subscriber.c to your liking.

Make sure RTIMEARCH is specified in the command line. The default makefile supports host platforms of Windows and Solaris.  

gmake RTIMEARCH=ppc604Vx5.5gcc


---------------------------------
Running for VxWorks (kernel-mode)
---------------------------------
To run the publisher for VxWorks, kernel-mode:

ld < <working directory>/objs/pentiumVx6.9gcc4.3.3/HelloWorld_publisher
taskSpawn "pub", 255, 0x8, 300000, publisher_main

To run the subscriber:

ld < <working directory>/objs/pentiumVx6.9gcc4.3.3/HelloWorld_subscriber
taskSpawn "sub", 255, 0x8, 300000, subscriber_main


------------------------------
Running for VxWorks (RTP-mode)
------------------------------
To run the publisher for VxWorks, RTP-mode:

cd "<your_working directory>/objs/pentiumVx6.9gcc4.3.3_rtp"
cmd rtp exec -p 100 -u 150000 HelloWorld_publisher -- -domain <your_domain> -peer <your_peer>

To run the subscriber:

cd "<your_working directory>/objs/pentiumVx6.9gcc4.3.3_rtp"
cmd rtp exec -p 100 -u 150000 HelloWorld_subscriber -- -domain <your_domain> -peer <your_peer>

