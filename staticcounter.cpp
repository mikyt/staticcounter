
#include <iostream>

#include "gcc-plugin.h"

#include "featureextractor.h"


int plugin_is_GPL_compatible;




    
extern "C" void wrap_up(void *gcc_data, void *user_data) {
    std::cout << "Wrapping everything up" << std::endl;
    
   
}

extern "C" int plugin_init(struct plugin_name_args *plugin_info, 
                struct plugin_gcc_version *version) {

    
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

