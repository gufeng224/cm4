/*=============================================================*/
// Macro definition
/*=============================================================*/


/****************************************************************************/
/*        AUDIO DIGITAL registers                                           */
/****************************************************************************/
enum{
    HAYDN_DMA_W1_CFG_0       = 0x60,  //BANK1_DMA
    HAYDN_DMA_W1_CFG_1       = 0x62,
    HAYDN_DMA_W1_CFG_2       = 0x64,
    HAYDN_DMA_W1_CFG_3       = 0x66,
    HAYDN_DMA_W1_CFG_4       = 0x68,
    HAYDN_DMA_W1_CFG_5       = 0x6A,
    HAYDN_DMA_W1_CFG_6       = 0x6C,
    HAYDN_DMA_W1_CFG_7       = 0x6E,
    HAYDN_DMA_W1_CFG_8       = 0x70,
    HAYDN_DMA_W1_CFG_9       = 0x72,
    HAYDN_DMA_W1_STS_0       = 0x74,
    HAYDN_DMA_W1_STS_1       = 0x76,
    HAYDN_DMA_W1_STS_2       = 0x78,
    HAYDN_DMA_W1_TST_0       = 0x7A,
    HAYDN_DMA_W1_TST_1       = 0x7C,
    HAYDN_DMA_W1_TST_2       = 0x7E,
};


enum{
    HAYDN_AUSDM_CFG16        = 0xD6,
    HAYDN_AUSDM_CFG5         = 0xE4,
    HAYDN_AUSDM_CFG14        = 0xF6
};


#define REG_AUDIOBAND_CFG_02 0x032004
#define REG_VLD_GEN_CFG_0 0x032080


/**
 * @brief Register 32004h
 */
#define AUD_CODEC_ADC_DEC_POS                          8
#define AUD_CODEC_ADC_DEC_MSK                          (0xF<<AUD_CODEC_ADC_DEC_POS)


/**
 * @brief Register 32080h
 */
#define AUD_VLD_GEN_ADC_EN                             (1<<7)
#define AUD_VLD_GEN_ADC_FS_SEL_POS                     0
#define AUD_VLD_GEN_ADC_FS_SEL_MSK                     (0x7<<AUD_VLD_GEN_ADC_FS_SEL_POS)


//cfg0
#define REG_SW_RST_DMA               (1 << 15)
#define REG_WR_BIT_MODE              (1 << 9)
#define REG_WR_FULL_FLAG_CLR         (1 << 7)
#define REG_WR_LEVEL_CNT_MASK        (1 << 5)
#define REG_WR_TRIG                  (1 << 4)
#define REG_WR_SWAP                  (1 << 3)
#define REG_WR_FREERUN               (1 << 2)
#define REG_WR_ENABLE                (1 << 1)
#define REG_MIU_REQUSTER_ENABLE      (1 << 0)

//cfg1
#define REG_WR_BASE_ADDR_LO_POS      0
#define REG_WR_BASE_ADDR_LO_MSK      (0xFFFF << REG_WR_BASE_ADDR_LO_POS)

//cfg2
#define REG_WR_BASE_ADDR_HI_OFFSET   16
#define REG_WR_BASE_ADDR_HI_POS      0
#define REG_WR_BASE_ADDR_HI_MSK      (0x0FFF << REG_WR_BASE_ADDR_HI_POS)

//cfg3
#define REG_WR_BUFF_SIZE_POS         0
#define REG_WR_BUFF_SIZE_MSK         (0xFFFF << REG_WR_BUFF_SIZE_POS)

//cfg4
#define REG_WR_SIZE_POS              0
#define REG_WR_SIZE_MSK              (0xFFFF << REG_WR_SIZE_POS)

//cfg5
#define REG_WR_OVERRUN_TH_POS        0
#define REG_WR_OVERRUN_TH_MSK        (0xFFFF << REG_WR_OVERRUN_TH_POS)

//cfg8
#define REG_WR_LOCAL_FULL_INT_EN     (1 << 2)
#define REG_WR_OVERRUN_INT_EN        (1 << 1)
#define REG_WR_FULL_INT_EN           (1 << 0)


//sts1
#define REG_WR_LOCALBUF_FULL_STATUS  (1 << 9)
#define REG_WR_FULL_STATUS           (1 << 8)
#define REG_WR_OVERRUN_FLAG          (1 << 1)
#define REG_WR_FULL_FLAG             (1 << 0)

//AU_INT
//wriu -w 0x032296
//[15] intr debug
//[14] intr debug value
//[2] en dma intr
//[1] en vrec intr
//[0] en usb intr
#define REG_WR_AU_INT_DEBUG_ENABLE   (1 << 15)
#define REG_WR_AU_INT_DEBUG_VALUE    (1 << 14)
#define REG_WR_AU_INT_DMA_ENABLE     (1 << 2)
#define REG_WR_AU_INT_VREC_ENABLE    (1 << 1)
#define REG_WR_AU_INT_USB_ENABLE     (1 << 0)


//0x321D6
#define REG_SEL_GAIN_INMUX1_L_POS    13
#define REG_SEL_GAIN_INMUX1_L_MSK    (0x7<<REG_SEL_GAIN_INMUX1_L_POS)

#define REG_SEL_GAIN_INMUX1_R_POS    10
#define REG_SEL_GAIN_INMUX1_R_MSK    (0x7<<REG_SEL_GAIN_INMUX1_R_POS)


//0x321E4
#define REG_SEL_GAIN_INMUX0_L_POS    13
#define REG_SEL_GAIN_INMUX0_L_MSK    (0x7<<REG_SEL_GAIN_INMUX0_L_POS)

#define REG_SEL_GAIN_INMUX0_R_POS    10
#define REG_SEL_GAIN_INMUX0_R_MSK    (0x7<<REG_SEL_GAIN_INMUX0_R_POS)


//0x321F6
#define REG_SEL_GAIN_R_MICAMP1_POS   13
#define REG_SEL_GAIN_R_MICAMP1_MSK   (0x7<<REG_SEL_GAIN_R_MICAMP1_POS)

#define REG_SEL_GAIN_R_MICAMP0_POS   10
#define REG_SEL_GAIN_R_MICAMP0_MSK   (0x7<<REG_SEL_GAIN_R_MICAMP0_POS)

#define REG_SEL_GAIN_L_MICAMP1_POS   7
#define REG_SEL_GAIN_L_MICAMP1_MSK   (0x7<<REG_SEL_GAIN_L_MICAMP1_POS)

#define REG_SEL_GAIN_L_MICAMP0_POS   4
#define REG_SEL_GAIN_L_MICAMP0_MSK   (0x7<<REG_SEL_GAIN_L_MICAMP0_POS)


// cytsai
#ifndef NULL
#define NULL    ((void*)0)
#endif

