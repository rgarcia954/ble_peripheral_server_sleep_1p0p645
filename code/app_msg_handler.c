/**
 * @file app_msg_handler.c
 * @brief Message Handler source file
 *
 * @copyright @parblock
 * Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee.
 *
 * This is Reusable Code.
 * @endparblock
 */

#include <app.h>

uint8_t app_adv_data[ADV_DATA_LEN], app_scan_rsp_data[ADV_DATA_LEN];
uint8_t app_adv_data_len, app_scan_rsp_data_len;

cust_svc_desc app_cust_svc_db[APP_NUM_CUST_SVC];
uint16_t app_disc_svc_count[APP_MAX_NB_CON];

GAPM_ActivityStatus_t advActivityStatus;

struct gapm_set_dev_config_cmd devConfigCmd =
{
    .operation = GAPM_SET_DEV_CONFIG,
    .role = GAP_ROLE_ALL,
    .renew_dur = APP_BD_RENEW_DUR,
    .addr.addr = APP_BLE_PRIVATE_ADDR,
    .irk.key = APP_IRK,
#if SECURE_CONNECTION
    .pairing_mode = (GAPM_PAIRING_SEC_CON | GAPM_PAIRING_LEGACY),
#else
    .pairing_mode = GAPM_PAIRING_LEGACY,
#endif
    .privacy_cfg = (GAPM_PRIVACY_TYPE | GAPM_ADDRESS_TYPE),
    .gap_start_hdl = GAPM_DEFAULT_GAP_START_HDL,
    .gatt_start_hdl = GAPM_DEFAULT_GATT_START_HDL,
    .att_cfg = GAPM_DEFAULT_ATT_CFG,
    .sugg_max_tx_octets = GAPM_DEFAULT_TX_OCT_MAX,
    .sugg_max_tx_time = GAPM_DEFAULT_TX_TIME_MAX,
    .max_mtu = GAPM_DEFAULT_MTU_MAX,
    .max_mps = GAPM_DEFAULT_MPS_MAX,
    .max_nb_lecb = GAPM_DEFAULT_MAX_NB_LECB,
    .audio_cfg = GAPM_DEFAULT_AUDIO_CFG,
    .tx_pref_phy = GAP_PHY_ANY,
    .rx_pref_phy = GAP_PHY_ANY
};

struct gapm_adv_create_param advParam =
{
    .type = GAPM_ADV_TYPE_LEGACY,
    .disc_mode = APP_ADV_DISCOVERY_MODE,
    .prop = APP_ADV_PROPERTIES,
    .filter_pol = ADV_ALLOW_SCAN_ANY_CON_ANY,
	.max_tx_pwr = DEF_TX_POWER,
    .prim_cfg = {
        .adv_intv_min= APP_ADV_INT_MIN,
        .adv_intv_max= APP_ADV_INT_MAX,
        .chnl_map= APP_ADV_CHMAP,
        .phy = GAPM_PHY_TYPE_LE_1M,
     },
};

union gapc_bond_cfm_data pairingRsp =
{
    .pairing_feat =
    {
        .iocap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT,
        .oob = GAP_OOB_AUTH_DATA_NOT_PRESENT,
        .key_size = KEY_LEN,
        .ikey_dist = (GAP_KDIST_IDKEY | GAP_KDIST_SIGNKEY),
        .rkey_dist = (GAP_KDIST_ENCKEY | GAP_KDIST_IDKEY | GAP_KDIST_SIGNKEY),
        .auth = GAP_AUTH_REQ_SEC_CON_BOND,
        .sec_req = GAP_SEC1_NOAUTH_PAIR_ENC,
    }
};

struct gap_dev_name_buff
{
    uint16_t length;                       /**< name length */
    uint8_t value[APP_DEVICE_NAME_LEN];    /**< name value */
};

const struct gap_dev_name_buff getDevInfoCfmName =
{
    .length = APP_DEVICE_NAME_LEN,
    .value  = APP_DEVICE_NAME
};

