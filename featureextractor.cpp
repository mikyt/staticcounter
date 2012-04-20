#include "featureextractor.h"

#include "valuevector.h"

#include "gcc-plugin.h"
#include "diagnostic.h"
#include "tm.h"
#include "tree-pass.h"
#include "string.h"
#include "function.h"
#include "basic-block.h"
#include "libiberty.h"

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


extern "C" void all_passes_end_callback(void *gcc_data, void *user_data) {
    using namespace boost::interprocess;
    
    if (strcmp(current_pass->name, "*free_cfg")!=0) {
        return;
    }
    //Time to analyse the final cfg before it is erased!
    
    char shm_name[1024];
    sprintf(shm_name, SHM_NAME_PREFIX "_%d", getppid());
    
    //A special shared memory where we can
    //construct objects associated with a name.
    //Connect to the already created shared memory segment
    //and initialize needed resources
    static managed_shared_memory segment (open_only,
                                          shm_name);  //segment name

    //Find the vector using the c-string name
    static WeightedValueVector *values = segment.find<WeightedValueVector>("values").first;

    

    (*values)[avg_basic_block_number].update_average(n_basic_blocks_for_function(cfun));
    (*values)[avg_basic_block_single_successor].update_average(count_bb_single_successor(cfun));
    (*values)[avg_basic_block_two_successors].update_average(count_bb_two_successors(cfun));
    (*values)[avg_basic_block_more_two_successors].update_average(count_bb_more_two_successors(cfun));
    (*values)[avg_basic_block_single_predecessor].update_average(count_bb_single_predecessor(cfun));
    (*values)[avg_basic_block_two_predecessor].update_average(count_bb_two_predecessors(cfun));
    
}
