#include <vector>
#include <algorithm>
#include <stdexcept>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>
#include <cmath>

#include "capacitors.h"

double CapacitorBase::xc(double f) const {
    return 1 / (2 * M_PI * f * spec().get_cap_F());
}

double CapacitorBase::current(double f, double voltage) const {
    return voltage / xc(f);
}

double CapacitorBase::allowed_current(double f) const {
    double vmax = spec().get_v_max();
    return vmax / xc(f);
}

double CapacitorBase::voltage(double f, double current) const {
    return current * xc(f);
}

const CapacitorSpec& CapacitorBase::spec() const {
    return _spec;
}

std::string CapacitorBase::name() const {
    return _cap_name;
}

Capacitor::Capacitor(double cap_uF, double vmax, double imax, double power_max, std::string cap_name)
{
    _spec = CapacitorSpec(cap_uF, vmax, imax, power_max);
    if (cap_name.empty()) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(0) << cap_uF << "uF/" << vmax << "V";
        _cap_name = oss.str();
    } else {
        _cap_name = cap_name;
    }
}


// GroupCapacitorBase::GroupCapacitorBase(const std::array<CapacitorInterface*, 5>& capacitors) 
//         : _capacitors(capacitors) 
// {

// }

    // static function returning the string name
const std::string GroupCapacitorBase::_get_name(const std::string& cap_name, const CapacitorSpec& cap_spec, const std::string& type) 
{
    if(cap_name.empty())
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(0) << type
            << cap_spec.get_cap_uF() << "uF/"
            << cap_spec.get_v_max() << "V";
        return oss.str();
    }
    else
    {
        return cap_name;
    }
}

ParallelCapacitor::ParallelCapacitor(const std::array<CapacitorInterface*, 5>& capacitors, const std::string& cap_name) 
        : _capacitors(capacitors)    
{
    _cap_name = cap_name;
    this->initialize();
}

void ParallelCapacitor::initialize() {
    for(auto cap : _capacitors)
    {
        cap->initialize();
    }

    double cap_uF = std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
                        [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_cap_uF(); });
    double vmax = (*std::min_element(_capacitors.begin(), _capacitors.end(),
                        [](CapacitorInterface* a, CapacitorInterface* b) { return a->spec().get_v_max() < b->spec().get_v_max(); }))->spec().get_v_max();
    double imax = std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
                        [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_i_max(); });
    double power_max = std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
                        [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_power_max(); });

    _spec = CapacitorSpec(cap_uF, vmax, imax, power_max);
    _cap_name = _get_name(_cap_name, _spec, "parallel group");
}

double ParallelCapacitor::xc(double f) const {
    double reciprocal = std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
                                        [f](double sum, CapacitorInterface* cap) { return sum + 1.0 / cap->xc(f); });
    return 1.0 / reciprocal;
}

double ParallelCapacitor::current(double f, double voltage) const {
    return std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
                            [f, voltage](double sum, CapacitorInterface* cap) { return sum + cap->current(f, voltage); });
}

double ParallelCapacitor::allowed_current(double f) const {
    return spec().get_v_max() / xc(f);
}

double ParallelCapacitor::voltage(double f, double current) const {
    // traverse all capacitors and call the voltage function. 
    // The result is not important here but the side effect is important.
    for(auto cap : _capacitors)
    {
        cap->voltage(f, 0.0);
    }
    return current * xc(f);
}

SeriesCapacitor::SeriesCapacitor(const std::array<CapacitorInterface*, 2>& capacitors, const std::string& cap_name)
        : _capacitors(capacitors)    
{
    _cap_name = cap_name;
    this->initialize();
}

void SeriesCapacitor::initialize() {
    for(auto cap : _capacitors)
    {
        cap->initialize();
    }
    double cap_uF = 1.0 / std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
                        [](double sum, CapacitorInterface* cap) { return sum + 1.0 / cap->spec().get_cap_uF(); });
    double vmax = std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
                        [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_v_max(); });
    double imax = (*std::min_element(_capacitors.begin(), _capacitors.end(), 
                        [](CapacitorInterface* a, CapacitorInterface* b) { return a->spec().get_i_max() < b->spec().get_i_max(); }))->spec().get_i_max();
    double power_max = std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
                        [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_power_max(); });

    _spec = CapacitorSpec(cap_uF, vmax, imax, power_max);
    _cap_name = _get_name(_cap_name, _spec, "serial group");
}

double SeriesCapacitor::xc(double f) const {
    return std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
                            [f](double sum, CapacitorInterface* cap) { return sum + cap->xc(f); });
}

double SeriesCapacitor::current(double f, double voltage) const {
    auto current = voltage / xc(f);
    
    // traverse all capacitors and call the current function. The result is not important here but the side effect is important.
    for(auto cap : _capacitors)
    {
        auto voltage_per_cap = cap->xc(f) * current;
        cap->current(f, voltage_per_cap);
    }
    return current;
}

double SeriesCapacitor::allowed_current(double f) const {
    return (*std::min_element(_capacitors.begin(), _capacitors.end(), 
                            [f](CapacitorInterface* a, CapacitorInterface* b) { return a->allowed_current(f) < b->allowed_current(f); }))->allowed_current(f);
}

double SeriesCapacitor::voltage(double f, double current) const {
    return std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
                            [f, current](double sum, CapacitorInterface* cap) { return sum + cap->voltage(f, current); });
}