const union gapc_dev_info_val getDevInfoCfmAppearance =
{
    .appearance = APP_DEVICE_APPEARANCE
};

const union gapc_dev_info_val getDevInfoCfmSlvParams =
{
    .slv_pref_params = {APP_PREF_SLV_MIN_CON_INTERVAL,
                   APP_PREF_SLV_MAX_CON_INTERVAL,
                   APP_PREF_SLV_LATENCY,
                   APP_PREF_SLV_SUP_TIMEOUT}
};

const union gapc_dev_info_val* getDevInfoCfm[] =
{
    [GAPC_DEV_NAME] = (const union gapc_dev_info_val*) &getDevInfoCfmName,
    [GAPC_DEV_APPEARANCE] = &getDevInfoCfmAppearance,
    [GAPC_DEV_SLV_PREF_PARAMS] = &getDevInfoCfmSlvParams
};

struct ble_device_parameter ble_dev_params =
{
	.low_pwr_clk_accuracy = LOW_POWER_CLOCK_ACCURACY,
	.twosc = TWOSC,
#if defined(LPCLK_SRC_XTAL32) && defined(LPCLK_SRC_RC32)
	.low_pwr_clk =
	{
		.low_pwr_clk_xtal32 = LPCLK_SRC_XTAL32,
		.low_pwr_clk_rc32 = LPCLK_SRC_RC32,
		.low_pwr_standby_clk_src = LPCLK_STANDBYCLK_SRC
	}
#endif
};

static void SetConnectionCfmParams(uint8_t conidx, struct gapc_connection_cfm* cfm);

void BLE_ConfigHandler(ke_msg_id_t const msg_id, void const *param,
                            ke_task_id_t const dest_id,
                            ke_task_id_t const src_id)
{
    switch(msg_id)
    {
        case GAPM_CMP_EVT:
        {
            const struct gapm_cmp_evt *p = param;

            if(p->operation == GAPM_RESET) /* Step 2 */
            {
                swmLogInfo("__GAPM_RESET completed. Setting BLE device configuration...\r\n");

                /* Check privacy_cfg bit 0 to identify address type, public if not set*/
                if(devConfigCmd.privacy_cfg & GAPM_CFG_ADDR_PRIVATE)
                {
                	swmLogInfo("	devConfigCmd address to set static private random\r\n");
                }
                else
                {
                	/* Read Device BLE Public Address */
                	uint8_t ble_dev_addr_len = GAP_BD_ADDR_LEN;
                	uint8_t ble_dev_addr_buf[GAP_BD_ADDR_LEN] = {0};

                	/* Make sure proper BLE public address has been read and saved into ble_public_addr
                	 * using Device_BLE_Public_Address_Read() before calling Device_BLE_Param_Get() */
                	Device_BLE_Param_Get(PARAM_ID_BD_ADDRESS, &ble_dev_addr_len, ble_dev_addr_buf);

                	swmLogInfo("	Device BLE public address read: ");
                	for(int i = 0; i < GAP_BD_ADDR_LEN; i++)
                	{
                		swmLogInfo("0x%02x ", ble_dev_addr_buf[i]);
                	}
                    swmLogInfo("\r\n");

                    swmLogInfo("	devConfigCmd address set to public\r\n");
                	memcpy(devConfigCmd.addr.addr, ble_dev_addr_buf, GAP_BD_ADDR_LEN);
                }

                /* Send a device configuration request to the BLE stack.
                 * The stack sends back a GAPM_CMP_EVT / GAPM_SET_DEV_CONFIG upon completion. */
                GAPM_SetDevConfigCmd(&devConfigCmd);

                /* Set discSvcCount, custom service database and maximum number of custom services
                 * in GATT environment at GAPM_RESET */
                GATT_SetEnvData(app_disc_svc_count, app_cust_svc_db, APP_NUM_CUST_SVC);
            }
            else if(p->operation == GAPM_SET_DEV_CONFIG &&
                    p->status == GAP_ERR_NO_ERROR) /* Step 3 */
            {
                swmLogInfo("__GAPM_SET_DEV_CONFIG completed.\r\n");

                /* Request the stack to add our custom service server to the attribute database.
                 * The stack sends back a GATTM_ADD_SVC_RSP event. */
                GATTM_AddAttributeDatabase(CUSTOMSS_GetDatabaseDescription(), CS_NB);
                /* In parallel, the battery service server abstraction (BASS, ble_bass.c)
                 * also monitors this event and adds the standard profile to the database.
                 * See BASS_MsgHandler for details. */
                swmLogInfo("    Adding BLE profiles and custom services...\r\n");

                /* Request the stack to create an advertising activity.
                 * The stack sends back a GAPM_ACTIVITY_CREATED_IND. See ActivityHandler for next steps. */
                swmLogInfo("    Creating Advertising activity...\r\n");
                advParam.max_tx_pwr = tx_power_level_dbm;
                GAPM_ActivityCreateAdvCmd(&advActivityStatus, GAPM_STATIC_ADDR, &advParam);
            }
        }
        break;
        case GAPM_PROFILE_ADDED_IND: /* Step 4 - BASS profile added */
        {
            swmLogInfo("__GAPM_PROFILE_ADDED_IND - profile added count=%d\r\n",
                    GAPM_GetProfileAddedCount());
        }
        break;
        case GATTM_ADD_SVC_RSP: /* Step 5 - Custom service added */
        {
            swmLogInfo("__GATTM_ADD_SVC_RSP - custom service added count=%d\r\n",
                    GATTM_GetServiceAddedCount());
        }
        break;

    }
}

