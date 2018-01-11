/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include <string.h>

#include <board.h>
#include <lora.h>
#include <lora-commissioning.h>
#include <LoRaMac.h>
#include <Region.h>
#include <timer.h>
#include <utilities.h>

/* Private typedef -----------------------------------------------------------*/
struct lora_frame {
    uint8_t *buf;
    uint8_t port;
    size_t size;
};

/* Private define ------------------------------------------------------------*/
#define APP_TX_DUTYCYCLE                            5000
#define APP_TX_DUTYCYCLE_RND                        1000

#define LORAWAN_APP_PORT                            1
#define LORAWAN_APP_DATA_MAX_SIZE                   242

#define LORAWAN_ADR_ON                              1
#define LORAWAN_CONFIRMED_MSG_ON                    false
#define LORAWAN_DEFAULT_DATARATE                    DR_0

#if defined(REGION_EU868)
#include "LoRaMacTest.h"
#define LORAWAN_DUTYCYCLE_ON                        false   // TODO: Set to true for release

#define USE_SEMTECH_DEFAULT_CHANNEL_LINEUP          1
#if (USE_SEMTECH_DEFAULT_CHANNEL_LINEUP == 1)
#define LC4                { 867100000, 0, { ((DR_5 << 4) | DR_0) }, 0 }
#define LC5                { 867300000, 0, { ((DR_5 << 4) | DR_0) }, 0 }
#define LC6                { 867500000, 0, { ((DR_5 << 4) | DR_0) }, 0 }
#define LC7                { 867700000, 0, { ((DR_5 << 4) | DR_0) }, 0 }
#define LC8                { 867900000, 0, { ((DR_5 << 4) | DR_0) }, 0 }
#define LC9                { 868800000, 0, { ((DR_7 << 4) | DR_7) }, 2 }
#define LC10               { 868300000, 0, { ((DR_6 << 4) | DR_6) }, 1 }
#endif /* USE_SEMTECH_DEFAULT_CHANNEL_LINEUP */
#endif /* REGION_EU868 */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct lora_frame lora_frame;

static uint8_t DevEui[] = LORAWAN_DEVICE_EUI;
static uint8_t AppEui[] = LORAWAN_APPLICATION_EUI;
static uint8_t AppKey[] = LORAWAN_APPLICATION_KEY;

#if (OVER_THE_AIR_ACTIVATION == 0)
static uint8_t NwkSKey[] = LORAWAN_NWKSKEY;
static uint8_t AppSKey[] = LORAWAN_APPSKEY;
static uint32_t DevAddr = LORAWAN_DEVICE_ADDRESS;
#endif /* OVER_THE_AIR_ACTIVATION */

static TimerEvent_t lora_tx_timer;

static bool lora_cfg_tx_confirmed = false;

static bool lora_tx_pending = true;
static bool lora_network_joined = false;
static bool lora_data_prepared = false;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static bool
lora_tx_prepared(void)
{
    return lora_network_joined && lora_data_prepared && lora_tx_pending;
}

static void
lora_tx_timer_handler(void)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t status;

    TimerStop(&lora_tx_timer);

    mib_req.Type = MIB_NETWORK_JOINED;
    status = LoRaMacMibGetRequestConfirm(&mib_req);

    if (status == LORAMAC_STATUS_OK) {
        if (mib_req.Param.IsNetworkJoined == true) {
            /* TODO: fix */
            // DeviceState = DEVICE_STATE_SEND;
            lora_tx_pending = true;
        } else {
            /* TODO: fix */
            // DeviceState = DEVICE_STATE_JOIN;
        }
    }
}

static void
lora_mcps_confirm(McpsConfirm_t *mcps_confirm)
{
    if (mcps_confirm->Status == LORAMAC_EVENT_INFO_STATUS_OK) {
        switch(mcps_confirm->McpsRequest) {
            case MCPS_UNCONFIRMED:
            {
                // Check Datarate
                // Check TxPower
                break;
            }
            case MCPS_CONFIRMED:
            {
                // Check Datarate
                // Check TxPower
                // Check AckReceived
                // Check NbTrials
                break;
            }
            case MCPS_PROPRIETARY:
            {
                break;
            }
            default:
                break;
        }
    }

    lora_tx_pending = true;
}

static void
lora_mcps_indication(McpsIndication_t *mcps_indication)
{
    if (mcps_indication->Status != LORAMAC_EVENT_INFO_STATUS_OK) {
        return;
    }

    switch(mcps_indication->McpsIndication) {
        case MCPS_UNCONFIRMED:
        {
            break;
        }
        case MCPS_CONFIRMED:
        {
            break;
        }
        case MCPS_PROPRIETARY:
        {
            break;
        }
        case MCPS_MULTICAST:
        {
            break;
        }
        default:
            break;
    }

    // Check Multicast
    // Check Port
    // Check Datarate
    // Check FramePending
    // Check Buffer
    // Check BufferSize
    // Check Rssi
    // Check Snr
    // Check RxSlot


    if (mcps_indication->RxData == true) {
        switch(mcps_indication->Port) {
        case 1: // The application LED can be controlled on port 1 or 2
        case 2:
            break;
        case 3:
            break;
        case 224:
            /* TODO: add compliance test back */
            break;
        default:
            break;
        }
    }
}

