##################################################
 Makefile for generation of DDS Security material
##################################################

This makefile can be used to generate all cryptographic material and signed 
XML configuration files required by a DDS application wanting to enable the
DDS Security extensions.

-------------
 Basic Usage
-------------

Simply call make from this directory:

    make

This command will generate:

  - A Certificate Authority in the "ca/" subdirectory

  - A pair of private key and public certificate for two DDS identities,
    a "publisher", and a "subscriber", in the "ca/certs" directory.

  - A signed XML governance file, and two signed XML permissions files, one
    for the "publisher" identities, and one for the "subscriber", in the
    "xml" directory.

------------------
 Makefile targets
------------------

  - all (default)
  
    Makes sure all required files exists, or trigger their generation.

  - generate:
  
    Regenerate all material by first deleting it. Useful to refresh all files,
    including the ca.

  - clean:
  
    Delete all generated material.
    
  - clean-all:
  
    Delete all generated material and the ca/ directory.

----------------
 Advanced usage
----------------

The makefile's behavior can be customized through several variables.
These can be overriden in personal.mk (or simply from the command line, and/or
shell environment).

For example, you may consider modifying variables ALGO_KEY and ALGO_SIGN to
control the algorithm used to generate peer keys and CA keys respectively.
Valid values are "rsa", "dsa", "ecdsa".

You can specify any variable explicitely on the command line when invoking
make, e.g. using the "generate" target to force regeneration of all material:

    make generate ALGO_KEY=dsa ALGO_SIGN=ecdsa

Please refer to the Makefile for a complete list of all variables and an
explanation of how to use them.