void BLE_ActivityHandler(ke_msg_id_t const msg_id, void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id)
{
    switch(msg_id)
    {
        case GAPM_CMP_EVT:
        {
            const struct gapm_cmp_evt *p = param;
            if (p->operation == GAPM_SET_ADV_DATA) /* Step 7 */
            {
                swmLogInfo("__GAPM_SET_ADV_DATA status = %d. Start advertising activity...\r\n",p->status);
                GAPM_AdvActivityStart(advActivityStatus.actv_idx, 0, 0);

                /* From now on, this device is advertising. Any peer device can
                 * connect, discover services, pair/bond/encrypt, etc.
                 * When a peer device tries to connect, the stack sends back to the application
                 * a GAPC_CONNECTION_REQ_IND. See ConnectionHandler for more details. */
            }
        }
        break;

        case GAPM_ACTIVITY_CREATED_IND: /* Step 6 */
        {
            swmLogInfo("__GAPM_ACTIVITY_CREATED_IND actv_idx = %d. Setting adv and scan data...\r\n",
                    advActivityStatus.actv_idx);

            /* Request the stack to set the advertising and scan response data.
             * The stack sends back a GAPM_CMP_EVT: operation = GAPM_SET_ADV_DATA. */
            GAPM_SetAdvDataCmd(GAPM_SET_SCAN_RSP_DATA, advActivityStatus.actv_idx,
                    app_adv_data_len, app_adv_data);
            GAPM_SetAdvDataCmd(GAPM_SET_ADV_DATA, advActivityStatus.actv_idx,
                    app_adv_data_len, app_adv_data);
        }
        break;

        case GAPM_ACTIVITY_STOPPED_IND: /* Step 9(c) */
        {
            /* The advertising activity is stopped upon receiving
             * connection request. Restart advertising if not connected to
             * maximum number of peers configured for this application */
            if((GAPC_ConnectionCount() < APP_MAX_NB_CON))
            {
                swmLogInfo("__GAPM_ACTIVITY_STOPPED_IND. Restarting advertising...\r\n");
				if(advActivityStatus.state == ACTIVITY_STATE_NOT_STARTED)
				{
					GAPM_AdvActivityStart(advActivityStatus.actv_idx, 0, 0);
				}
            }
        }
        break;
    }
}

