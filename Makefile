#################################
#			Makefile			#
#################################

SOURCES := Weightfield.cxx WFGUI.cxx Potentials.cxx Field.cxx Carriers.cxx
HEADERS := WFGUI.h Potentials.h Field.h Carriers.h
FILE_TO_CHECK := root_files/Geant_Vin.root
ROOT_CONF := `root-config --cflags --glibs`

weightfield: $(HEADERS) $(SOURCES)
ifeq ($(wildcard $(FILE_TO_CHECK)),)
	$(error Missing file $(FILE_TO_CHECK))
endif
	rootcint -f Dict.cxx -c WFGUI.h LinkDef.h 
	g++ -O3 -Wall -Wextra -o weightfield $(SOURCES) $(ROOT_CONF) Dict.cxx -lCint 
	
all: $(HEADERS) $(SOURCES)
ifeq ($(wildcard $(FILE_TO_CHECK)),)
	$(error Missing file $(FILE_TO_CHECK))
endif
	rootcint -f Dict.cxx -c WFGUI.h LinkDef.h 
	g++ -O3 -Wall -Wextra -o weightfield $(SOURCES) $(ROOT_CONF) Dict.cxx -lCint 
	
debug: $(HEADERS) $(SOURCES)
ifeq ($(wildcard $(FILE_TO_CHECK)),)
	$(error Missing file $(FILE_TO_CHECK))
endif
	rootcint -f Dict.cxx -c WFGUI.h LinkDef.h 
	g++ -g -O0 -Wall -Wextra -o weightfield $(SOURCES) $(ROOT_CONF) Dict.cxx -lCint
