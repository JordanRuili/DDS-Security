
=======================================
 C Hello World w/ DDS Security Example
=======================================

An example publication and subscription pair to send and receive simple strings.
Discovery of endpoints is done with the dynamic-endpoint discovery, and each
DomainParticipant uses the DDS Security extensions to secure communications.

 Purpose
==========

This example shows how to perform basic publish-subscribe communication, and 
how to enable DDS Security extensions in a DomainParticipant.

This example differs from the HelloWorld_dpde example in that a set of security 
plugins in installed on each DomainParticipant in order to:
  - perform mutual authentication during the discovery phase
  - enforce access control rules on Domain and Topics
  - encrypt exchanged RTPS communication

For convenience, complete source code, a sample makefile for Linux/Unix, and
a sample Visual Studio project for Windows have been provided.

 Source Overview
=================

A simple "HelloWorld" type, containing a message string, is defined in 
HelloWorld.idl.

For the type to be useable by RTI Connext DDS Micro, type-support files must be
generated that implement a type-plugin interface.  The example Makefile 
will generate these support files, by invoking rtiddsgen.  Note that rtiddsgen
can be invoked manually, with an example command like this:

  ${RTIMEHOME}/rtiddsgen/scripts/rtiddsgen -micro -language C HelloWorld.idl

The generated source files are HelloWorld.c, HelloWorldSupport.c, and 
HelloWorldPlugin.c.  Associated header files are also generated.
 
The DataWriter and DataReader of the type are managed in HelloWorld_publisher.c 
and HelloWorld_subscriber.c, respectively.  The DomainParticipant of each is 
managed in HelloWorldApplication.c.  

 Security Overview
===================

The DDS Security extensions are configured on each DomainParticipant via 
additional settings in the DomainParticipant's QoS.

All source code changes required to enable DDS Security can be found in
HelloWorldApplication.c

All additional configuration and cryptographic material required by DDS 
Security can be found in the "security/" directory.

This directory contains a makefile to regenerate keys, certificates, and 
signed XML documents on Unix platforms.

Please refer to the DDS Security user manual in the online documentation for
additional information on how to manually regenerate these files.

 Example Files Overview
========================

HelloWorldApplication.c:
------------------------
This file contains the logic for creating an application. This includes steps 
for configuring discovery and creating a DomainParticipant. This file has been
modified to register the security plugins and configure the DomainParticipant
to use DDS Security.

HelloWorld_publisher.c:
-----------------------
This file contains the logic for creating a Publisher and a DataWriter, and 
sending data.  

HelloWorld_subscriber.c:
------------------------
This file contains the logic for creating a Subscriber and a DataReader, a 
DataReaderListener, and listening for data.

HelloWorldPlugin.c:
-------------------
This file creates the plugin for the HelloWorld data type.  This file contains 
the code for serializing and deserializing the HelloWorld type, creating, 
copying, printing and deleting the HelloWorld type, determining the size of the 
serialized type, and handling hashing a key, and creating the plug-in.

HelloWorldSupport.c
-------------------
This file defines the HelloWorld type and its typed DataWriter, DataReader, and 
Sequence.

HelloWorld.c
------------
This file contains the APIs for managing the HelloWorld type.

security/:
----------
Directory containing cryptographic material and XML configuration required
by DDS Security.

 How to Compile and Run
========================

----------------------
 Compiling using make
----------------------
Before compiling, make sure that the desired version of compiler and linker
is in your PATH environment variable, and that the following environment 
variable are properly configured:

  - RTIMEHOME: installation of RTI Connext DDS Micro
  
  - RTI_OPENSSLHOME or OPENSSLHOME: these variables are used to configure the
    OpenSSL libraries used to build the example. These libraries should match
    those used to build RTI Connext DDS Micro.

  - SYSLIBXML2 and LIBXML2HOME (optional): if you the RTI Connext DDS Micro
    libraries were built using an external libxml2 version than the one shipped
    with the product, SYSLIBXML2 must be set to properly link the external
    library, and LIBXML2HOME can be used to inform the build system on where
    to find it (if not located in a system path)