void BLE_ConnectionHandler(ke_msg_id_t const msg_id, void const *param,
                            ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    uint8_t conidx = KE_IDX_GET(src_id);
    switch(msg_id)
    {
        case GAPC_CONNECTION_REQ_IND: /* Step 8 */
        {
            const struct gapc_connection_req_ind* p = param;

            swmLogInfo("__GAPC_CONNECTION_REQ_IND conidx=%d\r\n", conidx);

            /* If the peer device address is private resolvable and bond list is not empty */
            if(GAP_IsAddrPrivateResolvable(p->peer_addr.addr, p->peer_addr_type) &&
               BondList_Size() > 0) /* Step 9(a) */
            {
                /* Ask the stack to resolve the address with the IRKs we have in our bond list.
                 * In case of success, the stack returns GAPM_ADDR_SOLVED_IND.
                 * If not successful (not bonded previously) the stack returns GAPM_CMP_EVT /
                 * GAPM_RESOLV_ADDR with status GAP_ERR_NOT_FOUND (see below). */
                GAPM_ResolvAddrCmd(conidx, p->peer_addr.addr);
            }
            else /* Step 9(b) */
            {
                /* Address is not private resolvable or bondlist empty. Confirms connection right away.
                 * If the device was previously bonded, the LTK is included. */
                struct gapc_connection_cfm cfm;
                SetConnectionCfmParams(conidx, &cfm);
                GAPC_ConnectionCfm(conidx, &cfm); /* Send connection confirmation */
            }
        }
        break;

        case GAPC_DISCONNECT_IND:
        {
            swmLogInfo("__GAPC_DISCONNECT_IND: reason = %d\r\n",
                    ((struct gapc_disconnect_ind*)param)->reason);
            /* If advertising activity is stopped, restart advertising while
             * not connected to maximum number of peers for this application */
            if(GAPC_ConnectionCount() == (APP_MAX_NB_CON - 1))
            {
                swmLogInfo("    Restarting advertising...\r\n");
                GAPM_AdvActivityStart(advActivityStatus.actv_idx, 0, 0);
            }
        }
        break;

        case GAPM_ADDR_SOLVED_IND: /* Step 10(a) */
        {
            /* Private address resolution was successful */
            swmLogInfo("__GAPM_ADDR_SOLVED_IND\r\n");
            struct gapc_connection_cfm cfm;
            conidx = KE_IDX_GET(dest_id);
            /* Set confirmation params (including LTK). */
            SetConnectionCfmParams(conidx, &cfm);
            /* Send confirmation */
            GAPC_ConnectionCfm(conidx, &cfm);
        }
        break;

        case GAPM_CMP_EVT: /* Step 10(b) */
        {
            /* Private address resolution couldn't find an IRK that resolves this address.
             * (i.e. peer device not previously bonded). Confirm connection without LTK. */
            const struct gapm_cmp_evt* p = param;
            if((p->operation == GAPM_RESOLV_ADDR) &&
                    (p->status == GAP_ERR_NOT_FOUND))
            {
                struct gapc_connection_cfm cfm;
                conidx = KE_IDX_GET(dest_id);
                SetConnectionCfmParams(conidx, &cfm);
                GAPC_ConnectionCfm(conidx, &cfm);
            }
        }
        break;

        case GAPC_PARAM_UPDATE_REQ_IND: /* Step 11 */
        {
            /* Peer device requested update in connection params. Accept it. */
            GAPC_ParamUpdateCfm(conidx, true, 0xFFFF, 0xFFFF);
            swmLogInfo("GAPC_PARAM_UPDATE_REQ_IND\r\n");
        }
        break;

        case GAPC_GET_DEV_INFO_REQ_IND: /* Step 12 */
        {
            /* Peer device requested information about the device (such as name,
             * appearance, slv pref. params). See getDevInfoCfm for details.  */
            const struct gapc_get_dev_info_req_ind* p = param;
            GAPC_GetDevInfoCfm(conidx, p->req, getDevInfoCfm[p->req]);
            swmLogInfo("GAPC_GET_DEV_INFO_REQ_IND: req = %d\r\n", p->req);
        }
        break;
    }
}

