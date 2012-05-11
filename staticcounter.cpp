
#include "valuevector.h"

#include <iostream>

#include "gcc-plugin.h"

#include "featureextractor.h"

#define OUTFILE_NAME "features.xml"

int plugin_is_GPL_compatible;

static char shm_name[1024];
static std::string outfile_name;

bool fileexists(const char *filename)
{
  std::ifstream ifile(filename);
  return ifile;
}
    
extern "C" void wrap_up(void *gcc_data, void *user_data) {
    using namespace boost::interprocess;
    
    managed_shared_memory segment (open_only,
                                   shm_name);  //segment name
      
    //Find the vector using the c-string name
    WeightedValueVector *values = segment.find<WeightedValueVector>("values").first;
    
    std::ofstream outfile (outfile_name.c_str());

    values->to_xml(outfile);
    outfile.close();
    
    //std::cout << "Features saved on file " << filename << std::endl;
    //segment.destroy<WeightedValueVector>("values");
    //bool removeResult = shared_memory_object::remove(shm_name.c_str()); 
    //std::cout << "Final remove: " << removeResult << std::endl;
}

void init_vector() {
    using namespace boost::interprocess;
    
    unsigned int numfeatures = LAST;
    
    //Estimating the amount of memory needed for the vector
    size_t overhead (sizeof(WeightedValue*) * 8);
    size_t segment_size ((sizeof(WeightedValue)+overhead)*numfeatures);
    
    //The process-specific name of the shared memory area
    //NB: only the parent pid is the same for all the processes that need to
    //access this area, so we use it instead of the pid.
    if( ! fileexists(outfile_name.c_str())) {
    	//This should be the execution of the first instance of the plugin
        shared_memory_object::remove(shm_name);
    }

    managed_shared_memory segment (create_only, 
                                   shm_name, 
                                   segment_size);
    //Initialize shared memory STL-compatible allocator
    const ShmemAllocator alloc_inst (segment.get_segment_manager());

    WeightedValueVector *values = segment.construct<WeightedValueVector>("values")(alloc_inst);
    values->resize(numfeatures);
}

void set_outfile_name(struct plugin_name_args *plugin_info) {
    outfile_name = OUTFILE_NAME;
    
    for(int i=0, e=plugin_info->argc; i<e; ++i) {
        std::string key = plugin_info->argv[i].key;
        if(key!="outputfile") {
            continue;
        }
        
        outfile_name = plugin_info->argv[i].value;
        return;
    }
}

extern "C" int plugin_init(struct plugin_name_args *plugin_info, 
                struct plugin_gcc_version *version) {
    
    //Build shared memory name
    sprintf(shm_name, SHM_NAME_PREFIX "_%d", getppid());
    
    set_outfile_name(plugin_info);
    
    try {
        init_vector();
    }
    catch (...) {
        //This is not the first process going through here: nothing to do
        //The vector already exists
    }
    
    register_callback(plugin_info->base_name,
                      PLUGIN_PASS_EXECUTION,
                      &all_passes_end_callback,
                      NULL);
    
    register_callback(plugin_info->base_name,
                      PLUGIN_FINISH,
                      &wrap_up,
                      NULL);

    
    
    return 0;
}

