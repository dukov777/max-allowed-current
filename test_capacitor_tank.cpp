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

TEST(MainFunctionTest, HandlesValidArgs) {
    const char* argv[] = {
    "./calculate-tank-caps", "-i", "2", "-f", "60",
         "-group1", "1uF_1000V", "3.3uF_800V", 
         "-group2", "1uF_1000V", "3.3uF_800V", 
         "-spec", "../capacitors-spec.json"};
    
    int argc = sizeof(argv) / sizeof(char*);

    // Call the main function with the test arguments
    int result = _main_(argc, (char**)argv);

    // Check the result or any other side effects
    EXPECT_EQ(result, 0);
}

TEST(MainFunctionTest, NoGroups) {
    const char* argv[] = {
    "./calculate-tank-caps", "-i", "2", "-f", "60",
         "-spec", "../capacitors-spec.json"};
    
    int argc = sizeof(argv) / sizeof(char*);

    ASSERT_EXIT(_main_(argc, (char**)argv), ::testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST(MainFunctionTest, NoSpecFile) {
    const char* argv[] = {
        "./calculate-tank-caps", "-i", "2", "-f", "60",
            "-group1", "1uF_1000V", "3.3uF_800V", 
            "-group2", "1uF_1000V", "3.3uF_800V", 
            "-spec", "../capacitors-spec.json"};
    
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

    ASSERT_EQ(data.capacitor_spec_file, "../capacitors-spec.json");
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

// std::vector<CapacitorInterface*> capss1(5);
// std::vector<Capacitor> capss(5);

// TEST(CapacitorSpecTest, TestCapacitorssss) {

//     capss1[0] = &capss[0];
//     capss1[1] = &capss[1];

// }


} // namespace
