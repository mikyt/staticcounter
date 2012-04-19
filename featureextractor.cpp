#include "featureextractor.h"

#include "valuemap.h"

#include "gcc-plugin.h"
#include "diagnostic.h"
#include "tm.h"
#include "tree-pass.h"
#include "string.h"
#include "function.h"
#include "basic-block.h"

const char * function_name(struct function *fun) {
    const char* name;
    tree decl = fun->decl;
    
    name = IDENTIFIER_POINTER (DECL_NAME(decl));
    return name;
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

unsigned int count_bb_two_predecessors(struct function *fun) {
    unsigned int count = 0;
    
    struct basic_block_def *bb;
    FOR_EACH_BB_FN(bb, fun) {
        if(EDGE_COUNT(bb->preds)==2) {
            count++;
        }
    }
    return count;
}

void increase_value(WeightedValueMap& values, const std::string name, const double value) {
    WeightedValueMap::iterator element;
    double old_value;
    unsigned int old_count;
    
    element = values.find(name);
    if(element == values.end()) {
        //not found!
        //init a new one
        old_value = 0;
        old_count = 0;
    }
    else {
        old_value = element->second.value;
        old_count = element->second.count;
    }
    
    WeightedValue new_value;
    new_value.count = old_count + 1;
    new_value.value = old_value + value;
    
    values[name] = new_value;
    
    std::cout << "Increasing " << name << " by " << value << " starting from "
              << old_value << " reaching " << values[name].value << std::endl;
}

void update_average(WeightedValueMap& values, const std::string name, const double new_value) {
    WeightedValueMap::iterator element;
    double old_value;
    unsigned int old_count;
    
    element = values.find(name);
    if(element == values.end()) {
        //not found!
        //init a new one
        old_value = 0;
        old_count = 0;
    }
    else {
        old_value = element->second.value;
        old_count = element->second.count;
    }
    
    WeightedValue new_weighted_value;
    new_weighted_value.value = ((old_value*old_count)+new_value)/(old_count+1);
    new_weighted_value.count = old_count + 1;
    
    values[name] = new_weighted_value;
    
    std::cout << "New average for " << name << " adding " << new_value << " starting from "
              << old_value << " reaching " << values[name].value << std::endl;
}

extern "C" void all_passes_end_callback(void *gcc_data, void *user_data) {
    static WeightedValueMap values;
    if (strcmp(current_pass->name, "*free_cfg")!=0) {
        return;
    }
    
    //Time to analyse the final cfg before it is erased!
    printf("main_input_filename: %s\n", main_input_filename);
    printf("Gathering data about function: %s\n", function_name(cfun));
    printf("Storage in valuemap: %x, size: %ld\n", &values, values.size());
    
    
    update_average(values, "avg_basic_block_number", n_basic_blocks_for_function(cfun));
    update_average(values, "avg_basic_block_single_successor", count_bb_single_successor(cfun));
    update_average(values, "avg_basic_block_two_successors", count_bb_two_successors(cfun));
    update_average(values, "avg_basic_block_more_two_successors", count_bb_more_two_successors(cfun));
    update_average(values, "avg_basic_block_single_predecessor", count_bb_single_predecessor(cfun));
    update_average(values, "avg_basic_block_two_predecessor", count_bb_two_predecessors(cfun));
    
    print_valuemap(values);
    printf("Finished working on function: %s\n", function_name(cfun));
    
    
}
