#ifndef VALUEMAP_H
#define VALUEMAP_H

#include <iostream>
#include <map>
#include <string>

typedef struct {
    unsigned int count;
    double value;
} WeightedValue;

typedef std::map<std::string, WeightedValue> WeightedValueMap;


void print_valuemap(const WeightedValueMap valuemap) {
    for(WeightedValueMap::const_iterator i=valuemap.begin(), e=valuemap.end();
        i != e;
        ++i) {
        std::string featureName (i->first);
        double featureValue (i->second.value);
        std::cout << featureName << " = " << featureValue << std::endl;
    }
}


#endif