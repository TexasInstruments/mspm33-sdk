"use strict";

let Common = system.getScript("/ti/driverlib/Common.js");

const peripheralInstances = [
    // ANALOG group
    { name: "ADC0", displayName: "ADC 0", type: "ADC", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "ADC1", displayName: "ADC 1", type: "ADC", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "ACOMP0", displayName: "ACOMP 0", type: "ACOMP", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "ACOMP1", displayName: "ACOMP 1", type: "ACOMP", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "VREF", displayName: "VREF", type: "VREF", defaultSecure: false, hasPrivilegeAccess: false },

    // COMMUNICATION group
    { name: "UC1_0", displayName: "UC1_0", type: "Unicomm", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "UC1_1", displayName: "UC1_1", type: "Unicomm", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "UC2", displayName: "UC2", type: "Unicomm", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "UC15_0", displayName: "UC15_0", type: "Unicomm", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "UC15_1", displayName: "UC15_1", type: "Unicomm", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "UC12", displayName: "UC12", type: "Unicomm", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "UC13_0", displayName: "UC13_0", type: "Unicomm", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "UC13_1", displayName: "UC13_1", type: "Unicomm", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "UC13_2", displayName: "UC13_2", type: "Unicomm", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "UC13_3", displayName: "UC13_3", type: "Unicomm", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "UC14", displayName: "UC14", type: "Unicomm", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "I2S0", displayName: "I2S 0", type: "I2S", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "I2S1", displayName: "I2S 1", type: "I2S", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "MCAN0", displayName: "MCAN 0", type: "MCAN", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "MCAN1", displayName: "MCAN 1", type: "MCAN", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "QSPI", displayName: "QSPI", type: "QSPI", defaultSecure: false, hasPrivilegeAccess: false },

    // SYSTEM group
    { name: "GPIOA", displayName: "GPIO A", type: "GPIO", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "GPIOB", displayName: "GPIO B", type: "GPIO", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "GPIOC", displayName: "GPIO C", type: "GPIO", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "DMA0", displayName: "DMA 0", type: "DMA", defaultSecure: true, defaultPrivileged: true, hasPrivilegeAccess: true },
    { name: "DMA1", displayName: "DMA 1", type: "DMA", defaultSecure: false, defaultPrivileged: false, hasPrivilegeAccess: true },
    { name: "SYSCTL", displayName: "System Control", type: "SYSCTL", defaultSecure: true, defaultPrivileged: true, hasPrivilegeAccess: true },
    { name: "IOMUX", displayName: "IOMUX", type: "IOMUX", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "WUC", displayName: "WUC", type: "WUC", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "FRIREGS", displayName: "FRI Registers", type: "FRIREGS", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "MEMCFG", displayName: "Memory Config", type: "MEMCFG", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "LFSS", displayName: "LFSS", type: "LFSS", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "EAM", displayName: "EAM", type: "EAM", defaultSecure: false, hasPrivilegeAccess: false },

    // TIMERS group
    { name: "TIMA0_0", displayName: "Timer A0_0", type: "TIMER", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "TIMA0_1", displayName: "Timer A0_1", type: "TIMER", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "TIMG4_0", displayName: "Timer G4_0", type: "TIMER", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "TIMG4_1", displayName: "Timer G4_1", type: "TIMER", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "TIMG4_2", displayName: "Timer G4_2", type: "TIMER", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "TIMG4_3", displayName: "Timer G4_3", type: "TIMER", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "TIMG12_0", displayName: "Timer G12_0", type: "TIMER", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "TIMG8_0", displayName: "Timer G8_0", type: "TIMER", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "TIMG8_1", displayName: "Timer G8_1", type: "TIMER", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "WWDT", displayName: "Watchdog Timer", type: "WDT", defaultSecure: false, hasPrivilegeAccess: false },

    // SECURITY group
    { name: "CRC", displayName: "CRC", type: "CRC", defaultSecure: true, hasPrivilegeAccess: false },
    { name: "AES", displayName: "AES", type: "AES", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "SHA", displayName: "SHA", type: "SHA", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "PKA", displayName: "PKA", type: "PKA", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "TRNG", displayName: "TRNG", type: "TRNG", defaultSecure: true, hasPrivilegeAccess: false },
    { name: "DEBUGSS", displayName: "Debug Subsystem", type: "DEBUGSS", defaultSecure: true, hasPrivilegeAccess: false },
    { name: "KEYSTORECTL", displayName: "Keystore Control", type: "KEYSTORECTL", defaultSecure: false, hasPrivilegeAccess: false },
    { name: "GSC", displayName: "Global Security Controller", type: "GSC", defaultSecure: true, defaultPrivileged: true, hasPrivilegeAccess: true }
];

