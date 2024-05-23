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

ParallelCapacitor::ParallelCapacitor(const std::string &cap_name, CapacitorInterface *cap1, CapacitorInterface *cap2, CapacitorInterface *cap3, CapacitorInterface *cap4, CapacitorInterface *cap5) 
    : capacitors({cap1, cap2, cap3, cap4, cap5})
{
    _cap_name = cap_name;
}

ParallelCapacitor::ParallelCapacitor(std::array<CapacitorInterface *, 5> *capacitors, const std::string &cap_name)
{
    _capacitors = capacitors;
    _cap_name = cap_name;
}

void ParallelCapacitor::initialize() {
    double cap_uF = 0;
    // vmax is initialized to infinity
    double vmax = std::numeric_limits<double>::infinity();
    double imax = 0;
    double power_max = 0;
    
    for(auto cap: *_capacitors)
    {
        if(cap != nullptr){
            cap->initialize();
            cap_uF += cap->spec().get_cap_uF();
            // get the minimum of the maximum voltage
            vmax = vmax < cap->spec().get_v_max() ? vmax : cap->spec().get_v_max();
            imax += cap->spec().get_i_max();
            power_max += cap->spec().get_power_max();
        }
    }

    // double cap_uF = std::accumulate(_capacitors->begin(), _capacitors->end(), 0.0, 
    //                     [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_cap_uF(); });
    // double vmax = (*std::min_element(_capacitors->begin(), _capacitors->end(),
    //                     [](CapacitorInterface* a, CapacitorInterface* b) { return a->spec().get_v_max() < b->spec().get_v_max(); }))->spec().get_v_max();
    // double imax = std::accumulate(_capacitors->begin(), _capacitors->end(), 0.0, 
    //                     [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_i_max(); });
    // double power_max = std::accumulate(_capacitors->begin(), _capacitors->end(), 0.0, 
    //                     [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_power_max(); });

    _spec = CapacitorSpec(cap_uF, vmax, imax, power_max);
    _cap_name = _get_name(_cap_name, _spec, "parallel group");
}

double ParallelCapacitor::xc(double f) const {
    // double reciprocal = std::accumulate(_capacitors->begin(), _capacitors->end(), 0.0, 
    //                                     [f](double sum, CapacitorInterface* cap) { return sum + 1.0 / cap->xc(f); });
    
    double sum = 0;
    for(auto cap: *_capacitors)
    {
        if(cap != nullptr){
            sum += 1.0 / cap->xc(f);
        }
    }
    double reciprocal = sum;

    return 1.0 / reciprocal;
}

double ParallelCapacitor::current(double f, double voltage) const {
    // return std::accumulate(_capacitors->begin(), _capacitors->end(), 0.0, 
    //                         [f, voltage](double sum, CapacitorInterface* cap) { return sum + cap->current(f, voltage); });
    double sum = 0;
    for(auto cap: *_capacitors)
    {
        if(cap != nullptr){
            sum += cap->current(f, voltage);;
        }
    }
    return sum;
}

double ParallelCapacitor::allowed_current(double f) const {
    return spec().get_v_max() / xc(f);
}

double ParallelCapacitor::voltage(double f, double current) const {
    // traverse all capacitors and call the voltage function. 
    // The result is not important here but the side effect is important.
    for(auto cap : *_capacitors)
    {
        if(cap != nullptr){
            cap->voltage(f, 0.0);
        }
    }
    return current * xc(f);
}

SeriesCapacitor::SeriesCapacitor(const std::array<CapacitorInterface*, 2>* capacitors, const std::string& cap_name)
        : _capacitors(capacitors)    
{
    _cap_name = cap_name;
}

void SeriesCapacitor::initialize() 
{
    double reciprocal = 0;
    double cap_uF = 0;
    double vmax = 0;
    // imax is initialized to infinity
    double imax = INFINITY;
    double power_max = 0;
    
    for(auto cap: *_capacitors)
    {
        if(cap != nullptr){
            cap->initialize();
            
            reciprocal += 1 / cap->spec().get_cap_uF();
            vmax += cap->spec().get_v_max();
            // get the minimum of the maximum current
            imax = imax < cap->spec().get_i_max() ? imax : cap->spec().get_i_max();
            power_max += cap->spec().get_power_max();
        }
    }
    cap_uF = 1.0 / reciprocal;
    // double cap_uF = 1.0 / std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
    //                     [](double sum, CapacitorInterface* cap) { return sum + 1.0 / cap->spec().get_cap_uF(); });
    // double vmax = std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
    //                     [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_v_max(); });
    // double imax = (*std::min_element(_capacitors.begin(), _capacitors.end(), 
    //                     [](CapacitorInterface* a, CapacitorInterface* b) { return a->spec().get_i_max() < b->spec().get_i_max(); }))->spec().get_i_max();
    // double power_max = std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
    //                     [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_power_max(); });

    _spec = CapacitorSpec(cap_uF, vmax, imax, power_max);
    _cap_name = _get_name(_cap_name, _spec, "serial group");
}

double SeriesCapacitor::xc(double f) const {
    // return std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
    //                         [f](double sum, CapacitorInterface* cap) { return sum + cap->xc(f); });
    double xc = 0;
    for(auto cap: *_capacitors)
    {
        if(cap != nullptr){
            xc += cap->xc(f);
        }
    }
    return xc;
}

double SeriesCapacitor::current(double f, double voltage) const {
    auto current = voltage / xc(f);
    
    // traverse all capacitors and call the current function. The result is not important here but the side effect is important.
    for(auto cap: *_capacitors)
    {
        if(cap != nullptr){
            auto voltage_per_cap = cap->xc(f) * current;
            cap->current(f, voltage_per_cap);
        }
    }
    return current;
}

double SeriesCapacitor::allowed_current(double f) const {
    // return (*std::min_element(_capacitors.begin(), _capacitors.end(), 
    //                         [f](CapacitorInterface* a, CapacitorInterface* b) { return a->allowed_current(f) < b->allowed_current(f); }))->allowed_current(f);
    double allowed_current = INFINITY;
    for(auto cap: *_capacitors)
    {
        if(cap != nullptr){
            allowed_current = allowed_current < cap->allowed_current(f) ? allowed_current : cap->allowed_current(f);
        }
    }
    return allowed_current;
}

double SeriesCapacitor::voltage(double f, double current) const {
    // return std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
    //                         [f, current](double sum, CapacitorInterface* cap) { return sum + cap->voltage(f, current); });
    double sum = 0;
    for(auto cap: *_capacitors)
    {
        if(cap != nullptr){
            sum += cap->voltage(f, current);
        }
    }
    return sum;
}
