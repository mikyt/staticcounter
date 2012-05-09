

#include "valuevector.h"

#include "gcc-plugin.h"
#include "tree.h"
#include "diagnostic.h"
#include "tm.h"
#include "tree-pass.h"
#include "string.h"
#include "function.h"
#include "basic-block.h"
#include "gimple.h"

/* Declared in rtl.h 
 * Needs forward declaration to force C linkage */
extern "C" int print_rtl_single(FILE*, const_rtx);
#include "rtl.h"



#include "featureextractor.h"

const char * function_name(struct function *fun) {
    const char* name;
    tree decl = fun->decl;
    
    name = IDENTIFIER_POINTER (DECL_NAME(decl));
    return name;
}


unsigned int count_bb_single_successor(const struct function *fun) {
    unsigned int count = 0;
    
    struct basic_block_def *bb;
    FOR_EACH_BB_FN(bb, fun) {
        if(single_succ_p(bb)) {
            count++;
        }
    }
    return count;
}

unsigned int count_bb_two_successors(const struct function *fun) {
    unsigned int count = 0;
    
    struct basic_block_def *bb;
    FOR_EACH_BB_FN(bb, fun) {
        if(EDGE_COUNT(bb->succs)==2) {
            count++;
        }
    }
    return count;
}

unsigned int count_bb_more_two_successors(const struct function *fun) {
    unsigned int count = 0;
    
    struct basic_block_def *bb;
    FOR_EACH_BB_FN(bb, fun) {
        if(EDGE_COUNT(bb->succs)>2) {
            count++;
        }
    }
    return count;
}

unsigned int count_bb_single_predecessor(const struct function *fun) {
    unsigned int count = 0;
    
    struct basic_block_def *bb;
    FOR_EACH_BB_FN(bb, fun) {
        if(single_pred_p(bb)) {
            count++;
        }
    }
    return count;
}

unsigned int count_bb_two_predecessors(const struct function *fun) {
    unsigned int count = 0;
    
    struct basic_block_def *bb;
    FOR_EACH_BB_FN(bb, fun) {
        if(EDGE_COUNT(bb->preds)==2) {
            count++;
        }
    }
    return count;
}

unsigned int count_bb_more_two_predecessors(const struct function *fun) {
    unsigned int count = 0;
    
    struct basic_block_def *bb;
    FOR_EACH_BB_FN(bb, fun) {
        if(EDGE_COUNT(bb->preds)>2) {
            count++;
        }
    }
    return count;
}


unsigned int count_bb_pred_succ(const struct function *fun, unsigned int pred, unsigned int succ) {
    unsigned int count = 0;
    
    struct basic_block_def *bb;
    FOR_EACH_BB_FN(bb, fun) {
        if(EDGE_COUNT(bb->preds)==pred && EDGE_COUNT(bb->succs)==succ) {
            count++;
        }
    }
    return count;
}

unsigned int count_bb_single_pred_single_succ(const struct function *fun) {
    return count_bb_pred_succ(fun, 1, 1);
}

unsigned int count_bb_single_pred_two_succ(const struct function *fun) {
    return count_bb_pred_succ(fun, 1, 2);
}

unsigned int count_bb_two_pred_single_succ(const struct function *fun) {
    return count_bb_pred_succ(fun, 2, 1);
}

unsigned int count_bb_two_pred_two_succ(const struct function *fun) {
    return count_bb_pred_succ(fun, 2, 2);
}

unsigned int count_bb_more_two_pred_more_two_succ(const struct function *fun) {
    unsigned int count = 0;
    
    struct basic_block_def *bb;
    FOR_EACH_BB_FN(bb, fun) {
        if(EDGE_COUNT(bb->preds)>2 && EDGE_COUNT(bb->succs)>2) {
            count++;
        }
    }
    return count;
}

void print_rtl_inst(const rtx inst) {
    /*
      We use print_rtl_single() which takes a FILE*, so
      we need an fmemopen buffer to write to:
    */
    char buf[2048];
    FILE *f;
    
    buf[0]='\0';
    f=fmemopen(buf, sizeof(buf),  "w");
    
    print_rtl_single(f, inst);
    
    fclose(f);
    
    printf("%s", buf); 
}
    
unsigned int count_bb_inst(const struct basic_block_def *bb) {    
    unsigned int insn_count = 0;
    
    gimple_seq instructions(bb->il.gimple->seq);
    gimple_stmt_iterator gsi;
    for (gsi = gsi_start (instructions); !gsi_end_p (gsi); gsi_next (&gsi))
    {
        gimple g = gsi_stmt (gsi);
        
        insn_count++;
    }
    
    return insn_count;
}

unsigned int count_bb_inst_by_code(const struct basic_block_def *bb, enum gimple_code code) {    
    unsigned int insn_count = 0;
    
    gimple_seq instructions(bb->il.gimple->seq);
    gimple_stmt_iterator gsi;
    for (gsi = gsi_start (instructions); !gsi_end_p (gsi); gsi_next (&gsi))
    {
        gimple g = gsi_stmt (gsi);
        if (gimple_code(g) == code) {
            insn_count++;
        }
    }
    
    return insn_count;
}

unsigned int count_bb_less_15_inst(const struct function *fun) {
    unsigned int count = 0;
    
    struct basic_block_def *bb;
    FOR_EACH_BB_FN(bb, fun) {
        if(count_bb_inst(bb) < 15) {
            count++;
        }
    }
    return count;
}

