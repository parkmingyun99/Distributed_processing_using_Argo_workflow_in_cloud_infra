#pragma once
#include <limits>

/*
 * Port Parameters 
 */
using PortId = int;
constexpr PortId kErrorPort = std::numeric_limits<int>::max();
constexpr PortId kUnknownPort = kErrorPort;

enum PortType { kInPort, kOutPort };

enum ModelType { kUndefModel, kCoupled, kDevsAtomic, kEoAtomic };

/*
 * Simulation Time parameters
 */
using Time = int;
constexpr int kInfinity = std::numeric_limits<int>::max();
constexpr int kUnknownTime = std::numeric_limits<int>::min();
/*
 * Port Parameters 
 */
constexpr int kUnDefPriority = std::numeric_limits<int>::max();

enum class EvType { EV_TYPE_UNKNOWN, COUP_CHANGE_TYPE, EXT_EV_TYPE, INT_EV_TYPE, };


// #define ENG_LOG_TRACE_ENABLE 1
// #define NDEBUG 1
