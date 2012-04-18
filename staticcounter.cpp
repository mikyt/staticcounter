
#include <map>
#include <string>
#include <iostream>

#include "gcc-plugin.h"
#include "diagnostic.h"
#include "tm.h"
#include "tree-pass.h"
#include "string.h"
#include "function.h"
#include "basic-block.h"

int plugin_is_GPL_compatible;


const char * function_name(struct function *fun) {
    const char* name;
    tree decl = fun->decl;
    
    name = IDENTIFIER_POINTER (DECL_NAME(decl));
    return name;
}

typedef std::map<std::string, double> ValueMap;


void print_valuemap(const ValueMap valuemap) {
    for(ValueMap::const_iterator i=valuemap.begin(), e=valuemap.end();
        i != e;
        ++i) {
        std::string featureName (i->first);
        double featureValue (i->second);
        std::cout << featureName << " = " << featureValue << std::endl;
    }
}


unsigned int count_bb_single_successor(struct function *fun) {
    unsigned int count = 0;
    
    struct basic_block_def *bb;
    FOR_EACH_BB_FN(bb, fun) {
        if(single_succ_p(bb)) {
            count++;
        }
    }
    return count;
}

unsigned int count_bb_two_successors(struct function *fun) {
    unsigned int count = 0;
    
    struct basic_block_def *bb;
    FOR_EACH_BB_FN(bb, fun) {
        if(EDGE_COUNT(bb->succs)==2) {
            count++;
        }
    }
    return count;
}

unsigned int count_bb_more_two_successors(struct function *fun) {
    unsigned int count = 0;
    
    struct basic_block_def *bb;
    FOR_EACH_BB_FN(bb, fun) {
        if(EDGE_COUNT(bb->succs)>2) {
            count++;
        }
    }
    return count;
}

unsigned int count_bb_single_predecessor(struct function *fun) {
    unsigned int count = 0;
    
    struct basic_block_def *bb;
    FOR_EACH_BB_FN(bb, fun) {
        if(single_pred_p(bb)) {
            count++;
        }
    }
    return count;
}

extern "C" void all_passes_end_callback(void *gcc_data, void *user_data) {
    static ValueMap values;
    if (strcmp(current_pass->name, "*free_cfg")!=0) {
        return;
    }
    
    //Time to analyse the final cfg before it is erased!
    printf("Gathering data about function: %s\n", function_name(cfun));
    printf("Storage in valuemap: %x, size: %ld\n", &values, values.size());
    values["basic_block_number"] = n_basic_blocks_for_function(cfun);
    values["basic_block_single_successor"] = count_bb_single_successor(cfun);
    values["basic_block_two_successors"] = count_bb_two_successors(cfun);
    values["basic_block_more_two_successors"] = count_bb_more_two_successors(cfun);
    values["basic_block_single_predecessor"] = count_bb_single_predecessor(cfun);
    
    print_valuemap(values);
    printf("Finished working on function: %s\n", function_name(cfun));
}
    
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

