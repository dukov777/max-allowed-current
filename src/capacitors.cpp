#include <vector>
#include <algorithm>
#include <stdexcept>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>
#include <cmath>
#include <memory>

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

const std::string CapacitorBase::name() const {
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


ParallelCapacitor::ParallelCapacitor(const std::string &cap_name, CapacitorInterface *cap1, CapacitorInterface *cap2, CapacitorInterface *cap3, CapacitorInterface *cap4, CapacitorInterface *cap5) 
    : capacitors({cap1, cap2, cap3, cap4, cap5})
{
    _cap_name = cap_name;
    double cap_uF = 0;
    // vmax is initialized to infinity
    double vmax = std::numeric_limits<double>::infinity();
    double imax = 0;
    double power_max = 0;
    
    for(auto cap: capacitors)
    {
        if(cap != nullptr){
            cap_uF += cap->spec().get_cap_uF();
            // get the minimum of the maximum voltage
            vmax = vmax < cap->spec().get_v_max() ? vmax : cap->spec().get_v_max();
            imax += cap->spec().get_i_max();
            power_max += cap->spec().get_power_max();
        }
    }

    _spec = CapacitorSpec(cap_uF, vmax, imax, power_max);
}

double ParallelCapacitor::xc(double f) const {
    double reciprocal = std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                                        [f](double sum, CapacitorInterface* cap) { return (cap != nullptr) ? sum + 1.0 / cap->xc(f) : sum; });
    return 1.0 / reciprocal;
}

double ParallelCapacitor::current(double f, double voltage) const {
    return std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                            [f, voltage](double sum, CapacitorInterface* cap) { return (cap != nullptr) ? sum + cap->current(f, voltage) : sum;});
}

double ParallelCapacitor::allowed_current(double f) const {
    return spec().get_v_max() / xc(f);
}

double ParallelCapacitor::voltage(double f, double current) const {
    // traverse all capacitors and call the voltage function. 
    // The result is not important here but the side effect is important.
    for(auto cap : capacitors)
    {
        if(cap != nullptr){
            cap->voltage(f, 0.0);
        }
    }
    return current * xc(f);
}

SeriesCapacitor::SeriesCapacitor(const std::string& cap_name, CapacitorPtr& cap1, CapacitorPtr& cap2)
{
    capacitors = {std::move(cap1), std::move(cap2)};

    _cap_name = cap_name;
    
    double cap_uF = 1.0 / std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                        [](double sum, auto& cap) { return sum + 1.0 / cap->spec().get_cap_uF(); });
    double vmax = std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                        [](double sum, auto& cap) { return sum + cap->spec().get_v_max(); });
    double imax = (*std::min_element(capacitors.begin(), capacitors.end(), 
                        [](auto& a, auto& b) { return a->spec().get_i_max() < b->spec().get_i_max(); }))->spec().get_i_max();
    double power_max = std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                        [](double sum, auto& cap) { return sum + cap->spec().get_power_max(); });

    _spec = CapacitorSpec(cap_uF, vmax, imax, power_max);
}


double SeriesCapacitor::xc(double f) const
{
    return std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                            [f](double sum, auto& cap) { return sum + cap->xc(f); });
}

double SeriesCapacitor::current(double f, double voltage) const {
    auto current = voltage / xc(f);
    
    // traverse all capacitors and call the current function. The result is not important here but the side effect is important.
    for(auto& cap: capacitors)
    {
        auto voltage_per_cap = cap->xc(f) * current;
        cap->current(f, voltage_per_cap);
    }
    return current;
}

double SeriesCapacitor::allowed_current(double f) const {
    return (*std::min_element(capacitors.begin(), capacitors.end(), 
                            [f](auto& a, auto& b) { return a->allowed_current(f) < b->allowed_current(f); }))->allowed_current(f);
}

double SeriesCapacitor::voltage(double f, double current) const {
    return std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                            [f, current](double sum, auto& cap) { return sum + cap->voltage(f, current); });
}
