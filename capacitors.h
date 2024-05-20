#pragma once

#include <vector>
#include <algorithm>
#include <stdexcept>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <string>
#include <cmath>

#include <cmath>
#include <string>
#include <sstream>
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
    Capacitor(double cap_uF, double vmax, double imax, double power_max = 0.0, std::string cap_name = "")
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
        return 1 / (2 * M_PI * f * _spec.get_cap_F());
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
class ParallelCapacitor : public Capacitor {
private:
    std::vector<CapacitorInterface*> _capacitors;

private:
    // static function returning the string name
    static const std::string _get_name(const std::string& cap_name, const std::vector<CapacitorInterface*>& capacitors) 
    {
        if(cap_name.empty())
        {
            return "parallel group " 
                + std::to_string((int)std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                    [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_cap_uF(); })) 
                + "uF/" 
                + std::to_string((int)(*std::min_element(capacitors.begin(), capacitors.end(),
                    [](CapacitorInterface* a, CapacitorInterface* b) 
                    { 
                        return a->spec().get_v_max() < b->spec().get_v_max(); }))->spec().get_v_max())
                 + "V";
        }
        else
        {
            return cap_name;
        }
    }
public:
    ParallelCapacitor(const std::vector<CapacitorInterface*>& capacitors, const std::string& cap_name = "")
        : Capacitor(
            std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                            [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_cap_uF(); }),
            (*std::min_element(capacitors.begin(), capacitors.end(),
                              [](CapacitorInterface* a, CapacitorInterface* b) { return a->spec().get_v_max() < b->spec().get_v_max(); }))->spec().get_v_max(),
            std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                            [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_i_max(); }),
            std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                            [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_power_max(); }),
            _get_name(cap_name, capacitors)),
            _capacitors(capacitors) {}

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
        return std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
                               [f](double sum, CapacitorInterface* cap) { return sum + cap->allowed_current(f); });
    }
};

// SeriesCapacitor class definition
class SeriesCapacitor : public Capacitor {
private:
    std::vector<CapacitorInterface*> _capacitors;

    static std::string _get_name(const std::string& cap_name, const std::vector<CapacitorInterface*>& capacitors) {
        if (cap_name.empty()) {
            return "serial group " 
            + std::to_string((int)(1.0 / std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                    [](double sum, CapacitorInterface* cap) { return sum + 1.0 / cap->spec().get_cap_uF(); }))) 
            + "uF/" + std::to_string((int)std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                    [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_v_max(); })) 
            + "V";
        } else {
            return cap_name;
        }
    }

public:
    SeriesCapacitor(const std::vector<CapacitorInterface*>& capacitors, const std::string& cap_name = "")
        : Capacitor(
            1.0 / std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                                  [](double sum, CapacitorInterface* cap) { return sum + 1.0 / cap->spec().get_cap_uF(); }),
            std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                            [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_v_max(); }),
            (*std::min_element(capacitors.begin(), capacitors.end(), 
                             [](CapacitorInterface* a, CapacitorInterface* b) { return a->spec().get_i_max() < b->spec().get_i_max(); }))->spec().get_i_max(),
            std::accumulate(capacitors.begin(), capacitors.end(), 0.0, 
                            [](double sum, CapacitorInterface* cap) { return sum + cap->spec().get_power_max(); }),
            _get_name(cap_name, capacitors)),
            _capacitors(capacitors) {}

    double xc(double f) const override {
        return std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
                               [f](double sum, CapacitorInterface* cap) { return sum + cap->xc(f); });
    }

    double voltage(double f, double current) const override {
        return std::accumulate(_capacitors.begin(), _capacitors.end(), 0.0, 
                               [f, current](double sum, CapacitorInterface* cap) { return sum + cap->voltage(f, current); });
    }

    double allowed_current(double f) const override {
        return (*std::min_element(_capacitors.begin(), _capacitors.end(), 
                                [f](CapacitorInterface* a, CapacitorInterface* b) { return a->allowed_current(f) < b->allowed_current(f); }))->allowed_current(f);
    }
};

// Decorator base class for current violation
class CapacitorMaxCurrentViolationDecoratorBase : public Capacitor {
protected:
    CapacitorInterface* cap;

public:
    CapacitorMaxCurrentViolationDecoratorBase(CapacitorInterface* cap) : 
            Capacitor(cap->spec().get_cap_uF(), 
                cap->spec().get_v_max(), 
                cap->spec().get_i_max(), 
                cap->spec().get_power_max(), 
                cap->name()),
            cap(cap){}

    virtual double xc(double f) const override {
        return cap->xc(f);
    }

    virtual double voltage(double f, double current) const override {
        return cap->voltage(f, current);
    }

    virtual std::string name() const override {
        return cap->name();
    }

    virtual const CapacitorSpec& spec() const override {
        return cap->spec();
    }
};

// Decorator for max current violation
class CapacitorMaxCurrentViolationDecorator : public CapacitorMaxCurrentViolationDecoratorBase {
public:
    CapacitorMaxCurrentViolationDecorator(CapacitorInterface* cap) 
        : CapacitorMaxCurrentViolationDecoratorBase(cap) {}

    virtual double current(double f, double voltage) const override {
        double spec_max_current = cap->spec().get_i_max();
        double max_current = cap->current(f, voltage);
        if (max_current > spec_max_current) {
            throw std::runtime_error("Current " + std::to_string(max_current) + " exceeds maximum current " + std::to_string(spec_max_current) + " of " + cap->name());
        }
        return max_current;
    }
};

// Decorator for ParallelCapacitor validating the maximum current
class ParallelCapacitorMaxCurrentViolationDecorator : public CapacitorMaxCurrentViolationDecorator {
public:
    ParallelCapacitorMaxCurrentViolationDecorator(Capacitor* cap) : CapacitorMaxCurrentViolationDecorator(cap) {}

    double current(double f, double voltage) const override {
        double spec_max_current = cap->spec().get_i_max();
        double max_current = CapacitorMaxCurrentViolationDecorator::current(f, voltage);
        if (max_current > spec_max_current) {
            throw std::runtime_error("Current " + std::to_string(max_current) + " exceeds maximum current " + std::to_string(spec_max_current) + " of " + cap->name());
        }
        return max_current;
    }
};