unsigned int count_bb_between_15_and_500_inst(const struct function *fun) {
    unsigned int count = 0;
    
    struct basic_block_def *bb;
    FOR_EACH_BB_FN(bb, fun) {
        unsigned int bb_inst_count (count_bb_inst(bb));
        if( bb_inst_count >= 15 && bb_inst_count <= 500 ) {
            count++;
        }
    }
    return count;
}

unsigned int count_bb_more_500_inst(const struct function *fun) {
    unsigned int count = 0;
    
    struct basic_block_def *bb;
    FOR_EACH_BB_FN(bb, fun) {
        if(count_bb_inst(bb) > 500) {
            count++;
        }
    }
    return count;
}

unsigned int count_inst_by_code(const struct function *fun, enum gimple_code code) {
    unsigned int count=0;
     
    struct basic_block_def *bb;
    FOR_EACH_BB_FN(bb, fun) {
        count += count_bb_inst_by_code(bb, code);
    }
    
    return count;
}

bool is_binary_integer_op_p(const_gimple stmt) {
    if ( ! is_gimple_assign(stmt) ) {
        return false;
    }
    
    if ( ! (gimple_assign_rhs_class(stmt) == GIMPLE_BINARY_RHS)) {
        return false;
    }
        
    enum tree_code assign_code = gimple_assign_rhs_code(stmt);
    
    if ((assign_code >= PLUS_EXPR && assign_code <= ROUND_MOD_EXPR)
       || (assign_code >= LSHIFT_EXPR && assign_code <= TRUTH_XOR_EXPR)) {
        return true;
    }
    
    return false;
}

unsigned int count_bb_binary_integer_ops(const struct basic_block_def *bb) {    
    unsigned int insn_count = 0;
    
    gimple_seq instructions(bb->il.gimple->seq);
    gimple_stmt_iterator gsi;
    for (gsi = gsi_start (instructions); !gsi_end_p (gsi); gsi_next (&gsi))
    {
        gimple g = gsi_stmt (gsi);
        
        if (is_binary_integer_op_p(g)) {
            insn_count++;
        }
    }
    
    return insn_count;
}

unsigned int count_binary_integer_ops(const struct function *fun) {
    unsigned int count=0;
     
    struct basic_block_def *bb;
    FOR_EACH_BB_FN(bb, fun) {
        count += count_bb_binary_integer_ops(bb);
    }
    
    return count;
}

void perform_low_gimple_analyses(WeightedValueVector *values, const struct function *fun) {
    (*values)[avg_basic_block_number].update_average(n_basic_blocks_for_function(fun));    
    (*values)[avg_basic_block_single_successor].update_average(count_bb_single_successor(fun));
    (*values)[avg_basic_block_two_successors].update_average(count_bb_two_successors(fun));
    (*values)[avg_basic_block_more_two_successors].update_average(count_bb_more_two_successors(fun));
    (*values)[avg_basic_block_single_predecessor].update_average(count_bb_single_predecessor(fun));
    (*values)[avg_basic_block_two_predecessor].update_average(count_bb_two_predecessors(fun));
    (*values)[avg_basic_block_more_two_predecessor].update_average(count_bb_more_two_predecessors(fun));
    (*values)[avg_basic_block_single_pred_single_succ].update_average(count_bb_single_pred_single_succ(fun));
    (*values)[avg_basic_block_single_pred_two_succ].update_average(count_bb_single_pred_two_succ(fun));
    (*values)[avg_basic_block_two_pred_single_succ].update_average(count_bb_two_pred_single_succ(fun));
    (*values)[avg_basic_block_two_pre_two_succ].update_average(count_bb_two_pred_two_succ(fun));
    (*values)[avg_basic_block_more_two_pred_more_two_succ].update_average(count_bb_more_two_pred_more_two_succ(fun));
    (*values)[avg_basic_block_less_15_inst].update_average(count_bb_less_15_inst(fun));
    (*values)[avg_basic_block_between_15_and_500_inst].update_average(count_bb_between_15_and_500_inst(fun));
    (*values)[avg_basic_block_more_500_inst].update_average(count_bb_more_500_inst(fun));
    (*values)[avg_edges_in_cfg].update_average(n_edges_for_function(fun));
    (*values)[avg_method_assignment_inst].update_average(count_inst_by_code(fun, GIMPLE_ASSIGN));
    (*values)[avg_method_conditional_branch_inst].update_average(count_inst_by_code(fun, GIMPLE_COND));
    (*values)[avg_method_unconditional_branch_inst].update_average(count_inst_by_code(fun, GIMPLE_GOTO));
    (*values)[avg_method_binary_integer_ops].update_average(count_binary_integer_ops(fun));
}

void perform_rtl_analyses(WeightedValueVector *values, const struct function *fun) {
    
}

extern "C" void all_passes_end_callback(void *gcc_data, void *user_data) {
    using namespace boost::interprocess;
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
    
    if (strcmp(current_pass->name, "expand")==0) {
        //pass_expand ("expand") lowers the ir from GIMPLE to RTL.
        //Here, we are just about to execute it.
        //It time to perform analyses on the final version of GIMPLE
        perform_low_gimple_analyses(values, cfun);
    }
    
    if (strcmp(current_pass->name, "*free_cfg")==0) {
        //Time to analyse the final cfg before it is erased!
        perform_rtl_analyses(values, cfun);
        return;
    }
    
    
    

    
    
}
