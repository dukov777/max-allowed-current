#pragma once

#include <memory>
#include <string>
#include <array>
#include <cstring>
#include <stdexcept>

#include "CapacitorInterface.h"

// template <typename T>
// class CapacitorDeleter {
// public:
//     void operator()(CapacitorInterface* ptr) const;
// };

class CapacitorDeleter {
public:
    void operator()(CapacitorInterface* ptr) const;
};


typedef std::unique_ptr<CapacitorInterface, CapacitorDeleter> CapacitorPtr;

// typedef std::unique_ptr<CapacitorInterface> CapacitorPtr;

class CapacitorFactory
{
public:
    static CapacitorPtr
    create_single(double cap_uF, double vmax, double imax, double power_max, std::string cap_name = "");

    static CapacitorPtr
    create_parallel(const std::string& cap_name, CapacitorInterface* cap1, CapacitorInterface* cap2 = nullptr, CapacitorInterface* cap3 = nullptr, CapacitorInterface* cap4 = nullptr, CapacitorInterface* cap5 = nullptr);

    static CapacitorPtr
    create_parallel(const std::string& cap_name, CapacitorPtr& cap1);

    static CapacitorPtr
    create_parallel(const std::string& cap_name, CapacitorPtr& cap1, CapacitorPtr& cap2);

    static CapacitorPtr
    create_parallel(const std::string& cap_name, CapacitorPtr& cap1, CapacitorPtr& cap2, CapacitorPtr& cap3);

    static CapacitorPtr
    create_parallel(const std::string& cap_name, CapacitorPtr& cap1, CapacitorPtr& cap2, CapacitorPtr& cap3, CapacitorPtr& cap4);

    static CapacitorPtr
    create_parallel(const std::string& cap_name, CapacitorPtr& cap1, CapacitorPtr& cap2, CapacitorPtr& cap3, CapacitorPtr& cap4, CapacitorPtr& cap5);
};

