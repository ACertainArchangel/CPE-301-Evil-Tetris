#ifndef _mcu_touch_magic_h_
#define _mcu_touch_magic_h_

#define TCLK_LOW 		*tclkPort &= ~tclkPinSet
#define TCLK_HIGH   	*tclkPort |= tclkPinSet
#define TCS_LOW  		*tcsPort &= ~tcsPinSet
#define TCS_HIGH    	*tcsPort |= tcsPinSet
#define TDIN_LOW  		*tdinPort &= ~tdinPinSet
#define TDIN_HIGH		*tdinPort |= tdinPinSet
#define TDOUT_STATE 	((*tdoutPort) & tdoutPinSet)
#define TIRQ_STATE		((*tirqPort) & tirqPinSet)
#define CT_IIC_SDA_LOW	*cptsdaPort &= ~cptsdaPinSet
#define CT_IIC_SDA_HIGH *cptsdaPort |= cptsdaPinSet
#define CT_IIC_SCL_LOW  *cptsclPort &= ~cptsclPinSet
#define CT_IIC_SCL_HIGH *cptsclPort |= cptsclPinSet
#define CT_IIC_SDA_STATE ((*cptsdaPort) & cptsdaPinSet)

#define RST_CTRL_LOW  *cptrstPort &= ~cptrstPinSet
#define RST_CTRL_HIGH *cptrstPort |= cptrstPinSet
#define INT_CTRL_LOW  *cptintPort &= ~cptintPinSet
#define INT_CTRL_HIGH *cptintPort |= cptintPinSet


#endif