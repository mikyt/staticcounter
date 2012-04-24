#Neeeds gcc >= 4.5
GCC=g++
RM=rm

PLUGIN_SOURCE_FILES=staticcounter.cpp featureextractor.cpp
PLUGIN_OBJECT_FILES= $(patsubst %.cpp,%.o,$(PLUGIN_SOURCE_FILES))
GCCPLUGINS_DIR:= $(shell $(GCC) -print-file-name=plugin)
CXXFLAGS+= -I$(GCCPLUGINS_DIR)/include -fPIC -O2 -g

DEPS=valuevector.h featureextractor.h featurelist.inc

staticcounter.so: $(PLUGIN_OBJECT_FILES) 
	$(GCC) -shared  $(PLUGIN_OBJECT_FILES) -g -o $@ -lrt

staticcounter.o: $(DEPS)

featureextractor.o: $(DEPS)

.PHONY: clean

clean:
	$(RM) -rf *.so *.*~ *.o
