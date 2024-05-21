#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include "capacitors.h"

using json = nlohmann::json;


int _main_(int argc, char **argv);

struct ProgramData
{
    float i;
    float f;
    std::vector<std::string> group1;
    std::vector<std::string> group2;
    std::string capacitor_spec_file;
};

struct CapacitorSpecification
{
    float capacitance;
    float current;
    std::string name;
    float power;
    float voltage;
};

ProgramData get_commnad_line_params(int argc, char **argv);
std::vector<CapacitorSpecification> parse_capacitor_specifications(json& json_data);

class TankCalculator
{
    ParallelCapacitor parallel1;
    ParallelCapacitor parallel2;
    
    // Pointers are initialize during composition with the value of coresponding capacitors in parallel1 and parallel2. 
    std::vector<CapacitorInterface*> caps1;
    std::vector<CapacitorInterface*> caps2;

    // Pool of capacitor objects, created during Contrutor and initialized during composition.
    std::vector<Capacitor> capacitors_group1;
    std::vector<Capacitor> capacitors_group2;

    std::unordered_map<std::string, std::unique_ptr<CapacitorSpecification>> stored_specs;
    
public:
    TankCalculator(std::vector<CapacitorSpecification> &specs);
    void compose_capacitors_tank(std::vector<std::string> &group1, std::vector<std::string> &group2);
    double calculate_capacitors_tank(float frequency, float current);
    double calculate_allowed_current(float frequency);
};