The example can then be compiled using the default make target, and by 
specifying the target build architecture using variable RTIMEARCH, e.g.:
  
    make RTIMEARCH=i86Linux2.6gcc4.4.5

By default, all binaries will be linked statically and built in release mode.
If you prefer to build debug libraries, or link libraries dynamically, set 
the DEBUG and SHARED variables when invoking make, e.g.:

    make DEBUG=y SHARED=enabled
    
You can use any non-empty string to enable them.

---------------------------
 Running from a Unix shell
---------------------------
Given your RTIMEARCH of choice, run the subscriber by typing:

objs/${RTIMEARCH}/HelloWorld_subscriber

Similary, you can run the publisher by typing:

objs/${RTIMEARCH}/HelloWorld_publisher

If you built dynamically linked libraries, make sure to include the following
paths in your dynamic linker path (e.g. LD_LIBRARY_PATH on Linux, or
DYLD_LIBRARY_PATH on Darwin):

  - ${RTIMEHOME}/lib/${RTIMEARCH} for RTI Connext DDS Micro libraries
  
  - ${RTI_OPENSSLHOME}/${RTIMEARCH}/debug/lib or
    ${RTI_OPENSSLHOME}/${RTIMEARCH}/release/lib or
    ${OPENSSLHOME}/lib for OpenSSL libraries.

-----------------------
 Compiling for Windows
-----------------------
Before compiling, set the following environment variables:

  - RTIMEARCH to i86Win32VS2010
  
  - RTIMEHOME to the RTI Connext DDS Micro installation directory.
  
  - OPENSSLHOME to a directory containing OpenSSL header files and libraries. 

Open the Visual Studio 2010 solution (Hello-i86Win32VS2010.sln).
 
Select the desired build configuration (Debug, Debug DLL, Release, Release DLL).

Build the solution, or build the HelloPublisher and HelloSubscriber projects 
individually.

---------------------
 Running for Windows
