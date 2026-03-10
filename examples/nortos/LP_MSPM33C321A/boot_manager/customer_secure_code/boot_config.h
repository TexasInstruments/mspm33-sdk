#ifndef boot_config_h
#define boot_config_h

#include "ti_msp_dl_config.h"

/* clang-format off */

/* BCR Configuration Defines */
/* BCR Config ID */
#define BC_CFG_ID                                    (0x05000000U)

/* Debug Access Levels */
#define BC_CFG_DEBUG_DIS                             (0x5522U)
#define BC_CFG_DEBUG_L0                              (0xAABBU)
#define BC_CFG_DEBUG_L1                              (0xCCDDU)
#define BC_CFG_DEBUG_L2                              (0xDDCCU)

/* SWD Configuration */
#define BC_CFG_SW_DP_EN                              (0xAABBU)
#define BC_CFG_SW_DP_DIS                             (0x5522U)

/* BSL Pin Invoke */
#define BC_CFG_BSL_PIN_INVOKE_DIS                    (0x5522U)
#define BC_CFG_BSL_PIN_INVOKE_EN                     (0xAABBU)

/* Boot Mode Configuration */
#define BC_CFG_FAST_BOOT_DIS                         (0xFFFFU)
#define BC_CFG_FAST_BOOT_EN                          (0xAABBU)
#define BC_CFG_BOOTLOADER_MODE_DIS                   (0x5522U)
#define BC_CFG_BOOTLOADER_MODE_EN                    (0xAABBU)

/* Security Boot Configuration */
#define BC_CFG_SECURE_BOOT_DIS                       (0xFFFFU)
#define BC_CFG_SECURE_BOOT_CRC_EN                    (0xAABBU)
#define BC_CFG_SECURE_BOOT_HASH_EN                   (0xCCDDU)

/* Mass Erase Configuration */
#define BC_CFG_MASS_ERASE_DIS                        (0x5522U)
#define BC_CFG_MASS_ERASE_EN                         (0xAABBU)
#define BC_CFG_MASS_ERASE_EN_PW                      (0xCCDDU)

/* Factory Reset Configuration */
#define BC_CFG_FACTORY_RESET_DIS                     (0x5522U)
#define BC_CFG_FACTORY_RESET_EN                      (0xAABBU)
#define BC_CFG_FACTORY_RESET_EN_PW                   (0xCCDDU)

/* TI Failure Analysis Configuration */
#define BC_CFG_TI_FA_DIS                             (0x5522U)
#define BC_CFG_TI_FA_EN                              (0xAABBU)

/* Debug Hold Configuration */
#define BC_CFG_DEBUG_HOLD_DIS                        (0xAABBU)
#define BC_CFG_DEBUG_HOLD_EN                         (0x5522U)

#define BC_CFG_DEBUG_ACCESS_EN                       (0xAABBU)
#define BC_CFG_DEBUG_ACCESS_DIS                      (0xFFFFU)
#define BC_CFG_MASS_ERASE_EN                         (0xAABBU)
#define BC_CFG_FACTORY_RESET_EN                      (0xAABBU)

/* CSC Configuration */
#define BC_CFG_CSC_NOT_EXIST                         (0xAABBU)
#define BC_CFG_CSC_EXIST                             (0x5522U)

/* Flash Bank Swap Configuration */
#define BC_CFG_FLASH_BS_DIS                          (0xAABBU)
#define BC_CFG_FLASH_BS_EN                           (0x5522U)

/* NonMain Write Protection */
#define BC_CFG_NONMAIN_WRITE_PROTECT_DIS            (0xAABBU)
#define BC_CFG_NONMAIN_WRITE_PROTECT_EN             (0x5522U)

/* Protection Configuration */
#define BC_CFG_STATIC_WRITE_ERASE_PROTECT_DIS       (0x0U)
#define BC_CFG_FULL_SECURE_FLASH                    (0xFFFFFFFFU)
#define BC_CFG_FULL_PRIVILEGE_FLASH                 (0xFFFFFFFFU)
#define BC_CFG_NON_MAIN_STATIC_PROT_MASK           (0x1U)

/* SYSPLL Configuration */
#define BC_CFG_PLL_80MHZ                           (0xAAU)
#define BC_CFG_PLL_CUSTOM                          (0xBBU)
#define BC_CFG_PLL_SOURCE_HFXT_20MHZ               (0xAAU)
#define BC_CFG_PLL_SOURCE_HFXT_40MHZ               (0xBBU)
#define BC_CFG_PLL_SETTLING_TIME_DEFAULT           (0xC8U)
#define BC_CFG_HFXT_STARTUP_MONITOR_TIME_DEFAULT   (0x08U)
#define BC_CFG_PLL_CONFIG_QDIV_DEFAULT             (0xFU)
#define BC_CFG_PLL_CONFIG_RDIV_DEFAULT             (0x0U)

