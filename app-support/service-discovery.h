/*
 * Copyright (C) 2018-2019
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v3.0. See the file LICENSE in the top level
 * directory for more details.
 *
 * See AUTHORS.md for complete list of NDN IOT PKG authors and contributors.
 */
#ifndef NDN_APP_SUPPORT_SERVICE_DISCOVERY_H
#define NDN_APP_SUPPORT_SERVICE_DISCOVERY_H

#include "../encode/interest.h"
#include "../encode/data.h"
#include "../util/uniform-time.h"

#ifdef __cplusplus
extern "C" {
#endif

const static uint32_t SD_ADV_INTERVAL = 5000;

/**
 * The structure to represent a NDN service.
 */
typedef struct ndn_service {
  /**
   * a bit vector:
   * index 7 (leftmost) bit. whether initialized. 0: uninitialized 1: initialized
   * index 6 bit. whether to advertise. 0: no adv, 1: adv.
   * index 0-5 bits. The state of the service.
   */
  uint8_t status;
  /**
   * The NDN service ID.
   */
  uint8_t service_id;
} ndn_service_t;

/**
 * The structure to keep the state of one's own services
 */
typedef struct sd_self_state {
  /**
   * The home prefix component
   */
  const name_component_t* home_prefix;
  /**
   * The locator name components of the device
   */
  const name_component_t* device_locator[NDN_NAME_COMPONENTS_SIZE];
  uint8_t device_locator_size;
  /**
   * Device IDs
   */
  ndn_service_t services[NDN_SD_SERVICES_SIZE];
} sd_self_state_t;

/**
 * The structure to keep the cached service information in the system
 */
typedef struct sd_sys_state {
  uint8_t interested_services[NDN_SD_SERVICES_SIZE];
  ndn_name_t cached_services[NDN_SD_SERVICES_SIZE];
  ndn_time_ms_t expire_tps[NDN_SD_SERVICES_SIZE];
} sd_sys_state_t;

/**
 * Init state and load a device's meta info into the state.
 * @param dev_identity_name. Input. The name of a device in the format of
 *   /[home-prefix]/[device-locator], a device-locator could be "/bedroom/sensor1" or "/front-door-lock"
 */
void
ndn_sd_init(const ndn_name_t* dev_identity_name);

/**
 * Add a service provided by self device into the state.
 * @param service_id. Input. Service ID.
 * @param adv. Input. Whether to advertise.
 * @param status_code. Input. The status of the service.
 * @return NDN_SUCCESS if there is no error.
 */
int
sd_add_or_update_self_service(uint8_t service_id, bool adv, uint8_t status_code);

/**
 * Add an interested service type so sd will cache related service provider information.
 * @param service_id. Input. Service ID.
 * @return NDN_SUCCESS if there is no error.
 */
int
sd_add_interested_service(uint8_t service_id);

/**
 * Register the prefixes with corresponding onInterest, onData callbacks.
 * Should be called ONLY ONCE after the service discovery state is initialized.
 */
void
sd_listen();

/**
 * Express an Interest packet to advertise one's own services.
 * @return NDN_SUCCESS(0) if there is no error.
 */
int
sd_start_adv_self_services();

/**
 * Query interested services from the system controller.
 * Usually used at the end of the security bootstrapping.
 * @param service_id. Input. The service ID that the device is interested in.
 * @return NDN_SUCCESS(0) if there is no error.
 */
int
sd_query_sys_services(uint8_t service_id);

/**
 * Express an Interest packet to query the SPs for the service.
 * @param service_id. Input. The service to be queried.
 * @param is_any. Input. If is true, query one SP that can provide the service.
 *   If is false, query all the SPs that can provide the service.
 * @return NDN_SUCCESS(0) if there is no error.
 */
int
sd_query_service(uint8_t service_id, const ndn_name_t* granularity, bool is_any);

#ifdef __cplusplus
}
#endif

#endif // NDN_APP_SUPPORT_SERVICE_DISCOVERY_H