---------------------
If you compiled a project using a non-statically linked configuration (Debug
DLL or Release DLL), you will need to first add the RTI Connext DDS Micro 
and OpenSSL libraries to the PATH environmental variable to be able to run the
examples. Note that often the dll files for OpenSSL are stored in a bin\ directory,
rather than lib\, thus both are added to the PATH (but only the directory actually
containing the dll's is required):

set PATH=%RTIMEHOME%\lib\%RTIMEARCH%;%OPENSSLHOME%\lib:%OPENSSLHOME%\bin;%PATH%

From a command prompt, run the subscriber by typing

objs\%RTIMEARCH%\HelloSubscriber.exe

and run the publisher by typing:

objs\%RTIMEARCH%\HelloPublisher.exe


How to (re-)generate cryptographic material
===========================================

The cryptographic material required to run the example is stored
in the "security/" directory.

A single Certificate Authority (CA) is used as both Identity and Permissions
CA for the DDS DomainParticipant created by each application.

This CA is stored in the "security/ca/" directory and it is used to generate
identity certificates, and to sign XML configuration files.

Two "identities" are generated and stored in the "security/ca/certs/" directory.

Each identity includes a private key, a certificates, and a permissions file.

Plain and signed XML configuration files are stored in directory "security/xml/".

On Unix systems, generation is automated using make.

On Windows systems, the Visual Studio projects will perform 
custom build steps to generate all required files.

------------------
Generating on Unix
------------------

Generation of cryptographic material is automatically triggered by
the top-level Makefile when building the example.

Makefile "security/Makefile" offers the following targets:

  - all (default)
    - create directory "security/ca/"
    - initialize a Certificate Authority (using openssl_ca.cnf)
    - create an identity for publisher application (using openssl_publisher.cnf)
    - create an identity for subscriber application (using openssl_subscriber.cnf)
    - sign XML configuration files (governance, and permissions for each application)

  - clean
    - delete all generated files (leave directories)

  - clean-all
    - make target "clean"
    - delete directory "security/ca"

  - generate
    - make target "clean"
    - make target "all"

  - env
    - print list of generated files


For example, if you wanted to re-generate all security material, from directory
"HelloWorld_dpde_secure/":

  make -C security/ generate

The Makefile can be controlled via several environment variables:

  - OPENSSL: Path to openssl executable
    - Default: openssl
    - Accepted values: an absolute path to the openssl executable to use
      (or make sure openssl is in PATH).

  - ALGO_KEY: Encryption algorithm used to generate private keys used by
              applications.
    - Default: rsa
    - Accepted values: rsa, dsa, ecdsa

  - ALGO_SIGN: Encryption algorithm used to generate the CA's private key.
    - Default: rsa
    - Accepted values: rsa, dsa, ecdsa

  - KEY_SIZE: Size of generated private keys
    - Default: 2048
    - Accepted values: any valid key size accepted by openssl

  - KEY_CURVE: Name of curve function used to generate ECDSA parameter
    - Default: prime256v1
    - Accepted values: any valid curve function id accepted by openssl

  - XML_GOV: Base name of governance file
    - Default: governance
    - Accepted values: If you want to use a different governance file, place
      it in "security/xml/" and set this variable to the file's base name
      (i.e. without the ".xml" extension).

  - XML_PERM: Base name of permissions files
    - Default: permissions
    - Accepted values: if you want to use different permissions files,
      place them in "security/xml/" and set this variable to the files' base
      prefix. Files must follow the naming pattern "$(XML_PERM)_$(PEER)".
      If you change this value, you must provide a permissions file with
      the selected prefix for each peer identity to be generated.

  - PEERS: list of peer identities to generate
    - Default: publisher, subscriber
    - Accepted values: By default, two identities are generated, one for
      each application generated by the example. If you add and/or modify this
      list, make sure create an OpenSSL configuration file 
      "security/openssl_$(PEER).cnf" for each peer.

All variables can be specified either by:

  - exporting them in your shell environment, e.g.

    export OPENSSL=/path/to/openssl

  - passing them directly to make, e.g.

    make -C security/ generate OPENSSL=/path/to/openssl

  - creating a "security/personal.mk" file:

    echo "OPENSSL=/path/to/openssl" > security/personal.mk

The personal.mk file is optional and it will be sourced last.

See "security/Makefile" for additional environment variables that can be used
to control the makefile's behavior.

---------------------
Generating on Windows
---------------------

Generation of cryptographic material will be automatically performed by each 
Visual Studio project as custom build steps.

Make sure that "openssl" is available in %PATH%, e.g. by adding "%OPENSSLHOME%\bin"
to %PATH%.

Because each project contains all custom build steps (for both publisher and subscriber), 
the cryptographic material will be generated twice when building the provided Solution.

The generated material will use RSA for both peer private keys and the CA's.

If you want to make sure all material is properly re-generated, manually delete all
generated files, e.g.

  rmdir /s security\ca

  del security\xml\*.p7s


How to test interoperability with RTI Connext DDS' hello_security
=================================================================

The built-in Security plugins included with RTI Connext DDS Micro can interoperate 
with those provided by RTI Connext DDS.

A few changes are required in order to have the application generated by 
HelloWorld_dpde_secure communicate with RTI Connext DDS' hello_security example.

If you installed the RTI Connext DDS host bundle, the example can typically be found in:
"${HOME}/rti_workspace/${CONNEXT_VERSION}/examples/connext_dds/c/hello_security".
  
Please refer to the RTI Connext DDS documentation for instruction on how to build
"hello_security" for your target platform.

-------------------------
Changes to hello_security
-------------------------

The hello_security example uses a slightly different data-type. 

In order to interoperate with HellOWorld_dpde_secure, modify 
"hello_security/src/HelloWorld.idl" to match "HelloWorld_dpde_secure/HelloWorld.idl".

E.g. you can simply copy HelloWorld.idl from HelloWorld_dpde_secure to hello_security:

  cp /path/to/HelloWorld_dpde_secure/HelloWorld.idl /path/to/hello_security/src/HelloWorld.idl

After modifying the IDL definition, regenerate support code using rtiddsgen and 
re-compile the example.

---------------------------------------
Configuration of Cryptographic Material
---------------------------------------

The examples must be run using a consistent set of cryptographic material. Since both
examples use separate sets by default, applications will fail to authenticate each other.

The cryptographic material used by the hello_security example is specified in 
"hello_security/USER_QOS_PROFILES.xml" as DDS properties in the DomainParticipantQos.

By default, the security configuration is picked up from 
"${HOME}/rti_workspace/${CONNEXT_VERSION}/examples/dds_security":
  
  - CA: 
    - Identity: "cert/cacertECdsa.pem"
    - Permissions: "cert/cacertECdsa.pem"

  - Publisher
    - Key: "cert/peer1keyECdsa.pem"
    - Certificate: "cert/peer1ECdsa.pem"
    - Permissions: "xml/signed/signed_PermissionsA.p7s"

  - Subscriber
    - Key: "cert/peer2keyECdsa.pem"
    - Certificate: "cert/peer2ECdsa.pem"
    - Permissions: "xml/signed/signed_PermissionsB.p7s"

  - Governance: "xml/signed/signed_Governance.p7s"
    
Since RTI Connext DDS Micro does not directly support XML configuration, HelloWorld_dpde_secure 
uses by default security material stored in the "security/" directory:

  - CA:
    - Identity: "ca/ca.pem"
    - Permissions: "ca/ca.pem"

  - Publisher:
    - Key: "ca/certs/publisher_key.pem"
    - Certificate: "ca/certs/publisher.pem"
    - Permissions: "xml/permissions_publisher.p7s"

  - Publisher:
    - Key: "ca/certs/subscriber_key.pem"
    - Certificate: "ca/certs/subscriber.pem"
    - Permissions: "xml/permissions_subscriber.p7s"

  - Governance: "xml/governance.p7s"

The applications accept several command-line arguments to specify custom paths for each
relevant external file.

--------------------
Running the examples
--------------------

If you prefer not to modify any file, you can run HelloPublisher and HelloSubscriber from hello_security
as you would usually do, e.g from "${HOME}/rti_workspace/${CONNEXT_VERSION}/examples/connext_dds/c/hello_security",
run the RTI Connext DDS publisher:

  objs/${NDDSARCH}/HelloPublisher

Run the RTI Connext DDS Micro subscriber using hello_security's cryptographic material, e.g.
from "HelloWorld_dpde_secure/":

  objs/${RTIMEARCH}/HelloWorld_subscriber \
      -ca-id "${HOME}/rti_workspace/${CONNEXT_VERSION}/examples/dds_security/cert/cacertECdsa.pem"             \
      -ca-perm "${HOME}/rti_workspace/${CONNEXT_VERSION}/examples/dds_security/cert/cacertECdsa.pem"           \
      -key "${HOME}/rti_workspace/${CONNEXT_VERSION}/examples/dds_security/cert/peer2keyECdsa.pem"             \
      -cert "${HOME}/rti_workspace/${CONNEXT_VERSION}/examples/dds_security/cert/peer2ECdsa.pem"               \
      -gov "${HOME}/rti_workspace/${CONNEXT_VERSION}/examples/dds_security/xml/signed/signed_Governance.p7s"   \
      -perm "${HOME}/rti_workspace/${CONNEXT_VERSION}/examples/dds_security/xml/signed/signed_PermissionsB.p7s"

Alternatively, you can modify "hello_security/USER_QOS_PROFILES.xml" to use the security material provided
by HelloWorld_dpde_secure.