/* NonMain Write Protection Configuration */
#define BC_CFG_USER_CONFIG_NONMAIN_WRITE_PROTECT_DIS   (0xAABBU)
#define BC_CFG_USER_CONFIG_NONMAIN_WRITE_PROTECT_EN    (0x5522U)

/* Default Values */
#define CFG_DEFAULT_VALUE                          (0xFFFFFFFFU)

/* Password Lengths */
#define PASSWORD_WORD_LEN                          (0x8U)
#define HASH256_LEN                                (0x8U)

#define BL_CFG_UART_BAUDRATE_4800    ( ( uint16_t ) 0x1 )
#define BL_CFG_UART_BAUDRATE_9600    ( ( uint16_t ) 0x2 )
#define BL_CFG_UART_BAUDRATE_19200   ( ( uint16_t ) 0x3 )
#define BL_CFG_UART_BAUDRATE_38400   ( ( uint16_t ) 0x4 )
#define BL_CFG_UART_BAUDRATE_57600   ( ( uint16_t ) 0x5 )
#define BL_CFG_UART_BAUDRATE_115200  ( ( uint16_t ) 0x6 )
#define BL_CFG_UART_BAUDRATE_1000000 ( ( uint16_t ) 0x7 )
#define BL_CFG_UART_BAUDRATE_2000000 ( ( uint16_t ) 0x8 )
#define BL_CFG_UART_BAUDRATE_3000000 ( ( uint16_t ) 0x9 )

#define DEF_UART_RXD_PAD    ((uint8_t) 22)
#define DEF_UART_RXD_MUX    ((uint8_t) 7)
#define DEF_UART_TXD_PAD    ((uint8_t) 21)
#define DEF_UART_TXD_MUX    ((uint8_t) 7)
#define DEF_I2C_SDA_PAD     ((uint8_t) 1)
#define DEF_I2C_SDA_MUX     ((uint8_t) 6)
#define DEF_I2C_SCL_PAD     ((uint8_t) 2)
#define DEF_I2C_SCL_MUX     ((uint8_t) 6)
#define DEF_MCAN_RXD_PAD    ((uint8_t) 60)
#define DEF_MCAN_RXD_MUX    ((uint8_t) 4)
#define DEF_MCAN_TXD_PAD    ((uint8_t) 59)
#define DEF_MCAN_TXD_MUX    ((uint8_t) 4)

/* BSL Configuration Defines */
#define BSL_CFG_MEMORY_READOUT_ENABLE     (0xAABBU)
#define BSL_CFG_MEMORY_READOUT_DISABLE    (0x5522U)

/* BSL Security Alert Configuration */
#define BSL_CFG_SECURITY_FACTORY_RESET    (0xAABBU)
#define BSL_CFG_SECURITY_DISABLE_BSL      (0xCCDDU)
#define BSL_CFG_SECURITY_IGNORE           (0xFFFFU)

/* BSL Plugin Types */
#define BSL_CFG_PLUGIN_TYPE_ANY           (0xFFFFU)
#define BSL_CFG_PLUGIN_TYPE_UART          (0x1000U)
#define BSL_CFG_PLUGIN_TYPE_I2C           (0x2000U)

/* Defines for BSL invoke GPIO pin */
#define DEFAULT_BSL_PIN_INVOCATION_DATA0          ((uint8_t) 0xa8)
#define DEFAULT_BSL_PIN_INVOCATION_DATA1          ((uint8_t) 0x12)

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define PLACE_IN_MEMORY(x) __attribute__((section(x))) __attribute__((retain))
#elif defined(__GNUC__) || defined(__clang__)
#define PLACE_IN_MEMORY(x) __attribute__((section(x))) __attribute__((used))
#elif defined(__IAR_SYSTEMS_ICC__)
#define STRINGIFY(a) #a
#define PLACE_IN_MEMORY(x) _Pragma(STRINGIFY(location = x)) __root
#else
#error "Non-Main table currently not supported for this compiler"
#endif

