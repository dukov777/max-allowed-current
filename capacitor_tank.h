#pragma once

#include <nlohmann/json.hpp>
#include <string>

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