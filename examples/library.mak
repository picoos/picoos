#############################################################################
#  This makefile compiles the sourcefiles to a library.                     #
#  The library can be referenced by the main makefile (Please have a look   #
#  to the example file outfile.mak. Set there the variable 'SRC_LIB' or     #
#  'MODULES' to reference this library). Please notice: In pico]OS syntax,  #
#  a library that is based on this makefile is also called 'a module'.      #
#  Note: You must rename this file to "makefile" or "makefile.pico".        #
#############################################################################


# ---------------------------------------------------------------------------
# PRECONDITION SETUP
# ---------------------------------------------------------------------------

# Set relative path to the picoos root directory  -> YOU NEED TO CHANGE THIS!
RELROOT = ../../../picoos/

# Get pico]OS path from environment (if it is set)
ifneq '$(strip $(PICOOS))' ''
RELROOT := $(PICOOS)/
endif

# Include common makefile
include $(RELROOT)make/common.mak



# ---------------------------------------------------------------------------
# LIBRARY SETTINGS
# ---------------------------------------------------------------------------

# Set target file name (library filename without extension)
TARGET =


### Set up lists of source files ###

# Set list of C source files / assembler files
SRC_TXT =

# Set list of header files used by the C source files
SRC_HDR = 

# You may add some already compiled object files here
SRC_OBJ =


# Set additional C-defines
CDEFINES +=

# Set additional include paths
DIR_USRINC +=

# Set the output directory for the generated binaries. If you do not
# set this variable, the files will be stored in the pico]OS root
# root directory structure (pioos/out, picoos/lib, picoos/obj)
# Note: Please keep the ifeq/endif pair. This allows to set the
# output directory from outsite (may be by a project makefile).
ifeq '$(strip $(DIR_OUTPUT))' ''
DIR_OUTPUT = $(CURRENTDIR)/bin
endif



# ---------------------------------------------------------------------------
# BUILD THE LIBRARY
# ---------------------------------------------------------------------------

include $(MAKE_LIB)