const instanceGroups = {
    ANALOG: ["ADC", "ACOMP", "VREF"],
    COMMUNICATION: ["Unicomm", "I2S", "MCAN", "QSPI"],
    SYSTEM: ["GPIO", "DMA", "SYSCTL", "IOMUX", "WUC", "FRIREGS", "MEMCFG", "LFSS", "EAM"],
    TIMERS: ["TIMER", "WDT"],
    SECURITY: ["CRC", "AES", "SHA", "PKA", "TRNG", "DEBUGSS", "KEYSTORECTL", "GSC"],
};

function generatePeripheralAccessConfig() {
    let configs = [];

    Object.keys(instanceGroups).forEach(groupName => {
        let groupInstances = [];
        instanceGroups[groupName].forEach(type => {
            let typeInstances = peripheralInstances.filter(inst => inst.type === type);
            groupInstances = groupInstances.concat(typeInstances);
        });

        if (groupInstances.length > 0) {
            let groupConfig = {
                name: `group_${groupName.toLowerCase()}`,
                displayName: groupName,
                collapsed: true,
                config: []
            };

            // Create instance configurations
            groupInstances.forEach(instance => {
                let instanceConfig = {
                    name: `config_${instance.name.toLowerCase()}`,
                    displayName: instance.displayName,
                    collapsed: false,
                    config: [
                        {
                            name: `secure_${instance.name.toLowerCase()}`,
                            displayName: "Secure Access",
                            default: instance.defaultSecure,
                            description: `Configure secure access for ${instance.displayName}`
                        }
                    ]
                };

                // Only add privileged access configuration if the peripheral supports it
                if (instance.hasPrivilegeAccess) {
                    instanceConfig.config.push({
                        name: `privileged_${instance.name.toLowerCase()}`,
                        displayName: "Privileged Access",
                        default: instance.defaultPrivileged || false,
                        description: `Configure privileged access for ${instance.displayName}`
                    });
                }

                groupConfig.config.push(instanceConfig);
            });

            configs.push(groupConfig);
        }
    });

    return configs;
}

function generateInstanceConfigs(peripheral, isSecure) {
    return peripheral.instances.map(instance => ({
        name: `${isSecure ? 'secure' : 'priv'}_${peripheral.name.toLowerCase()}_${instance.toLowerCase()}`,
        displayName: instance,
        default: isSecure ? peripheral.defaultSecure : false, // Use defaultSecure for secure access
        description: `Configure ${isSecure ? 'secure' : 'privileged'} access for ${peripheral.displayName} ${instance}`,
        onChange: (inst, ui) => {
            onPeripheralAccessChange(inst, ui, peripheral.name, instance, isSecure);
        }
    }));
}

function generateSRAMConfig() {
    return {
        name: "sram_config",
        displayName: "SRAM Configuration",
        description: "Configure SRAM Security and Privilege Attributes",
        longDescription: `Configure security (secure/non-secure) and privilege (privileged/non-privileged) attributes for SRAM memory regions.
            The SRAM is divided into 16 pages of 16KB each, with each page further divided into:
            - Two 512B chunks  (0x000-0x3FF)
            - One 1KB chunk   (0x400-0x7FF)
            - One 2KB chunk   (0x800-0xFFF)
            - Three 4KB chunks (0x1000-0x3FFF)`,
        config: [
            {
                name: "sram_quick_config",
                displayName: "SRAM Quick Configuration",
                description: "Configure SRAM security in larger chunks",
                collapsed: false,
                config: [
                    {
                        name: "secure_sram_size",
                        displayName: "Secure SRAM Size (KB)",
                        description: "Amount of SRAM to mark as secure from start of SRAM",
                        default: 0,
                        options: Array.from({ length: 17 }, (_, i) => ({
                            name: i * 16,
                            displayName: `${i * 16} KB`
                        })),
                        onChange: function (inst) {
                            updateDetailedSRAMConfig(inst, 'secure');
                        }
                    },
                    {
                        name: "privileged_sram_size",
                        displayName: "Privileged SRAM Size (KB)",
                        description: "Amount of SRAM to mark as privileged from start of SRAM",
                        default: 0,
                        options: Array.from({ length: 17 }, (_, i) => ({
                            name: i * 16,
                            displayName: `${i * 16} KB`
                        })),
                        onChange: function (inst) {
                            updateDetailedSRAMConfig(inst, 'privileged');
                        }
                    }
                ]
            },
            {
                name: "sram_detailed_config",
                displayName: "SRAM Detailed View",
                description: "View of detailed SRAM security configuration",
                collapsed: true,
                config: generateDetailedSRAMPages()
            }
        ]
    };
}

