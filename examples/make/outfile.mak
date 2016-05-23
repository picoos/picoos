#############################################################################
#  This makefile compiles the main program of your project. It links        #
#  external libraries and the pico]OS RTOS to the program and generates     #
#  the executable. This makefile can call other makefiles to compile        #
#  the libraries that are needed to create the executable.                  #
#  Copy this file to your project main directory and rename it to makefile. #
#############################################################################


# ---------------------------------------------------------------------------
# PRECONDITION SETUP
# ---------------------------------------------------------------------------

# Set relative path to the picoos root directory  -> YOU NEED TO CHANGE THIS!
RELROOT = ../../../picoos/

# Set fixed target platform                       -> YOU MAY CHANGE THIS!
#PORT = x86w32  (->need to be specified at the command line)

# Set fixed build mode (DEBUG or RELEASE)         -> YOU MAY CHANGE THIS!
#BUILD = DEBUG  (->need to be specified at the command line)

# Get pico]OS path from environment (if it is set)
ifneq '$(strip $(PICOOS))' ''
RELROOT := $(PICOOS)/
endif

# Include common makefile
include $(RELROOT)make/common.mak



# ---------------------------------------------------------------------------
# PROJECT SETTINGS
# ---------------------------------------------------------------------------

# To include the pico]OS nano layer, set this define to 1
NANO = 0

# Set target filename (the name of the executable, without extension)
TARGET =


### Set up lists of source files ###

# Set list of C source files / assembler files
SRC_TXT =

# Set list of header files used by the C source files
SRC_HDR = 

# You may add some already compiled object files here
SRC_OBJ =

# Add the libraries you are using in your program here
SRC_LIB =


# Set additional C-defines
CDEFINES +=

# Set additional include paths
DIR_USRINC +=

# Set the directory that contains the configuration header files.
# If this variable is not set, the default configuration files will be
# taken from the port/default directory.
# If you wish to use your own configuration files, you can place them
# in the current directory. Then, set  DIR_CONFIG = $(CURRENTDIR)
DIR_CONFIG =

# Set the output directory for the generated binaries. If you do not
# set this variable, the files will be stored in the pico]OS root
# root directory structure (pioos/out, picoos/lib, picoos/obj)
DIR_OUTPUT = $(CURRENTDIR)/bin

# Include pico]OS based modules. Set here the list of modules
# (libraries) you want to use in your project. Note that the module
# name is the name of the directory where the sources and the makefile
# is stored. The makefile must be named "makefile" or "makefile.pico"
# Example:  MODULES += $(RELROOT)../lcdinterface
MODULES +=

# Execute external makefiles to build other libraries. Add here
# the filenames (with path) of other makefiles you wish to execute.
EXEC_MAKEFILES +=



# ---------------------------------------------------------------------------
# BUILD THE EXECUTABLE
# ---------------------------------------------------------------------------

include $(MAKE_OUT)

