#pragma once

#include <vector>
#include <algorithm>
#include <stdexcept>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>

// #include <cmath>
#include <string>
#include <iomanip>

// CapacitorSpec class definition
class CapacitorSpec {
private:
    double cap_uF;
    double cap_F;
    double i_max;
    double v_max;
    double power_max;

public:
    CapacitorSpec() : cap_uF(0.0), cap_F(0.0), i_max(0.0), v_max(0.0), power_max(0.0) {}
    CapacitorSpec(double cap_uF, double v_max, double i_max, double power_max)
        : cap_uF(cap_uF), cap_F(cap_uF * 1e-6), i_max(i_max), v_max(v_max), power_max(power_max) {}

    double get_i_max() const { return i_max; }
    double get_v_max() const { return v_max; }
    double get_power_max() const { return power_max; }
    double get_cap_F() const { return cap_F; }
    double get_cap_uF() const { return cap_uF; }
};

// CapacitorInterface class definition
class CapacitorInterface {
public:
    virtual double xc(double f) const = 0;
    virtual double current(double f, double voltage) const = 0;
    virtual double allowed_current(double f) const = 0;
    virtual double voltage(double f, double current) const = 0;
    virtual const CapacitorSpec& spec() const = 0;
    virtual std::string name() const = 0;

    virtual ~CapacitorInterface() {}
};

// Capacitor class definition
class Capacitor : public CapacitorInterface {
private:
    CapacitorSpec _spec;
    std::string _cap_name;

public:
    Capacitor() : _spec(0, 0, 0, 0), _cap_name(){}

    Capacitor(double cap_uF, double vmax, double imax, double power_max, std::string cap_name = "")
        : _spec(cap_uF, vmax, imax, power_max) {
        if (cap_name.empty()) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(0) << cap_uF << "uF/" << vmax << "V";
            _cap_name = oss.str();
        } else {
            _cap_name = cap_name;
        }
    }

    virtual double xc(double f) const {
        return 1 / (2 * M_PI * f * spec().get_cap_F());
    }

    virtual double current(double f, double voltage) const {
        return voltage / xc(f);
    }

    virtual double allowed_current(double f) const {
        double vmax = spec().get_v_max();
        return vmax / xc(f);
    }

    virtual double voltage(double f, double current) const {
        return current * xc(f);
    }

    virtual const CapacitorSpec& spec() const {
        return _spec;
    }

    virtual std::string name() const {
        return _cap_name;
    }
};

// ParallelCapacitor class definition
class GroupCapacitorBase : public CapacitorInterface 
{
protected:
    std::vector<CapacitorInterface*> _capacitors;
    CapacitorSpec _spec;
    std::string _cap_name;

protected:
    GroupCapacitorBase(){}

    GroupCapacitorBase(const std::vector<CapacitorInterface*>& capacitors) 
        : _capacitors(capacitors) {}

    // static function returning the string name
    static const std::string _get_name(const std::string& cap_name, const CapacitorSpec& cap_spec, const std::string& type = "group") 
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

public:
    double current(double f, double voltage) const override {
        return voltage / xc(f);
    }

    double voltage(double f, double current) const override {
        return current * xc(f);
    }

    const CapacitorSpec& spec() const override {
        return _spec;
    }

    std::string name() const override {
        return _cap_name;
    }

};

// ParallelCapacitor class definition
class ParallelCapacitor : public GroupCapacitorBase
{
public:
    ParallelCapacitor()
    {
    }

    ParallelCapacitor(const std::vector<CapacitorInterface*>& capacitors, const std::string& cap_name = "") 
        : GroupCapacitorBase(capacitors) 
    {
        double cap_uF = std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                            [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_cap_uF(); });
        double vmax = (*std::min_element(capacitors.begin(), capacitors.end(),
                            [](CapacitorInterface* a, CapacitorInterface* b) { return a->spec().get_v_max() < b->spec().get_v_max(); }))->spec().get_v_max();
        double imax = std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                            [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_i_max(); });
        double power_max = std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                            [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_power_max(); });

        _spec = CapacitorSpec(cap_uF, vmax, imax, power_max);
        _cap_name = _get_name(cap_name, _spec, "parallel group");
    }

    double xc(double f) const override {
        double reciprocal = std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
                                            [f](double sum, CapacitorInterface* cap) { return sum + 1.0 / cap->xc(f); });
        return 1.0 / reciprocal;
    }

    double current(double f, double voltage) const override {
        return std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
                               [f, voltage](double sum, CapacitorInterface* cap) { return sum + cap->current(f, voltage); });
    }

    double allowed_current(double f) const override {
        return spec().get_v_max() / xc(f);
    }

    double voltage(double f, double current) const override {
        // traverse all capacitors and call the voltage function. 
        // The result is not important here but the side effect is important.
        for(auto cap : _capacitors)
        {
            cap->voltage(f, 0.0);
        }
        return current * xc(f);
    }
};

