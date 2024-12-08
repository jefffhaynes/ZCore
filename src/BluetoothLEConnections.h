#pragma once

#include "Array.h"
#include <zephyr/bluetooth/bluetooth.h>

inline Array<bt_conn*, CONFIG_BT_MAX_CONN> BluetoothLEConnections;
