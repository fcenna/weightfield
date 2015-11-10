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
	ifeq($(wildcard Geant_Vin.root),"")
		$(error Missing file Geant_Vin.root)
	endif
	echo $(wildcard Geant_*)
	rootcint -f Dict.cxx -c WFGUI.h LinkDef.h 
	g++ -O3 -Wall -Wextra -o weightfield Weightfield.cxx WFGUI.cxx Potentials.cxx Field.cxx Carriers.cxx `root-config --cflags --glibs` Dict.cxx -lCint 
debug:
# 	ifneq("$(wildcard Geant_Vin.root)","Geant_Vin.root")
# 		$(error Missing file Geant_Vin.root)
# 	endif
	rootcint -f Dict.cxx -c WFGUI.h LinkDef.h 
	g++ -g -O0 -Wall -Wextra -o weightfield Weightfield.cxx WFGUI.cxx Potentials.cxx Field.cxx Carriers.cxx `root-config --cflags --glibs` Dict.cxx -lCint
