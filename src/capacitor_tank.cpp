#include <argparse/argparse.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

#include "capacitors.h"
#include "capacitor_tank.h"
#include "capacitor_violation_check.h"


using json = nlohmann::json;

ProgramData get_commnad_line_params(int argc, char **argv)
{
    ProgramData data;

    // create a new ArgumentParser object
    argparse::ArgumentParser program("calculate-tank-caps");

    // add program arguments
    program.add_argument("-i")
        .help("Current")
        .default_value(0.0f)
        .scan<'g', float>();

    program.add_argument("-f")
        .help("Frequency")
        .default_value(0.0f)
        .scan<'g', float>();

    program.add_argument("-group1")
        .help("Capacitors in group 1. Minimum 1 capacitors required. Maximum 5 capacitors allowed.")
        .nargs(1, 5)
        .action([](const std::string &value)
                { return value; });

    program.add_argument("-group2")
        .help("Capacitors in group 2. Minimum 1 capacitors required. Maximum 5 capacitors allowed.")
        .nargs(1, 5)
        .action([](const std::string &value)
                { return value; });

    program.add_argument("-spec")
        .help("Path to capacitor specification file")
        .default_value(std::string("capacitors-spec.json"));

    try
    {
        // Parse the command line arguments
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error &err)
    {
        // dump help message and exit
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        exit(EXIT_FAILURE);
    }

    // Get the CLI values
    data.i = program.get<float>("-i");
    data.f = program.get<float>("-f");
    data.group1 = program.get<std::vector<std::string>>("-group1");
    data.group2 = program.get<std::vector<std::string>>("-group2");

    data.capacitor_spec_file = program.get<std::string>("-spec");

    return data;
}

CapacitorSpecification parse_component(const json &j)
{
    CapacitorSpecification comp;
    comp.capacitance = j.at("capacitance").get<float>();
    comp.current = j.at("current").get<float>();
    comp.name = j.at("name").get<std::string>();
    comp.power = j.at("power").get<float>();
    comp.voltage = j.at("voltage").get<float>();
    return comp; // Use std::move to enable move semantics
}

std::vector<CapacitorSpecification> parse_capacitor_specifications(json& json_data)
{
    std::vector<CapacitorSpecification> capacitor_spec;
    try
    {
        for (const auto &item : json_data)
        {
            capacitor_spec.emplace_back(parse_component(item));
        }
    }
    catch (json::exception &e)
    {
        std::cerr << "Error parsing capacitor specification file: " << e.what() << std::endl;
        return capacitor_spec;
    }

    return capacitor_spec;
}

std::vector<CapacitorSpecification> parse_capacitor_specifications_file(const std::string &filepath)
{
    std::vector<CapacitorSpecification> capacitor_spec;

    // read the capacitor specification file
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open capacitor specification file." << std::endl;
        exit(EXIT_FAILURE);
    }

    // parse the json data
    json json_data;
    try
    {
        file >> json_data;
        capacitor_spec = parse_capacitor_specifications(json_data);
    }
    catch (json::parse_error &e)
    {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return capacitor_spec;
    }

    return capacitor_spec;
}

TankCalculator::TankCalculator(std::vector<CapacitorSpecification> &specs)
{
    capacitors_group1.reserve(5);
    capacitors_group2.reserve(5);

    for (auto &spec : specs)
    {
        stored_specs[spec.name] = std::make_unique<CapacitorSpecification>(spec);
    }
}

void TankCalculator::compose_capacitors_tank(
    std::vector<std::string> &group1,
    std::vector<std::string> &group2)
{
    for (auto &name : group1)
    {
        if(stored_specs.find(name) == stored_specs.end())
        {
            std::cerr << "Error: Capacitor " << name << " not found in the specification file." << std::endl;
            exit(EXIT_FAILURE);
        }

        auto cap = Capacitor(
                stored_specs[name]->capacitance*1e6,
                stored_specs[name]->voltage,
                stored_specs[name]->current,
                stored_specs[name]->power,
                stored_specs[name]->name);
        
        capacitors_group1.push_back(cap);

        caps1.push_back(&capacitors_group1[0]);
    }

    parallel1 = ParallelCapacitor(caps1, "group1");

    for (auto &name : group2)
    {
        if(stored_specs.find(name) == stored_specs.end())
        {
            std::cerr << "Error: Capacitor " << name << " not found in the specification file." << std::endl;
            exit(EXIT_FAILURE);
        }

        auto cap = Capacitor(
                stored_specs[name]->capacitance*1e6,
                stored_specs[name]->voltage,
                stored_specs[name]->current,
                stored_specs[name]->power,
                stored_specs[name]->name);
        
        capacitors_group2.push_back(cap);

        caps2.push_back(&capacitors_group2[0]);
    }
    
    parallel2 = ParallelCapacitor(caps2, "group2");
}

double TankCalculator::calculate_capacitors_tank(float frequency, float current)
{
    CapacitorMaxViolationCheckDecorator current_violation_checker1(&parallel1);
    CapacitorMaxViolationCheckDecorator current_violation_checker2(&parallel2);

    std::vector<CapacitorInterface*> serials;
    serials.push_back(&current_violation_checker1);
    serials.push_back(&current_violation_checker2);

    SeriesCapacitor serial(serials, "Tank Circuit Example");
    return serial.current(frequency, current);
}

double TankCalculator::calculate_allowed_current(float frequency)
{
    std::vector<CapacitorInterface*> serials;
    serials.push_back(&parallel1);
    serials.push_back(&parallel2);

    SeriesCapacitor serial(serials, "Tank Circuit Example");
    return serial.allowed_current(frequency);
}

int _main_(int argc, char **argv)
{
    // get the command line parameters
    ProgramData data = get_commnad_line_params(argc, argv);

    // validate constraints on the input data
    if (data.group1.size() < 1 || data.group1.size() > 5)
    {
        std::cerr << "Error: Capacitors in group 1. Minimum 1 capacitors required. Maximum 5 capacitors allowed." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (data.group2.size() < 1 || data.group2.size() > 5)
    {
        std::cerr << "Error: Capacitors in group 2. Minimum 1 capacitors required. Maximum 5 capacitors allowed." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<CapacitorSpecification> capacitor_spec = parse_capacitor_specifications_file(data.capacitor_spec_file);

    // Calculate the tank capacitors
    TankCalculator tank_calculator(capacitor_spec);
    tank_calculator.compose_capacitors_tank(data.group1, data.group2);
    tank_calculator.calculate_capacitors_tank(data.f, data.i);
    auto allowed_current = tank_calculator.calculate_allowed_current(data.f);
    std::cout << "Allowed current: " << allowed_current << std::endl;

    return 0;
}

