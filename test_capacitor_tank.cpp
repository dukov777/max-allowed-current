#include <vector>
#include <stdexcept>
#include <cmath>
// #include "capacitors.h"  // Assuming the translated classes are defined in this header file
#include "capacitor_tank.h"
#include "capacitors.h"


#include "gtest/gtest.h"
namespace {


TEST(TankTest, TestCapacitor) {
    ASSERT_EQ(1, 1);
}

// TEST(MainFunctionTest, HandlesValidArgs) {
//     const char* argv[] = {
//     "./calculate-tank-caps", "-i", "2", "-f", "60",
//          "-group1", "1uF_1000V", "3.3uF_800V", 
//          "-group2", "1uF_1000V", "3.3uF_800V", 
//          "-spec", "capacitors-spec.json"};
    
//     int argc = sizeof(argv) / sizeof(char*);

//     // Call the main function with the test arguments
//     int result = _main_(argc, (char**)argv);

//     // Check the result or any other side effects
//     EXPECT_EQ(result, 0);
// }

TEST(MainFunctionTest, NoGroups) {
    const char* argv[] = {
    "./calculate-tank-caps", "-i", "2", "-f", "60",
         "-spec", "capacitors-spec.json"};
    
    int argc = sizeof(argv) / sizeof(char*);

    ASSERT_EXIT(_main_(argc, (char**)argv), ::testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST(MainFunctionTest, NoSpecFile) {
    const char* argv[] = {
        "./calculate-tank-caps", "-i", "2", "-f", "60",
            "-group1", "1uF_1000V", "3.3uF_800V", 
            "-group2", "1uF_1000V", "3.3uF_800V", 
            "-spec", "capacitors-spec.json"};
    
    int argc = sizeof(argv) / sizeof(char*);
    
    ProgramData data = get_commnad_line_params(argc, (char**)argv);

    ASSERT_EQ(data.i, 2);
    ASSERT_EQ(data.f, 60);
    ASSERT_EQ(data.group1.size(), 2);
    ASSERT_EQ(data.group1[0], "1uF_1000V");
    ASSERT_EQ(data.group1[1], "3.3uF_800V");

    ASSERT_EQ(data.group2.size(), 2);
    ASSERT_EQ(data.group2[0], "1uF_1000V");
    ASSERT_EQ(data.group2[1], "3.3uF_800V");

    ASSERT_EQ(data.capacitor_spec_file, "capacitors-spec.json");
}

TEST(MainFunctionTest, NoGroupsSpecFile) {
    const char* argv[] = {
        "./calculate-tank-caps", "-i", "2", "-f", "60",
            "-spec", "../capacitors-spec.json"};
    
    int argc = sizeof(argv) / sizeof(char*);
    
    ProgramData data = get_commnad_line_params(argc, (char**)argv);

    ASSERT_EQ(data.i, 2);
    ASSERT_EQ(data.f, 60);
    ASSERT_EQ(data.group1.size(), 0);
    ASSERT_EQ(data.group2.size(), 0);
    ASSERT_EQ(data.capacitor_spec_file, "../capacitors-spec.json");
}

TEST(MainFunctionTest, JSON_Parse){
    std::stringstream ss(R"([{"capacitance": 1, "current": 1000, "name": "1uF_1000V", "power": 500, "voltage": 1000},
                                {"capacitance": 3.3, "current": 800, "name": "3.3uF_800V", "power": 500, "voltage": 800}])");
    
    nlohmann::json json_data;
    ss >> json_data;

    std::vector<CapacitorSpecification> capacitor_spec = parse_capacitor_specifications(json_data);

    ASSERT_EQ(capacitor_spec.size(), 2);
    ASSERT_EQ(capacitor_spec[0].capacitance, 1);
    ASSERT_EQ(capacitor_spec[0].current, 1000);
    ASSERT_EQ(capacitor_spec[0].name, "1uF_1000V");
    ASSERT_EQ(capacitor_spec[0].power, 500);
    ASSERT_EQ(capacitor_spec[0].voltage, 1000);

    ASSERT_NEAR(capacitor_spec[1].capacitance, 3.3, 1e-0);
    ASSERT_EQ(capacitor_spec[1].current, 800);
    ASSERT_EQ(capacitor_spec[1].name, "3.3uF_800V");
    ASSERT_EQ(capacitor_spec[1].power, 500);
    ASSERT_EQ(capacitor_spec[1].voltage, 800);
}


TEST(CapacitorSpecTest, TestCapacitorssss) 
{
    std::vector<Capacitor> capacitors_group1(5);
    std::vector<Capacitor> capacitors_group2(5);

    capacitors_group1[0] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
    capacitors_group1[1] = Capacitor(3.3, 800, 500, 500e3, "3.3uF_800V");

    std::vector<CapacitorInterface*> cap;
    cap.reserve(5);
    cap.push_back(&capacitors_group1[0]);
    
    ASSERT_EQ(cap.size(), 1);

    // check if the first element is not null
    ASSERT_NE(cap[0], nullptr);
    ASSERT_EQ(cap[0]->spec().get_v_max(), 1000);
    ASSERT_EQ(cap[0]->spec().get_i_max(), 500);
    ASSERT_EQ(cap[0]->spec().get_power_max(), 500e3);
    ASSERT_EQ(cap[0]->spec().get_cap_uF(), 1);

}

TEST(CapacitorSpecTest, TestTank1) 
{
    double f = 1;

    std::vector<Capacitor> capacitors_group1(5);
    std::vector<Capacitor> capacitors_group2(5);

    capacitors_group1[0] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
    capacitors_group1[1] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");

    std::vector<CapacitorInterface*> group1;
    group1.reserve(5);

    group1.push_back(&capacitors_group1[0]);
    group1.push_back(&capacitors_group1[1]);


    ParallelCapacitor parallel1(group1);
    
    ASSERT_NEAR(parallel1.current(f, 1000), 0.012566, 1e-4);

    capacitors_group2[0] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
    capacitors_group2[1] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");

    std::vector<CapacitorInterface*> group2;
    group2.reserve(5);

    group2.push_back(&capacitors_group2[0]);
    group2.push_back(&capacitors_group2[1]);


    ParallelCapacitor parallel2(group2);
    
    ASSERT_NEAR(parallel2.current(f, 1000), 0.012566, 1e-4);

    std::vector<CapacitorInterface*> ser1;
    ser1.reserve(2);
    
    ser1.push_back(&parallel1);
    ser1.push_back(&parallel2);
    SeriesCapacitor series(ser1);
    
    ASSERT_NEAR(series.current(f, 1000), 0.012566/2, 1e-4);
}

TEST(MainFunctionTest, NoSpecFiledddddddddddddddddddddd) {

    std::stringstream ss(R"([{"capacitance": 1, "current": 1000, "name": "1uF_1000V", "power": 500, "voltage": 1000},
                                {"capacitance": 3.3, "current": 800, "name": "3.3uF_800V", "power": 500, "voltage": 800}])");
    
    nlohmann::json json_data;
    ss >> json_data;

    std::vector<CapacitorSpecification> capacitor_spec = parse_capacitor_specifications(json_data);

    ASSERT_EQ(capacitor_spec.size(), 2);
    ASSERT_EQ(capacitor_spec[0].capacitance, 1);
    ASSERT_EQ(capacitor_spec[0].current, 1000);
    ASSERT_EQ(capacitor_spec[0].name, "1uF_1000V");
    ASSERT_EQ(capacitor_spec[0].power, 500);
    ASSERT_EQ(capacitor_spec[0].voltage, 1000);

    ASSERT_NEAR(capacitor_spec[1].capacitance, 3.3, 1e-0);
    ASSERT_EQ(capacitor_spec[1].current, 800);
    ASSERT_EQ(capacitor_spec[1].name, "3.3uF_800V");
    ASSERT_EQ(capacitor_spec[1].power, 500);
    ASSERT_EQ(capacitor_spec[1].voltage, 800);

    TankCalculator tank_calculator(capacitor_spec);

    std::vector<std::string> group1 = {"1uF_1000V", "3.3uF_800V"};
    std::vector<std::string> group2 = {"1uF_1000V", "3.3uF_800V"};

    tank_calculator.compose_capacitors_tank(group1, group2);
    ASSERT_ANY_THROW(tank_calculator.calculate_capacitors_tank(10000000, 1000));
    // auto current = tank_calculator.calculate_capacitors_tank(10000000, 1000);
    // ASSERT_NEAR(current, 0.006283185, 1e-4);
}

} // namespace
