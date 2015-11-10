#################################
#			Makefile			#
#################################

FILE_TO_CHECK=Geant_Vin.root

weightfield:
ifeq ($(wildcard $(FILE_TO_CHECK)),)
	$(error Missing file $(FILE_TO_CHECK))
endif
	rootcint -f Dict.cxx -c WFGUI.h LinkDef.h 
	g++ -O3 -Wall -Wextra -o weightfield Weightfield.cxx WFGUI.cxx Potentials.cxx Field.cxx Carriers.cxx `root-config --cflags --glibs` Dict.cxx -lCint 
	
all: 
ifeq ($(wildcard $(FILE_TO_CHECK)),)
	$(error Missing file $(FILE_TO_CHECK))
endif
	rootcint -f Dict.cxx -c WFGUI.h LinkDef.h 
	g++ -O3 -Wall -Wextra -o weightfield Weightfield.cxx WFGUI.cxx Potentials.cxx Field.cxx Carriers.cxx `root-config --cflags --glibs` Dict.cxx -lCint 
debug:
ifeq ($(wildcard $(FILE_TO_CHECK)),)
	$(error Missing file $(FILE_TO_CHECK))
endif
	rootcint -f Dict.cxx -c WFGUI.h LinkDef.h 
	g++ -g -O0 -Wall -Wextra -o weightfield Weightfield.cxx WFGUI.cxx Potentials.cxx Field.cxx Carriers.cxx `root-config --cflags --glibs` Dict.cxx -lCint
