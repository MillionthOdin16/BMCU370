#pragma once

#include "main.h"
#include "config.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * Filament status enumeration
     */
    enum class AMS_filament_stu
    {
        offline,        ///< Filament not detected or offline
        online,         ///< Filament detected and online
        NFC_waiting     ///< Waiting for NFC data read
    };

    /**
     * Filament motion state enumeration  
     */
    enum class AMS_filament_motion
    {
        before_pull_back,   ///< Preparing to retract filament
        need_pull_back,     ///< Needs to retract filament
        need_send_out,      ///< Needs to feed filament
        on_use,             ///< Currently in use
        idle                ///< Idle state
    };

    /**
     * BambuBus communication package types
     */
    enum class BambuBus_package_type
    {
        ERROR = -1,                 ///< Communication error
        NONE = 0,                   ///< No data
        filament_motion_short,      ///< Short motion command
        filament_motion_long,       ///< Long motion command
        online_detect,              ///< Online detection
        REQx6,                      ///< Request type 6
        NFC_detect,                 ///< NFC detection
        set_filament_info,          ///< Set filament information
        MC_online,                  ///< Multi-color unit online
        read_filament_info,         ///< Read filament information
        set_filament_info_type2,    ///< Set filament info (type 2)
        version,                    ///< Version information
        serial_number,              ///< Serial number
        heartbeat,                  ///< Heartbeat signal
        ETC,                        ///< Other/miscellaneous
        __BambuBus_package_packge_type_size ///< Enum size marker
    };

    /**
     * BambuBus device type identifiers
     */
    enum BambuBus_device_type
    {
        BambuBus_none = DEVICE_TYPE_NONE,       ///< No device
        BambuBus_AMS = DEVICE_TYPE_AMS,         ///< AMS device
        BambuBus_AMS_lite = DEVICE_TYPE_AMS_LITE ///< AMS Lite device
    };
    // Function declarations
    extern void BambuBus_init();
    extern BambuBus_package_type BambuBus_run();
    extern bool Bambubus_read();
    extern void Bambubus_set_need_to_save();
    extern int get_now_filament_num();
    extern uint16_t get_now_BambuBus_device_type();
    extern void reset_filament_meters(int num);
    extern void add_filament_meters(int num, float meters);
    extern float get_filament_meters(int num);
    extern void set_filament_online(int num, bool if_online);
    extern bool get_filament_online(int num);
    extern AMS_filament_motion get_filament_motion(int num);
    extern void set_filament_motion(int num, AMS_filament_motion motion);
    extern bool BambuBus_if_on_print();

#ifdef __cplusplus
}
#endif