typedef struct {
    /*! Predetermined Bootloader config signature ID */
    uint32_t BSL_CONFIG_ID;

    /*! Interface pin configurations */
    struct {
        uint8_t UART_RXD_pad_num;
        uint8_t UART_RXD_PF_mux_sel;
        uint8_t UART_TXD_pad_num;
        uint8_t UART_TXD_PF_mux_sel;
        uint8_t I2C_SDA_pad_num;
        uint8_t I2C_SDA_PF_mux_sel;
        uint8_t I2C_SCL_pad_num;
        uint8_t I2C_SCL_PF_mux_sel;
        uint8_t MCAN_RX_pad_num;
        uint8_t MCAN_RX_PF_mux_sel;
        uint8_t MCAN_TX_pad_num;
        uint8_t MCAN_TX_PF_mux_sel;
    } BLINTERFACE_PINS;
    struct {
        uint8_t Pin_data_0;
        uint8_t Pin_data_1;
    } BSLPIN_INVOKE;

    uint16_t READOUT;
    uint32_t PASSWORD[8];
    uint32_t *APP_REV_POINTER;
    uint16_t SECURITY_ALERT_LEVEL;
    uint16_t UART_BAUD_RATE;
    uint16_t I2C_SLAVE_ADDR;
    uint8_t RESERVED[14];
    uint32_t CRC;
} __packed BSL_Config;

typedef struct {
    uint32_t WRITE_ERASE_PROTECTION_A;
    uint32_t WRITE_ERASE_PROTECTION_B;
    uint32_t SECURITY_PROTECTION_A;
    uint32_t SECURITY_PROTECTION_B;
    uint32_t PRIVILEGE_PROTECTION_A;
    uint32_t PRIVILEGE_PROTECTION_B;
    uint32_t reserved;
} __packed BANK_PROTECTION_t;

typedef struct {
    uint32_t WRITE_ERASE_PROTECTION;
    uint32_t SECURITY_PROTECTION;
    uint32_t PRIVILEGE_PROTECTION;
    uint32_t reserved;
} __packed DBANK_PROTECTION_t;

typedef struct {
    /*! Predetermined Bootcode config signature ID */
    uint32_t BCR_CONFIG_ID;                 

    /*! Debug Access setting */
    uint16_t DEBUG_ACCESS;
    /*! Serial Wire Debug Port mode */
    uint16_t SWDP_MODE;

    /*! BSL pin invoke enable/disable */
    uint16_t BSL_PIN_INVOKE;
    /*! Debug hold control */
    uint16_t DEBUG_HOLD;

    /*! CSC policy control */
    uint16_t CSC_EXISTS;
    /*! Flash bank swap policy */
    uint16_t FLASH_BANK_SWAP_POLICY;

    /*! Fast boot mode configuration */
    uint16_t FAST_BOOT_MODE;

    /*! Services configuration */
    struct {
        uint16_t BOOTLOADER_MODE;
        uint16_t MASS_ERASE_MODE;
        uint16_t FACTORY_RESET_MODE;
    } __packed SERVICES;

    /*! Password configurations */
    struct {
        uint32_t MASS_ERASE[8];
        uint32_t FACTORY_RESET[8];
        uint32_t DEBUG_LOCK[8];
        uint32_t DEBUG_NS_LOCK[8];
        uint32_t RSVD_PSWD[8];
    } __packed PASSWORDS;

    /*! Secure boot configuration */
    uint16_t SECURE_BOOT_MODE;
    uint16_t BC_RESERVED_0;

    /*! Secure application parameters */
    uint32_t USER_SECURE_APP_START_ADDR;
    uint32_t USER_SECURE_APP_LENGTH;
    uint32_t USER_SECURE_APP_HASH[8];

    /*! GSC configurations */
    struct {
        uint16_t BANK0_NM_USER_CONFIG;
        uint16_t BC_RESERVED_1;
        BANK_PROTECTION_t BANK0;
        BANK_PROTECTION_t BANK1;
        DBANK_PROTECTION_t DBANK;
    } __packed GSC_CONFIGS;

    /*! Clock configurations */
    uint8_t SYSPLL_CONFIG;
    uint8_t SYSPLL_CONFIG_QDIV;
    uint8_t SYSPLL_CONFIG_RDIV;
    uint8_t SYSPLL_SOURCE;
    uint8_t SYSPLL_SETTLING_TIME;
    uint8_t HFXT_STARTUP_MONITOR_TIME;

    /*! Reserved fields */
    uint16_t RESVD[11];

    /*! CRC of BCR config structure */
    uint32_t CRC;
} __packed BCR_Config;

/* Added for secondary_bsl to build */
extern const BSL_Config BSLConfig;
extern const BCR_Config BCRConfig;

/* clang-format on */

#endif /* boot_config_h */