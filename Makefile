#Neeeds gcc >= 4.5
GCC=g++
RM=rm

PLUGIN_SOURCE_FILES=staticcounter.cpp
PLUGIN_OBJECT_FILES= $(patsubst %.cpp,%.o,$(PLUGIN_SOURCE_FILES))
GCCPLUGINS_DIR:= $(shell $(GCC) -print-file-name=plugin)
CXXFLAGS+= -I$(GCCPLUGINS_DIR)/include -fPIC -O2
      
staticcounter.so: $(PLUGIN_OBJECT_FILES)
	$(GCC) -shared $^ -o $@

clean:
	$(RM) -rf *.so *.cpp~ *.o