function generateDetailedSRAMPages() {
    const chunks = [
        { size: '512b0', display: 'First 512B' },
        { size: '512b1', display: 'Second 512B' },
        { size: '1kb', display: '1KB' },
        { size: '2kb', display: '2KB' },
        { size: '4kb0', display: 'First 4KB' },
        { size: '4kb1', display: 'Second 4KB' },
        { size: '4kb2', display: 'Third 4KB' }
    ];

    let pages = [];

    for (let pageIndex = 0; pageIndex < 16; pageIndex++) {
        const baseAddress = 0x20000000 + (pageIndex * 0x4000);
        const endAddress = baseAddress + 0x3FFF;

        let pageConfig = {
            name: `sram_page_${pageIndex}`,
            displayName: `SRAM Page ${pageIndex}`,
            description: `Configure SRAM Page ${pageIndex} (0x${baseAddress.toString(16).toUpperCase()}-0x${endAddress.toString(16).toUpperCase()})`,
            collapsed: true,
            config: []
        };

        // Generate configurations for each chunk
        chunks.forEach(chunk => {
            // Add secure configuration
            pageConfig.config.push({
                name: `p${pageIndex}_${chunk.size}_secure`,
                displayName: `${chunk.display} Chunk - Secure`,
                description: `Enable secure-only access for ${chunk.display.toLowerCase()} chunk`,
                default: false
            });

            // Add privileged configuration
            pageConfig.config.push({
                name: `p${pageIndex}_${chunk.size}_priv`,
                displayName: `${chunk.display} Chunk - Privileged`,
                description: `Enable privileged-only access for ${chunk.display.toLowerCase()} chunk`,
                default: false
            });
        });

        pages.push(pageConfig);
    }

    return pages;
}

// Helper function to format address in hex with proper padding
function formatHexAddress(address) {
    return `0x${address.toString(16).toUpperCase().padStart(8, '0')}`;
}

// Function to calculate chunk offset within a page
function getChunkOffset(pageBase, chunk) {
    const offsets = {
        '512b0': 0x000,
        '512b1': 0x200,
        '1kb': 0x400,
        '2kb': 0x800,
        '4kb0': 0x1000,
        '4kb1': 0x2000,
        '4kb2': 0x3000
    };
    return pageBase + (offsets[chunk] || 0);
}

// Function to get chunk size in bytes
function getChunkSize(chunk) {
    const sizes = {
        '512b0': 512,
        '512b1': 512,
        '1kb': 1024,
        '2kb': 2048,
        '4kb0': 4096,
        '4kb1': 4096,
        '4kb2': 4096
    };
    return sizes[chunk] || 0;
}

