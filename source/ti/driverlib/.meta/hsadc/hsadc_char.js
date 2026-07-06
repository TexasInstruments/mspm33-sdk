var tEOC_12bit_SYSCLK = {
    ADC_CLK_DIV_1_0 : 11,
    ADC_CLK_DIV_2_0 : 21,
    ADC_CLK_DIV_2_5 : 26,
    ADC_CLK_DIV_3_0 : 31,
    ADC_CLK_DIV_3_5 : 36,
    ADC_CLK_DIV_4_0 : 41,
    ADC_CLK_DIV_4_5 : 46,
    ADC_CLK_DIV_5_0 : 51,
    ADC_CLK_DIV_5_5 : 56,
    ADC_CLK_DIV_6_0 : 61,
    ADC_CLK_DIV_6_5 : 66,
    ADC_CLK_DIV_7_0 : 71,
    ADC_CLK_DIV_7_5 : 76,
    ADC_CLK_DIV_8_0 : 81,
    ADC_CLK_DIV_8_5 : 86,    
}

var tEOC_16bit_SYSCLK = {
    ADC_CLK_DIV_1_0 : 31,
    ADC_CLK_DIV_2_0 : 60,
    ADC_CLK_DIV_2_5 : 75,
    ADC_CLK_DIV_3_0 : 90,
    ADC_CLK_DIV_3_5 : 104,
    ADC_CLK_DIV_4_0 : 119,
    ADC_CLK_DIV_4_5 : 134,
    ADC_CLK_DIV_5_0 : 149,
    ADC_CLK_DIV_5_5 : 163,
    ADC_CLK_DIV_6_0 : 178,
    ADC_CLK_DIV_6_5 : 193,
    ADC_CLK_DIV_7_0 : 208,
    ADC_CLK_DIV_7_5 : 222,
    ADC_CLK_DIV_8_0 : 237,
    ADC_CLK_DIV_8_5 : 252,    
}


var Cp_MSPM33C321X = {
    ADCINA0     : { CMP_DIS: 9.1 , CMP_EN:  11.6  },
    ADCINA1     : { CMP_DIS: 7.4 , CMP_EN:  9.9  },
    ADCINA2     : { CMP_DIS: 4.1 , CMP_EN:  6.6  },
    ADCINA3     : { CMP_DIS: 3.3 , CMP_EN:  5.8  },
    ADCINA4     : { CMP_DIS: 3.8 , CMP_EN:  6.3  },
    ADCINA5     : { CMP_DIS: 3.5 , CMP_EN:  6  },
    ADCINA6     : { CMP_DIS: 3.2 , CMP_EN:  5.7  },
    ADCINA7     : { CMP_DIS: 3.8 , CMP_EN:  6.3 },
    ADCINA8     : { CMP_DIS: 4.1 , CMP_EN:  6.6  },
    ADCINA9     : { CMP_DIS: 3.1 , CMP_EN:  5.6  },
    ADCINA10    : { CMP_DIS: 4.7 , CMP_EN: 7.2  },
    ADCINA11    : { CMP_DIS: 4 , CMP_EN: 6.5  },
    ADCINA12    : { CMP_DIS: 3.4 , CMP_EN: 5.9  },
    ADCINA14    : { CMP_DIS: 3.8 , CMP_EN: 6.3  },
    ADCINA15    : { CMP_DIS: 3.8 , CMP_EN: 6.3  },
    ADCINC0     : { CMP_DIS: 4 , CMP_EN: 6.5  },
    ADCINC1     : { CMP_DIS: 3 , CMP_EN: 5.5  },
    ADCINC2     : { CMP_DIS: 3.6 , CMP_EN: 6.1  },
    ADCINC3     : { CMP_DIS: 3.8 , CMP_EN:  6.3 },
    ADCINC4     : { CMP_DIS: 3.8 , CMP_EN: 6.3  },
    ADCINC5     : { CMP_DIS: 3.6 , CMP_EN: 6.1  },
    ADCINC6     : { CMP_DIS: 3.9 , CMP_EN: 6.4  },
    ADCINC7     : { CMP_DIS: 3.3 , CMP_EN: 5.8  },
    ADCINC8     : { CMP_DIS: 3.8 , CMP_EN: 6.3  },
    ADCINC9     : { CMP_DIS: 4.1 , CMP_EN:  6.6  },
    ADCINC10    : { CMP_DIS: 4.7 , CMP_EN: 7.2  },
    ADCINC11    : { CMP_DIS: 4.1 , CMP_EN: 6.6  },
    ADCINC14    : { CMP_DIS: 4.2 , CMP_EN: 6.7  },
    ADCINC15    : { CMP_DIS: 9.1 , CMP_EN: 11.6  },
    ADCINB0     : { CMP_DIS: 4.1 , CMP_EN: 6.6  },
    ADCINB1     : { CMP_DIS: 4.7 , CMP_EN: 7.2  },
    ADCINB2     : { CMP_DIS: 3.9 , CMP_EN: 6.4  },
    ADCINB3     : { CMP_DIS: 75  , CMP_EN: 77.5 },
    ADCINB4     : { CMP_DIS: 3.8 , CMP_EN: 6.3  },
    ADCINB5     : { CMP_DIS: 3.5 , CMP_EN: 6  },
    ADCINB6     : { CMP_DIS: 4.1 , CMP_EN:  6.6  },
    ADCINB7     : { CMP_DIS: 7.4 , CMP_EN:  9.9  },
    ADCINB8     : { CMP_DIS: 3.8 , CMP_EN:  6.3  },
    ADCINB9     : { CMP_DIS: 3.3 , CMP_EN: 5.8  },
    ADCINB11    : { CMP_DIS: 4 , CMP_EN: 6.5  },
    ADCINB12    : { CMP_DIS: 3.6 , CMP_EN: 6.1  },
    ADCINB14    : { CMP_DIS: 3.8 , CMP_EN: 6.3  },
    ADCINB15    : { CMP_DIS: 9.1 , CMP_EN: 11.6  },
    AGPIO_B5    : { CMP_DIS: 3.2   , CMP_EN: 5.7  },
    AGPIO_B11   : { CMP_DIS: 3.1   , CMP_EN: 5.6  },
}



var Cp = {
    MSPM33C321X: Cp_MSPM33C321X,
	
}

var Ron = {
    MSPM33C321X:{ REF_3V3: 860, REF: 500}, 
	
}

var Ch = {
    MSPM33C321X:{ REF_3V3: 7.5,  REF: 12.5},
	
}

exports = {
    Ch: Ch,
    Ron: Ron,
    Cp: Cp,
    tEOC_12bit_SYSCLK,
    tEOC_16bit_SYSCLK
}