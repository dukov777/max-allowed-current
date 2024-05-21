#include <vector>
#include <algorithm>
#include <stdexcept>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>
#include <cmath>

#include "capacitor_dump_value.h"

// Decorator base class for current violation
CapacitoDumpValueDecoratorBase::CapacitoDumpValueDecoratorBase(CapacitorInterface* cap) : cap(cap)
{}

double CapacitoDumpValueDecoratorBase::xc(double f) const {
    return cap->xc(f);
}

std::string CapacitoDumpValueDecoratorBase::name() const {
    return cap->name();
}

const CapacitorSpec& CapacitoDumpValueDecoratorBase::spec() const {
    return cap->spec();
}

double CapacitoDumpValueDecoratorBase::allowed_current(double f) const {
    return cap->allowed_current(f);
}

CapacitoDumpValueDecorator::CapacitoDumpValueDecorator(CapacitorInterface* cap) 
    : CapacitoDumpValueDecoratorBase(cap) 
{
    
}

double CapacitoDumpValueDecorator::current(double f, double voltage) const 
{
    double current = cap->current(f, voltage);
    // double xc = cap->xc(f);
    // double voltage = current * xc;
    double power = current * voltage;

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) <<
        "Capacitor: " << cap->name() << 
        ", Current: " << current <<
        ", Voltage: " << voltage << 
        ", Power: " << power;
    std::cout << oss.str() << std::endl;
    
    return current;
}

double CapacitoDumpValueDecorator::voltage(double f, double current) const {
    
    double voltage = cap->voltage(f, current);
    // double xc = cap->xc(f);
    // double current = voltage / xc;
    double power = current * voltage;

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) <<
        "Capacitor: " << cap->name() << 
        ", Current: " << current << 
        ", Voltage: " << voltage << 
        ", Power: " << power;
    std::cout << oss.str() << std::endl;

    return voltage;
}