static void
lora_mlme_confirm(MlmeConfirm_t *mlme_confirm)
{
    switch(mlme_confirm->MlmeRequest) {
        case MLME_JOIN:
        {
            if (mlme_confirm->Status == LORAMAC_EVENT_INFO_STATUS_OK) {
                // Status is OK, node has joined the network
                lora_network_joined = true;
                /* TODO: fix */
                // DeviceState = DEVICE_STATE_SEND;
            } else {
                // Join was not successful. Try to join again
                /* TODO: fix */
                // DeviceState = DEVICE_STATE_JOIN;
            }
            break;
        }
        case MLME_LINK_CHECK:
        {
            if (mlme_confirm->Status == LORAMAC_EVENT_INFO_STATUS_OK) {
                // Check DemodMargin
                // Check NbGateways
                /* TODO: add compliance test back */
            }
            break;
        }
        default:
            break;
    }

    lora_tx_pending = true;
}

static bool
lora_transmit(void)
{
    McpsReq_t mcps_req;
    LoRaMacTxInfo_t tx_info;

    if (LoRaMacQueryTxPossible(lora_frame.size, &tx_info) != LORAMAC_STATUS_OK) {
        // Send empty frame in order to flush MAC commands
        mcps_req.Type = MCPS_UNCONFIRMED;
        mcps_req.Req.Unconfirmed.fBuffer = NULL;
        mcps_req.Req.Unconfirmed.fBufferSize = 0;
        mcps_req.Req.Unconfirmed.Datarate = LORAWAN_DEFAULT_DATARATE;
    } else {
        if (lora_cfg_tx_confirmed == false) {
            mcps_req.Type = MCPS_UNCONFIRMED;
            mcps_req.Req.Unconfirmed.fPort = lora_frame.port;
            mcps_req.Req.Unconfirmed.fBuffer = lora_frame.buf;
            mcps_req.Req.Unconfirmed.fBufferSize = lora_frame.size;
            mcps_req.Req.Unconfirmed.Datarate = LORAWAN_DEFAULT_DATARATE;
        } else {
            mcps_req.Type = MCPS_CONFIRMED;
            mcps_req.Req.Confirmed.fPort = lora_frame.port;
            mcps_req.Req.Confirmed.fBuffer = lora_frame.buf;
            mcps_req.Req.Confirmed.fBufferSize = lora_frame.size;
            mcps_req.Req.Confirmed.NbTrials = 8;
            mcps_req.Req.Confirmed.Datarate = LORAWAN_DEFAULT_DATARATE;
        }
    }

    if (LoRaMacMcpsRequest(&mcps_req) == LORAMAC_STATUS_OK) {
        return false;
    }

    return true;
}

/* Public functions ----------------------------------------------------------*/
bool
lora_network_is_joined(void)
{
    return lora_network_joined;
}