function updateDetailedSRAMConfig(inst, type = 'secure') {
    // Get the appropriate size based on type
    let sizeKB;
    if (type === 'secure') {
        sizeKB = parseInt(inst.secure_sram_size);
    } else if (type === 'privileged') {
        sizeKB = parseInt(inst.privileged_sram_size);
    } else {
        return;  // Invalid type
    }

    if (isNaN(sizeKB)) {
        return;
    }

    const pagesNeeded = Math.floor(sizeKB / 16);
    const remainingKB = sizeKB % 16;
    const suffix = type === 'secure' ? 'secure' : 'priv';

    // Reset all detailed configurations first for this type
    for (let pageIndex = 0; pageIndex < 16; pageIndex++) {
        const chunks = ['512b0', '512b1', '1kb', '2kb', '4kb0', '4kb1', '4kb2'];
        chunks.forEach(chunk => {
            const propName = `p${pageIndex}_${chunk}_${suffix}`;
            if (propName in inst) {
                inst[propName] = false;
            }
        });
    }

    // Set full pages
    for (let pageIndex = 0; pageIndex < pagesNeeded; pageIndex++) {
        const chunks = ['512b0', '512b1', '1kb', '2kb', '4kb0', '4kb1', '4kb2'];
        chunks.forEach(chunk => {
            const propName = `p${pageIndex}_${chunk}_${suffix}`;
            if (propName in inst) {
                inst[propName] = true;
            }
        });
    }

    // Handle partial page if needed
    if (remainingKB > 0 && pagesNeeded < 16) {
        let remainingBytes = remainingKB * 1024;
        const page = pagesNeeded;

        const chunkSizes = [
            { name: '512b0', size: 512 },
            { name: '512b1', size: 512 },
            { name: '1kb', size: 1024 },
            { name: '2kb', size: 2048 },
            { name: '4kb0', size: 4096 },
            { name: '4kb1', size: 4096 },
            { name: '4kb2', size: 4096 }
        ];

        for (const chunk of chunkSizes) {
            if (remainingBytes >= chunk.size) {
                const propName = `p${page}_${chunk.name}_${suffix}`;
                if (propName in inst) {
                    inst[propName] = true;
                }
                remainingBytes -= chunk.size;
            }
        }
    }
}


function generateFlashConfig() {
    return {
        name: "flash_config",
        displayName: "Flash Configuration",
        description: "Configure Flash Protection Attributes",
        longDescription: `Configure write-erase protection (WEP) and hide protection (HDP) for different flash regions:
            - Main Flash (Bank A/B)
            - Non-Main Flash
            - Data Flash`,
        config: [
            {
                name: "flash_detailed_view",
                displayName: "Flash Detailed View",
                description: "Detailed view of flash protection configuration",
                collapsed: true,
                config: generateDetailedFlashPages()
            }
        ]
    };
}



function generateMainFlashSectorConfigs() {
    const MAIN_FLASH_SECTORS_PER_HALF = 32;
    const TOTAL_SECTORS = 64;
    const SECTOR_SIZE = 0x800; // 2KB
    let configs = [];

    // Bank A Configuration (0x00000000 - 0x0007FFFF)
    let bankAConfig = {
        name: "bank_a_sectors",
        displayName: "Bank A Sectors (0x00000000 - 0x0007FFFF)",
        description: "Configure protection for Bank A sectors",
        collapsed: true,
        config: []
    };

    // First half (sectors 0-31)
    for (let i = 0; i < MAIN_FLASH_SECTORS_PER_HALF; i++) {
        const baseAddr = 0x0 + (i * SECTOR_SIZE);
        bankAConfig.config.push({
            name: `bank_a_sector_${i}`,
            displayName: `Sector ${i} (0x${baseAddr.toString(16).toUpperCase()} - 0x${(baseAddr + 0x7FF).toString(16).toUpperCase()})`,
            collapsed: false,
            config: [
                {
                    name: `bank_a_s${i}_wep`,
                    displayName: "Write-Erase Protection",
                    description: `Write-Erase Protection for Bank A Sector ${i}`,
                    default: false
                },
                {
                    name: `bank_a_s${i}_sec`,
                    displayName: "Secure",
                    description: `Security attribute for Bank A Sector ${i}`,
                    default: false
                }
            ]
        });
    }

    // Second half (sectors 32-63)
    for (let i = MAIN_FLASH_SECTORS_PER_HALF; i < TOTAL_SECTORS; i++) {
        const baseAddr = 0x0 + (i * SECTOR_SIZE);
        bankAConfig.config.push({
            name: `bank_a_sector_${i}`,
            displayName: `Sector ${i} (0x${baseAddr.toString(16).toUpperCase()} - 0x${(baseAddr + 0x7FF).toString(16).toUpperCase()})`,
            collapsed: false,
            config: [
                {
                    name: `bank_a_s${i}_wep`,
                    displayName: "Write-Erase Protection",
                    description: `Write-Erase Protection for Bank A Sector ${i}`,
                    default: false
                },
                {
                    name: `bank_a_s${i}_sec`,
                    displayName: "Secure",
                    description: `Security attribute for Bank A Sector ${i}`,
                    default: false
                }
            ]
        });
    }
    configs.push(bankAConfig);

    // Bank B Configuration (0x00080000 - 0x000FFFFF)
    let bankBConfig = {
        name: "bank_b_sectors",
        displayName: "Bank B Sectors (0x00080000 - 0x000FFFFF)",
        description: "Configure protection for Bank B sectors",
        collapsed: true,
        config: []
    };

    // First half (sectors 0-31)
    for (let i = 0; i < MAIN_FLASH_SECTORS_PER_HALF; i++) {
        const baseAddr = 0x80000 + (i * SECTOR_SIZE);
        bankBConfig.config.push({
            name: `bank_b_sector_${i}`,
            displayName: `Sector ${i} (0x${baseAddr.toString(16).toUpperCase()} - 0x${(baseAddr + 0x7FF).toString(16).toUpperCase()})`,
            collapsed: true,
            config: [
                {
                    name: `bank_b_s${i}_wep`,
                    displayName: "Write-Erase Protection",
                    description: `Write-Erase Protection for Bank B Sector ${i}`,
                    default: false
                },
                {
                    name: `bank_b_s${i}_sec`,
                    displayName: "Secure",
                    description: `Security attribute for Bank B Sector ${i}`,
                    default: false
                }
            ]
        });
    }

    // Second half (sectors 32-63)
    for (let i = MAIN_FLASH_SECTORS_PER_HALF; i < TOTAL_SECTORS; i++) {
        const baseAddr = 0x80000 + (i * SECTOR_SIZE);
        bankBConfig.config.push({
            name: `bank_b_sector_${i}`,
            displayName: `Sector ${i} (0x${baseAddr.toString(16).toUpperCase()} - 0x${(baseAddr + 0x7FF).toString(16).toUpperCase()})`,
            collapsed: true,
            config: [
                {
                    name: `bank_b_s${i}_wep`,
                    displayName: "Write-Erase Protection",
                    description: `Write-Erase Protection for Bank B Sector ${i}`,
                    default: false
                },
                {
                    name: `bank_b_s${i}_sec`,
                    displayName: "Secure",
                    description: `Security attribute for Bank B Sector ${i}`,
                    default: false
                }
            ]
        });
    }
    configs.push(bankBConfig);

    return configs;
}

