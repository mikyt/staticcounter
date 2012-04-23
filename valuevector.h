#ifndef VALUEMAP_H
#define VALUEMAP_H

#include <iostream>
#include <map>
#include <string>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#define SHM_NAME_PREFIX "staticcounter"

typedef enum {
    avg_basic_block_number, 
    avg_basic_block_single_successor,
    avg_basic_block_two_successors,
    avg_basic_block_more_two_successors,
    avg_basic_block_single_predecessor,
    avg_basic_block_two_predecessor,
    avg_basic_block_more_two_predecessor,
    avg_basic_block_single_pred_single_succ,
    avg_basic_block_single_pred_two_succ,
    avg_basic_block_two_pred_single_succ,
    avg_basic_block_two_pre_two_succ,
    avg_basic_block_more_two_pred_more_two_succ,
    avg_basic_block_less_15_inst,
    avg_basic_block_between_15_and_500_inst,
    avg_basic_block_more_500_inst,
    avg_edges_in_cfg,
    LAST
} feature_t;

static std::string featurestring(const int feature) {
#define tostring(s) case s: return #s
    switch(feature) {
        tostring(avg_basic_block_number);
        tostring(avg_basic_block_single_successor);
        tostring(avg_basic_block_two_successors);
        tostring(avg_basic_block_more_two_successors);
        tostring(avg_basic_block_single_predecessor);
        tostring(avg_basic_block_two_predecessor);
        tostring(avg_basic_block_more_two_predecessor);
        tostring(avg_basic_block_single_pred_single_succ);
        tostring(avg_basic_block_single_pred_two_succ);
        tostring(avg_basic_block_two_pred_single_succ);
        tostring(avg_basic_block_two_pre_two_succ);
        tostring(avg_basic_block_more_two_pred_more_two_succ);
        tostring(avg_basic_block_less_15_inst);
        tostring(avg_basic_block_between_15_and_500_inst);
        tostring(avg_basic_block_more_500_inst);
        tostring(avg_edges_in_cfg);
    }
#undef tostring        
}


struct WeightedValue {
    unsigned int count;
    double value;
    
    WeightedValue() : count(0), value(0) {}
        
    void update_average(const double new_value) {
        value = (value*count + new_value) / (count+1);
        count++;
    }
};


typedef boost::interprocess::allocator<WeightedValue, boost::interprocess::managed_shared_memory::segment_manager> ShmemAllocator;

class WeightedValueVector : public boost::interprocess::vector<WeightedValue, ShmemAllocator> {
public:
    WeightedValueVector(ShmemAllocator allocator) : vector(allocator) {}
    
    void to_xml(std::ostream& o) const {
        o << "<features>" << std::endl;
        for (int i=0, e=size(); i<e; ++i) {
            
            const std::string &name (featurestring(i));
            WeightedValue featurevalue((*this)[i]);
            const unsigned int &count (featurevalue.count);
            const double &value (featurevalue.value);
            o << "  <feature name=\"" << name << "\" value=\"" << value 
                                      << "\" count=\"" << count << "\" />"
                                      << std::endl;
                                      
        }
        o << "</features>";
    }
};


#endif