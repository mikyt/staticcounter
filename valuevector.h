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
#define feature(f) f,
#include "featurelist.inc"
#undef feature
    LAST
} feature_t;

static std::string featurestring(const int feature) {
#define feature(s) case s: return #s;
    switch(feature) {
#include "featurelist.inc"
    }
#undef feature
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