const FLASH_CONFIG = {
    MAIN_FLASH: {
        BANK_A_BASE: 0x00000000,
        BANK_B_BASE: 0x00080000,
        SECTOR_SIZE: 0x800,  // 2KB
        SECTORS_PER_BANK: 64
    },
    NONMAIN_FLASH: {
        BASE_ADDRESS: 0x00020000,
        SECTOR_SIZE: 0x800,  // 2KB
        TOTAL_SECTORS: 8
    },
    DATA_FLASH: {
        BASE_ADDRESS: 0x00040000,
        SECTOR_SIZE: 0x800,  // 2KB
        TOTAL_SECTORS: 16
    }
};

function generateDetailedFlashPages() {
    let configs = [];

    // Main Flash Pages
    configs.push({
        name: "main_flash_details",
        displayName: "Main Flash Details",
        collapsed: true,
        config: generateMainFlashSectorConfigs()
    });

    // Non-Main Flash (0x80100000 - 0x80103FFF)
    configs.push({
        name: "nonmain_flash_details",
        displayName: "Non-Main Flash Details",
        collapsed: true,
        config: generateNonMainFlashSectorConfigs()
    });

    // Data Flash (0x80000000 - 0x80007FFF)
    configs.push({
        name: "data_flash_details",
        displayName: "Data Flash Details",
        collapsed: true,
        config: generateDataFlashSectorConfigs()
    });

    return configs;
}

function updateMainFlashWEPConfig(inst) {

    updateDetailedFlashView(inst);
}