void BLE_PairingHandler(ke_msg_id_t const msg_id, void const *param,
                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    uint8_t conidx = KE_IDX_GET(src_id);
    switch(msg_id)
    {
        case GAPC_BOND_REQ_IND:  /* Step 13(a) - peer device wants to pair. Exchange keys */
        {
            const struct gapc_bond_req_ind* p = param;
            switch (p->request)
            {
                case GAPC_PAIRING_REQ:
                {
                    bool accept = BondList_Size() < BONDLIST_MAX_SIZE;
#if SECURE_CONNECTION
                    if (p->data.auth_req & GAP_AUTH_SEC_CON)
                    {
                        pairingRsp.pairing_feat.auth = GAP_AUTH_REQ_SEC_CON_BOND;
                        pairingRsp.pairing_feat.sec_req = GAP_SEC1_NOAUTH_PAIR_ENC;
                    }
                    else
#endif
                    {
                        pairingRsp.pairing_feat.auth = GAP_AUTH_REQ_NO_MITM_BOND;
                        pairingRsp.pairing_feat.sec_req = GAP_NO_SEC;
                    }
                    swmLogInfo("__GAPC_BOND_REQ_IND / GAPC_PAIRING_REQ: accept = %d conidx=%d\r\n", accept, conidx);
                    GAPC_BondCfm(conidx, GAPC_PAIRING_RSP, accept, &pairingRsp);
                }
                break;

                case GAPC_LTK_EXCH:
                {
                    /* Prepare and send random LTK (legacy only) */
                    swmLogInfo("__GAPC_BOND_REQ_IND / GAPC_LTK_EXCH\r\n");
                    union gapc_bond_cfm_data ltkExch;
                    ltkExch.ltk.ediv = co_rand_hword();
                    for(uint8_t i = 0, i2 = GAP_RAND_NB_LEN; i < GAP_RAND_NB_LEN; i++, i2++)
                    {
                        ltkExch.ltk.randnb.nb[i] = co_rand_byte();
                        ltkExch.ltk.ltk.key[i] = co_rand_byte();
                        ltkExch.ltk.ltk.key[i2] = co_rand_byte();
                    }
                    GAPC_BondCfm(conidx, GAPC_LTK_EXCH, true, &ltkExch); /* Send confirmation */
                }
                break;

                case GAPC_TK_EXCH: /* Prepare and send TK */
                {
                    swmLogInfo("__GAPC_BOND_REQ_IND / GAPC_TK_EXCH\r\n");
                    /* IO Capabilities are set to GAP_IO_CAP_NO_INPUT_NO_OUTPUT in this application.
                     * Therefore TK exchange is NOT performed. It is always set to 0 (Just Works algorithm). */
                }
                break;

                case GAPC_IRK_EXCH:
                {
                    swmLogInfo("__GAPC_BOND_REQ_IND / GAPC_IRK_EXCH\rn");
                    union gapc_bond_cfm_data irkExch;
                    memcpy(irkExch.irk.addr.addr.addr, GAPM_GetDeviceConfig()->addr.addr, GAP_BD_ADDR_LEN);
                    irkExch.irk.addr.addr_type = GAPM_GetDeviceConfig()->privacy_cfg;
                    memcpy(irkExch.irk.irk.key, GAPM_GetDeviceConfig()->irk.key, GAP_KEY_LEN);
                    GAPC_BondCfm(conidx, GAPC_IRK_EXCH, true, &irkExch); /* Send confirmation */
                }
                break;

                case GAPC_CSRK_EXCH:
                {
                    swmLogInfo("__GAPC_BOND_REQ_IND / GAPC_CSRK_EXCH\r\n");
                    union gapc_bond_cfm_data csrkExch;
                    GAPC_BondCfm(conidx, GAPC_CSRK_EXCH, true, &csrkExch); /* Send confirmation */
                }
                break;
            }
        }
        break;

        case GAPC_BOND_IND: /* Step 14(a) - pairing finished */
        {
            const struct gapc_bond_ind* p = param;
            if(p->info == GAPC_PAIRING_SUCCEED)
            {
                swmLogInfo("__GAPC_BOND_IND / GAPC_PAIRING_SUCCEED\r\n");
                GAPC_AddDeviceToBondList(conidx);
            }
            else if(p->info == GAPC_PAIRING_FAILED)
            {
                swmLogError("__GAPC_BOND_IND / GAPC_PAIRING_FAILED reason=%d\r\n", p->data.reason);
            }
        }
        break;

        case GAPC_ENCRYPT_REQ_IND: /* Step 13(b) */
        {
            /* Peer device was bonded previously and wants to encrypt the link.
             * Accept request if the bond information is valid & EDIV/RAND match */
            const struct gapc_encrypt_req_ind* p = param;
            bool found = (GAPC_IsBonded(conidx) &&
                          p->ediv == GAPC_GetBondInfo(conidx)->ediv &&
                          !memcmp(p->rand_nb.nb, GAPC_GetBondInfo(conidx)->rand, GAP_RAND_NB_LEN));

            swmLogInfo("__GAPC_ENCRYPT_REQ_IND: bond information %s\r\n", (found ? "FOUND" : "NOT FOUND"));
            GAPC_EncryptCfm(conidx, found, GAPC_GetBondInfo(conidx)->ltk, GAP_KEY_LEN);
        }
        break;

        case GAPC_ENCRYPT_IND: /* Step 14(b)  */
        {
            swmLogInfo("__GAPC_ENCRYPT_IND: Link encryption is ON\r\n");
        }
        break;
    }
}