void
lora_init(void)
{
    LoRaMacPrimitives_t loramac_primitives;
    LoRaMacCallback_t loramac_callbacks;
    MibRequestConfirm_t mib_req;

    lora_frame.buf = NULL;
    lora_frame.size = 0;
    lora_frame.port = LORAWAN_APP_PORT;

    loramac_primitives.MacMcpsConfirm = lora_mcps_confirm;
    loramac_primitives.MacMcpsIndication = lora_mcps_indication;
    loramac_primitives.MacMlmeConfirm = lora_mlme_confirm;

#if defined(REGION_AS923)
                LoRaMacInitialization(&loramac_primitives, &loramac_callbacks,
                LORAMAC_REGION_AS923);
#elif defined(REGION_AU915)
                LoRaMacInitialization(&loramac_primitives, &loramac_callbacks,
                LORAMAC_REGION_AU915);
#elif defined(REGION_CN779)
                LoRaMacInitialization(&loramac_primitives, &loramac_callbacks,
                LORAMAC_REGION_CN779);
#elif defined(REGION_EU868)
                LoRaMacInitialization(&loramac_primitives, &loramac_callbacks,
                LORAMAC_REGION_EU868);
#elif defined(REGION_IN865)
                LoRaMacInitialization(&loramac_primitives, &loramac_callbacks,
                LORAMAC_REGION_IN865);
#elif defined(REGION_KR920)
                LoRaMacInitialization(&loramac_primitives, &loramac_callbacks,
                LORAMAC_REGION_KR920);
#elif defined(REGION_US915)
                LoRaMacInitialization(&loramac_primitives, &loramac_callbacks,
                LORAMAC_REGION_US915);
#elif defined(REGION_US915_HYBRID)
                LoRaMacInitialization(&loramac_primitives, &loramac_callbacks,
                LORAMAC_REGION_US915_HYBRID);
#else
    #error "Please define a region in the compiler options."
#endif

    TimerInit(&lora_tx_timer, lora_tx_timer_handler);

    mib_req.Type = MIB_ADR;
    mib_req.Param.AdrEnable = LORAWAN_ADR_ON;
    LoRaMacMibSetRequestConfirm(&mib_req);

    mib_req.Type = MIB_PUBLIC_NETWORK;
    mib_req.Param.EnablePublicNetwork = LORAWAN_PUBLIC_NETWORK;
    LoRaMacMibSetRequestConfirm(&mib_req);

#if defined(REGION_EU868)
    LoRaMacTestSetDutyCycleOn(LORAWAN_DUTYCYCLE_ON);
#if (USE_SEMTECH_DEFAULT_CHANNEL_LINEUP == 1)
    LoRaMacChannelAdd(3, (ChannelParams_t)LC4);
    LoRaMacChannelAdd(4, (ChannelParams_t)LC5);
    LoRaMacChannelAdd(5, (ChannelParams_t)LC6);
    LoRaMacChannelAdd(6, (ChannelParams_t)LC7);
    LoRaMacChannelAdd(7, (ChannelParams_t)LC8);
    LoRaMacChannelAdd(8, (ChannelParams_t)LC9);
    LoRaMacChannelAdd(9, (ChannelParams_t)LC10);

    mib_req.Type = MIB_RX2_DEFAULT_CHANNEL;
    mib_req.Param.Rx2DefaultChannel = (Rx2ChannelParams_t){ 869525000, DR_3 };
    LoRaMacMibSetRequestConfirm(&mib_req);

    mib_req.Type = MIB_RX2_CHANNEL;
    mib_req.Param.Rx2Channel = (Rx2ChannelParams_t){ 869525000, DR_3 };
    LoRaMacMibSetRequestConfirm(&mib_req);
#endif /* USE_SEMTECH_DEFAULT_CHANNEL_LINEUP */
#endif /* REGION_EU868 */
}

void
lora_join(void)
{
    // Initialize LoRaMac device unique ID
    board_get_unique_id(DevEui);

#if (OVER_THE_AIR_ACTIVATION != 0)
    MlmeReq_t mlme_req;  

    mlme_req.Type = MLME_JOIN;

    mlme_req.Req.Join.DevEui = DevEui;
    mlme_req.Req.Join.AppEui = AppEui;
    mlme_req.Req.Join.AppKey = AppKey;
    mlme_req.Req.Join.NbTrials = 3;

    if (lora_tx_permitted == true) {
        LoRaMacMlmeRequest(&mlme_req);
    }
#else
    MibRequestConfirm_t mib_req;

    /* Choose a random device address if not already defined in
     * lora-commissioning.h */
    if (DevAddr == 0) {
        // Random seed initialization
        srand1(board_get_random_seed());

        // Choose a random device address
        DevAddr = randr(0, 0x01FFFFFF);
    }

    mib_req.Type = MIB_NET_ID;
    mib_req.Param.NetID = LORAWAN_NETWORK_ID;
    LoRaMacMibSetRequestConfirm(&mib_req);

    mib_req.Type = MIB_DEV_ADDR;
    mib_req.Param.DevAddr = DevAddr;
    LoRaMacMibSetRequestConfirm(&mib_req);

    mib_req.Type = MIB_NWK_SKEY;
    mib_req.Param.NwkSKey = NwkSKey;
    LoRaMacMibSetRequestConfirm(&mib_req);

    mib_req.Type = MIB_APP_SKEY;
    mib_req.Param.AppSKey = AppSKey;
    LoRaMacMibSetRequestConfirm(&mib_req);

    mib_req.Type = MIB_NETWORK_JOINED;
    mib_req.Param.IsNetworkJoined = true;
    LoRaMacMibSetRequestConfirm(&mib_req);
#endif

    lora_network_joined = true;
}

void
lora_prepare_frame(uint8_t *buf, size_t size)
{
    if ((buf == NULL) || (size <= 0)) {
        return;
    }
    
    lora_frame.buf = buf;
    lora_frame.size = size;

    lora_data_prepared = true;
}

void
lora_send_frame(void)
{
    /* TODO: add better dutycycle checks */
    lora_tx_pending = lora_transmit();

    // if (lora_tx_prepared()) {
    //     lora_tx_pending = lora_transmit();

    //     lora_data_prepared = false;
    // }
}