function generateNonMainFlashSectorConfigs() {
    const NONMAIN_FLASH_SECTORS = 4; // 4 sectors per bank
    const SECTOR_SIZE = 0x800;  // 2KB per sector
    let configs = [];

    // Bank A Configuration (0x80100000 - 0x80101FFF)
    let bankAConfig = {
        name: "nonmain_bank_a_sectors",
        displayName: "Non-Main Flash Bank A (0x80100000 - 0x80101FFF)",
        description: "Configure protection for Non-Main Flash Bank A",
        collapsed: false,
        config: []
    };

    for (let i = 0; i < NONMAIN_FLASH_SECTORS; i++) {
        const baseAddr = 0x80100000 + (i * SECTOR_SIZE);
        bankAConfig.config.push({
            name: `nonmain_bank_a_sector_${i}`,
            displayName: `Sector ${i} (0x${baseAddr.toString(16).toUpperCase()} - 0x${(baseAddr + 0x7FF).toString(16).toUpperCase()})`,
            collapsed: true,
            config: [
                {
                    name: `nonmain_bank_a_s${i}_wep`,
                    displayName: "Write-Erase Protection",
                    description: `Write-Erase Protection for Non-Main Bank A Sector ${i}`,
                    default: false
                },
                {
                    name: `nonmain_bank_a_s${i}_sec`,
                    displayName: "Secure",
                    description: `Security attribute for Non-Main Bank A Sector ${i}`,
                    default: false
                }
            ]
        });
    }
    configs.push(bankAConfig);

    // Bank B Configuration (0x80102000 - 0x80103FFF)
    let bankBConfig = {
        name: "nonmain_bank_b_sectors",
        displayName: "Non-Main Flash Bank B (0x80102000 - 0x80103FFF)",
        description: "Configure protection for Non-Main Flash Bank B",
        collapsed: true,
        config: []
    };

    for (let i = 0; i < NONMAIN_FLASH_SECTORS; i++) {
        const baseAddr = 0x80102000 + (i * SECTOR_SIZE);
        bankBConfig.config.push({
            name: `nonmain_bank_b_sector_${i}`,
            displayName: `Sector ${i} (0x${baseAddr.toString(16).toUpperCase()} - 0x${(baseAddr + 0x7FF).toString(16).toUpperCase()})`,
            collapsed: true,
            config: [
                {
                    name: `nonmain_bank_b_s${i}_wep`,
                    displayName: "Write-Erase Protection",
                    description: `Write-Erase Protection for Non-Main Bank B Sector ${i}`,
                    default: false
                },
                {
                    name: `nonmain_bank_b_s${i}_sec`,
                    displayName: "Secure",
                    description: `Security attribute for Non-Main Bank B Sector ${i}`,
                    default: false
                }
            ]
        });
    }
    configs.push(bankBConfig);

    return configs;
}

function generateDataFlashSectorConfigs() {
    const DATA_FLASH_SECTORS = 16; // 16 sectors total
    const SECTOR_SIZE = 0x800;  // 2KB per sector
    let configs = [];

    let dataFlashConfig = {
        name: "data_flash_sectors",
        displayName: "Data Flash (0x80000000 - 0x80007FFF)",
        description: "Configure protection for Data Flash",
        collapsed: false,
        config: []
    };

    for (let i = 0; i < DATA_FLASH_SECTORS; i++) {
        const baseAddr = 0x80000000 + (i * SECTOR_SIZE);
        dataFlashConfig.config.push({
            name: `data_flash_sector_${i}`,
            displayName: `Sector ${i} (0x${baseAddr.toString(16).toUpperCase()} - 0x${(baseAddr + 0x7FF).toString(16).toUpperCase()})`,
            collapsed: false,
            config: [
                {
                    name: `data_flash_s${i}_wep`,
                    displayName: "Write-Erase Protection",
                    description: `Write-Erase Protection for Data Flash Sector ${i}`,
                    default: false
                },
                {
                    name: `data_flash_s${i}_sec`,
                    displayName: "Secure",
                    description: `Security attribute for Data Flash Sector ${i}`,
                    default: false
                }
            ]
        });
    }
    configs.push(dataFlashConfig);

    return configs;
}

