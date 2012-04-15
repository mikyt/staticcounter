
#include "gcc-plugin.h"
#include "diagnostic.h"
#include "tm.h"
#include "tree-pass.h"
#include "string.h"
#include "function.h"

int plugin_is_GPL_compatible;


const char * function_name(struct function *fun) {
    const char* name;
    tree decl = fun->decl;
    
    name = IDENTIFIER_POINTER (DECL_NAME(decl));
    return name;
}


extern "C" void all_passes_end_callback(void *gcc_data, void *user_data) {
    printf("Finished working on function: %s\n", function_name(cfun));
}
    
    
extern "C" int plugin_init(struct plugin_name_args *plugin_info, 
                struct plugin_gcc_version *version) {

    register_callback(plugin_info->base_name,
                      PLUGIN_ALL_PASSES_END,
                      &all_passes_end_callback,
                      NULL);
                       
    return 0;
}