// SeriesCapacitor class definition
class SeriesCapacitor : public GroupCapacitorBase {
public:
    SeriesCapacitor(const std::vector<CapacitorInterface*>& capacitors, const std::string& cap_name = "")
        : GroupCapacitorBase(capacitors) 
    {
        double cap_uF = 1.0 / std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                            [](double sum, CapacitorInterface* cap) { return sum + 1.0 / cap->spec().get_cap_uF(); });
        double vmax = std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                            [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_v_max(); });
        double imax = (*std::min_element(capacitors.begin(), capacitors.end(), 
                            [](CapacitorInterface* a, CapacitorInterface* b) { return a->spec().get_i_max() < b->spec().get_i_max(); }))->spec().get_i_max();
        double power_max = std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                            [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_power_max(); });

        _spec = CapacitorSpec(cap_uF, vmax, imax, power_max);
        _cap_name = _get_name(cap_name, _spec, "serial group");
    }

    double xc(double f) const override {
        return std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
                               [f](double sum, CapacitorInterface* cap) { return sum + cap->xc(f); });
    }

    double current(double f, double voltage) const override {
        auto current = voltage / xc(f);
        
        // traverse all capacitors and call the current function. The result is not important here but the side effect is important.
        for(auto cap : _capacitors)
        {
            auto voltage_per_cap = cap->xc(f) * current;
            cap->current(f, voltage_per_cap);
        }
        return current;
    }

    double allowed_current(double f) const override {
        return (*std::min_element(_capacitors.begin(), _capacitors.end(), 
                                [f](CapacitorInterface* a, CapacitorInterface* b) { return a->allowed_current(f) < b->allowed_current(f); }))->allowed_current(f);
    }

    double voltage(double f, double current) const override {
        return std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
                               [f, current](double sum, CapacitorInterface* cap) { return sum + cap->voltage(f, current); });
    }
};


// Decorator base class for current violation
class CapacitorMaxCurrentViolationDecoratorBase : public CapacitorInterface {
protected:
    CapacitorInterface* cap;

public:
    CapacitorMaxCurrentViolationDecoratorBase(CapacitorInterface* cap) : cap(cap){}

    virtual double xc(double f) const override {
        return cap->xc(f);
    }

    virtual std::string name() const override {
        return cap->name();
    }

    virtual const CapacitorSpec& spec() const override {
        return cap->spec();
    }

    virtual double allowed_current(double f) const override {
        return cap->allowed_current(f);
    }
};


// Decorator for max current violation
class CapacitorMaxCurrentViolationDecorator : public CapacitorMaxCurrentViolationDecoratorBase {
public:
    CapacitorMaxCurrentViolationDecorator(CapacitorInterface* cap) 
        : CapacitorMaxCurrentViolationDecoratorBase(cap) {}

    virtual double current(double f, double voltage) const override {
        double spec_max_current = cap->spec().get_i_max();
        double current = cap->current(f, voltage);
        if (current > spec_max_current) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(0) << 
                "Current " << current <<
                " exceeds maximum current " << spec_max_current << 
                " of " + cap->name();
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
                "Power " << current * voltage << 
                " exceeds maximum power " << spec_max_power <<
                " of " << cap->name();
            #ifdef LOG_CONSOLE
                std::cout << oss.str() << std::endl;
            #else
                throw std::runtime_error(oss.str());
            #endif
        }

        return current;
    }

    virtual double voltage(double f, double current) const override {
        
        double spec_max_voltage = cap->spec().get_v_max();
        double voltage = cap->voltage(f, current);
        
        if (voltage > spec_max_voltage) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(0) << 
                "Voltage " << voltage << 
                " exceeds maximum voltage " << spec_max_voltage << 
                " of " << cap->name();
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
                "Power " << current * voltage << 
                " exceeds maximum power " << spec_max_power << 
                " of " << cap->name();
            
            #ifdef LOG_CONSOLE
                std::cout << oss.str() << std::endl;
            #else
                throw std::runtime_error(oss.str());
            #endif
        }
        return voltage;
    }
};