function generateHideProtectionConfig() {
    return {
        name: "hide_protection",
        displayName: "Hide Protection Configuration",
        description: "Configure Hide Protection (HDP) for flash regions",
        longDescription: "Configure hide protection for flash memory regions.",
        collapsed: true,
        config: [
            // Bank 0 settings
            {
                name: "bank0StartSector",
                displayName: "Bank 0 Start Sector",
                description: "Starting sector number for Bank 0",
                default: 0,
                min: 0,
                max: 255
            },
            {
                name: "bank0EndSector",
                displayName: "Bank 0 End Sector",
                description: "Ending sector number for Bank 0",
                default: 0,
                min: 0,
                max: 255
            },
            {
                name: "bank0HdpAccess",
                displayName: "Bank 0 HDP Access",
                description: "Enable/Disable access to Bank 0 protected region",
                default: false
            },

            // Bank 1 settings
            {
                name: "bank1StartSector",
                displayName: "Bank 1 Start Sector",
                description: "Starting sector number for Bank 1",
                default: 0,
                min: 0,
                max: 255
            },
            {
                name: "bank1EndSector",
                displayName: "Bank 1 End Sector",
                description: "Ending sector number for Bank 1",
                default: 0,
                min: 0,
                max: 255
            },
            {
                name: "bank1HdpAccess",
                displayName: "Bank 1 HDP Access",
                description: "Enable/Disable access to Bank 1 protected region",
                default: false
            },

            // Data Bank settings
            {
                name: "dataBankStartSector",
                displayName: "Data Bank Start Sector",
                description: "Starting sector number for Data Bank",
                default: 0,
                min: 0,
                max: 255
            },
            {
                name: "dataBankEndSector",
                displayName: "Data Bank End Sector",
                description: "Ending sector number for Data Bank",
                default: 0,
                min: 0,
                max: 255
            },
            {
                name: "dataBankHdpAccess",
                displayName: "Data Bank HDP Access",
                description: "Enable/Disable access to Data Bank protected region",
                default: false
            },

            // Global HDP mode settings
            {
                name: "mainHdpEnable",
                displayName: "Enable Main Flash HDP",
                description: "Enable/Disable Main Flash Hide Protection mode",
                default: false
            },
            {
                name: "dataBankHdpEnable",
                displayName: "Enable Data Bank HDP",
                description: "Enable/Disable Data Bank Hide Protection mode",
                default: false
            }
        ]
    };
}

// Update validation function
function validateHDPConfig(inst, validation) {
    if (inst.hide_protection) {
        // Validate Bank 0
        if (inst.hide_protection.bank0StartSector > inst.hide_protection.bank0EndSector) {
            validation.logError("Bank 0: Start sector must be less than or equal to end sector",
                "hide_protection.bank0StartSector");
        }

        // Validate Bank 1
        if (inst.hide_protection.bank1StartSector > inst.hide_protection.bank1EndSector) {
            validation.logError("Bank 1: Start sector must be less than or equal to end sector",
                "hide_protection.bank1StartSector");
        }

        // Validate Data Bank
        if (inst.hide_protection.dataBankStartSector > inst.hide_protection.dataBankEndSector) {
            validation.logError("Data Bank: Start sector must be less than or equal to end sector",
                "hide_protection.dataBankStartSector");
        }
    }
}


let gscConfig = [
    {
        name: "GROUP_PERIPHERAL_ACCESS",
        displayName: "Peripheral Access Control",
        description: "Configure secure and privileged access for peripheral instances",
        collapsed: true,
        config: generatePeripheralAccessConfig()
    },
    generateSRAMConfig(),
    generateFlashConfig(),
];

function onPeripheralAccessChange(inst, ui, peripheral, instance, isSecure) {
    // Add any specific logic needed when access settings change
    console.log(`${isSecure ? 'Secure' : 'Privileged'} access for ${peripheral}.${instance} changed`);
}

/*!
 *  ======== validate ========
 *  Validate this module's configuration
 *
 *  @param inst       - GSC instance to be validated
 *  @param validation - Issue reporting object
 */
function validate(inst, validation) {
    validatePeripheralAccess(inst, validation);
    validateHDPConfig(inst, validation);
}


function validatePeripheralAccess(inst, validation) {
}

let devSpecific = {
    config: gscConfig,
    validate: validate
};

let base = {
    displayName: "GSC",
    description: "Global Security Controller",
    longDescription: `
The Global Security Controller (GSC) module provides:
* Peripheral access control
* Secure/non-secure access configuration
* Privileged/non-privileged access configuration
`,
    defaultInstanceName: "CONFIG_GSC_",
    config: gscConfig,
    moduleStatic: {
        config: gscConfig
    },
    templates: {
        boardc: "/ti/driverlib/gsc/GSC.Board.c.xdt",
        boardh: "/ti/driverlib/gsc/GSC.Board.h.xdt",
        Call: true,
        Reset: true,
        Power: false,
        Function: true
    },
    validate: validate
};

function extend(base) {
    return Object.assign({}, base, {
        config: gscConfig,
        moduleStatic: {
            config: gscConfig
        }
    });
}

exports = {
    extend: extend,
    moduleStatic: base
};
