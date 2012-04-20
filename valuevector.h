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
    }
#undef tostring        
}


struct WeightedValue {
    unsigned int count;
    double value;
    
    WeightedValue() : count(0), value(0) {}
        
    void update_average(const double new_value) {
        //std::cout << "Updating average with value: " << new_value << std::endl;
        //std::cout << "  Old value: " << value << " Old count: " << count << std::endl;
        value = (value*count + new_value) / (count+1);
        count++;
        //std::cout << "  New value: " << value << " New count: " << count << std::endl;
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