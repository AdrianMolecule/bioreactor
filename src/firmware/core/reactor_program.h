#include <memory>

#include "sensor_state.h"
#include "reactor_state.h"

void program_run(std::unique_ptr<SensorState>& sensors, std::shared_ptr<ReactorState> reactor);
