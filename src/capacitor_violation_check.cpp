#include <vector>
#include <algorithm>
#include <stdexcept>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>
#include <cmath>

#include "capacitor_violation_check.h"

// Decorator base class for current violation
CapacitorMaxViolationCheckDecoratorBase::CapacitorMaxViolationCheckDecoratorBase(CapacitorInterface* cap) : cap(cap)
{}

double CapacitorMaxViolationCheckDecoratorBase::xc(double f) const {
    return cap->xc(f);
}

std::string CapacitorMaxViolationCheckDecoratorBase::name() const {
    return cap->name();
}

const CapacitorSpec& CapacitorMaxViolationCheckDecoratorBase::spec() const {
    return cap->spec();
}

double CapacitorMaxViolationCheckDecoratorBase::allowed_current(double f) const {
    return cap->allowed_current(f);
}

CapacitorMaxViolationCheckDecorator::CapacitorMaxViolationCheckDecorator(CapacitorInterface* cap) 
    : CapacitorMaxViolationCheckDecoratorBase(cap) 
{
    
}

double CapacitorMaxViolationCheckDecorator::current(double f, double voltage) const {
    double spec_max_current = cap->spec().get_i_max();
    double current = cap->current(f, voltage);
    if (current > spec_max_current) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(0) << 
            "Warning: Overcurrent condition on " << cap->name() << 
            ". The current is " << current << "A" << 
            ", which exceeds the maximum current of " << spec_max_current << "A!";
        #ifdef LOG_CONSOLE
            std::cout << oss.str() << std::endl;
        #else
            throw std::runtime_error(oss.str());
        #endif
    }

    double spec_max_power = cap->spec().get_power_max();
    if (current * voltage > spec_max_power) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(0) << 
            "Warning: Overpower condition on " << cap->name() << 
            ". The power is " << current * voltage << 
            "W, which exceeds the maximum power of " << spec_max_power << "W!";
        #ifdef LOG_CONSOLE
            std::cout << oss.str() << std::endl;
        #else
            throw std::runtime_error(oss.str());
        #endif
    }

    return current;
}

double CapacitorMaxViolationCheckDecorator::voltage(double f, double current) const {
    
    double spec_max_voltage = cap->spec().get_v_max();
    double voltage = cap->voltage(f, current);
    
    if (voltage > spec_max_voltage) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(0) << 
            "Warning: Overvoltage condition on " << cap->name() << 
            ". The voltage is " << voltage << 
            "V, which exceeds the maximum voltage of " << spec_max_voltage << "V!";
        #ifdef LOG_CONSOLE
            std::cout << oss.str() << std::endl;
        #else
            throw std::runtime_error(oss.str());
        #endif
    }

    double spec_max_power = cap->spec().get_power_max();
    if (current * voltage > spec_max_power) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(0) << 
            "Warning: Overpower condition on " << cap->name() << 
            ". The power is " << current * voltage << 
            "W, which exceeds the maximum power of " << spec_max_power << "W!";
        
        #ifdef LOG_CONSOLE
            std::cout << oss.str() << std::endl;
        #else
            throw std::runtime_error(oss.str());
        #endif
    }
    return voltage;
}