static void SetConnectionCfmParams(uint8_t conidx, struct gapc_connection_cfm* cfm)
{
    cfm->svc_chg_handle = 0;
    cfm->ltk_present = false;
#if SECURE_CONNECTION
    cfm->pairing_lvl = GAP_PAIRING_BOND_SECURE_CON;
#else
    cfm->pairing_lvl = GAP_PAIRING_BOND_UNAUTH;
#endif

    cfm->cli_feat = 0;
    cfm->cli_info = 0;
    cfm->gatt_start_handle = 0;
    cfm->gatt_end_handle = 0;
    cfm->svc_chg_handle = 0;

    if(GAPC_IsBonded(conidx))
    {
        cfm->ltk_present = true;
        memcpy(cfm->rcsrk.key, GAPC_GetBondInfo(conidx)->csrk, KEY_LEN);
        cfm->lsign_counter = 0xFFFFFFFF;
        cfm->rsign_counter = 0;
    }
    swmLogInfo("  connectionCfm->ltk_present = %d\r\n", cfm->ltk_present);
}

void PrepareAdvScanData(void)
{
    uint8_t companyID[] = APP_COMPANY_ID;
    uint8_t devName[]   = APP_DEVICE_NAME;

    /* Assemble advertising data as device name + company ID and
     * copy into app_adv_data */
    app_adv_data_len = 0;
    GAP_AddAdvData(APP_DEVICE_NAME_LEN + 1, GAP_AD_TYPE_COMPLETE_NAME,
                   devName, app_adv_data, &app_adv_data_len);
    GAP_AddAdvData(APP_COMPANY_ID_LEN + 1, GAP_AD_TYPE_MANU_SPECIFIC_DATA,
                   companyID, app_adv_data, &app_adv_data_len);

    /* Set scan response data as company ID */
    app_scan_rsp_data_len = 0;
    GAP_AddAdvData(APP_COMPANY_ID_LEN + 1, GAP_AD_TYPE_MANU_SPECIFIC_DATA,
                   companyID, app_scan_rsp_data, &app_scan_rsp_data_len);
}

