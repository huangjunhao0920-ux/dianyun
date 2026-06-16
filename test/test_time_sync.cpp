#include <cassert>

#include "time_sync/time_sync.h"

int main() {
    dianyun::TimeSynchronizer sync;
    sync.set_offset_ns("lidar", 100);
    assert(sync.align_timestamp_ns("lidar", 1000) == 1100);
    assert(sync.align_timestamp_ns("imu", 1000) == 1000);
    return 0;
}
