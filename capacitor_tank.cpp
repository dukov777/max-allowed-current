#include <argparse/argparse.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

#include "capacitors.h"
#include "capacitor_tank.h"


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

    // std::vector<CapacitorSpecification> capacitor_spec = parse_capacitor_specifications(data.capacitor_spec_file);

    // // Calculate the tank capacitors
    // TankCalculator tank_calculator(capacitor_spec);
    // tank_calculator.compose_capacitors_tank(data.group1, data.group2);
    // tank_calculator.calculate_capacitors_tank(data.f, data.i);

    return 0;
}

class TankCalculator
{
    std::unordered_map<std::string, std::unique_ptr<CapacitorSpecification>> stored_specs;
    // std::unique_ptr<MonitorDecorator> serial_tank;
public:
    TankCalculator(std::vector<CapacitorSpecification> &specs);
    void compose_capacitors_tank(std::vector<std::string> &group1, std::vector<std::string> &group2);
    // void calculate_capacitors_tank(float frequency, float current);
};

TankCalculator::TankCalculator(std::vector<CapacitorSpecification> &specs)
{
    for (auto &spec : specs)
    {
        stored_specs[spec.name] = std::make_unique<CapacitorSpecification>(spec);
    }
}


std::vector<Capacitor> cap_1uF_pool({Capacitor(1, 1000, 500), Capacitor(1, 1000, 500), Capacitor(1, 1000, 500), Capacitor(1, 1000, 500)});
std::vector<Capacitor> cap_3_3uF_pool({Capacitor(3.3, 800, 500), Capacitor(3.3, 800, 500), Capacitor(3.3, 800, 500), Capacitor(3.3, 800, 500)});

std::unordered_map<std::string, std::vector<Capacitor>> stored_caps = {
    {"1uF_1000V", cap_1uF_pool},
    {"3.3uF_800V", cap_3_3uF_pool}
};

void TankCalculator::compose_capacitors_tank(
    std::vector<std::string> &group1,
    std::vector<std::string> &group2)
{
    std::vector<CapacitorInterface*> caps1;
    std::vector<CapacitorInterface*> caps2;

    for (auto &name : group1)
    {
        caps1.push_back(&stored_caps[name].back());
        stored_caps[name].pop_back();
    }

    for (auto &name : group2)
    {
        caps2.push_back(&stored_caps[name].back());
        stored_caps[name].pop_back();

    }
    ParallelCapacitor parallel1(caps1, "group1");
    ParallelCapacitor parallel2(caps2, "group1");

    SeriesCapacitor serial({&parallel1, &parallel2});

    // std::vector<std::unique_ptr<CapacitorInterface>> serials;
    // // serials.push_back(std::make_unique<CurrentMonitorDecorator>(std::make_unique<SerialCapacitors>("Serial 1", std::move(caps1))));
    // serials.push_back(std::make_unique<MonitorDecorator>(std::make_unique<ParallelCapacitors>("group1", std::move(caps1))));
    // serials.push_back(std::make_unique<MonitorDecorator>(std::make_unique<ParallelCapacitors>("group2", std::move(caps2))));

    // serial_tank = std::make_unique<MonitorDecorator>(std::make_unique<SerialCapacitors>("Tank Circuit Example", std::move(serials)));
}

// void TankCalculator::calculate_capacitors_tank(float frequency, float current)
// {
//     serial_tank->calculate(frequency, current);
// }
