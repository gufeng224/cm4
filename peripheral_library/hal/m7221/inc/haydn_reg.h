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


// 0x032522
#define REG_PAGA_CTRL_11 0x032522
#define REG_I2S_TDM_PAD_OEN                  (1<<15)
#define REG_OVBURST_MODE                     (1<<14)
#define REG_CODEC_I2S_LOOP_BACK              (1<<13)
#define REG_PAGA_PDM_PADSEL_POS               6
#define REG_PAGA_PDM_PADSEL_MSK              (0x3<<REG_PAGA_PDM_PADSEL_POS)
#define REG_SEL_CLK_SRC_A1_256FSi_POS         4
#define REG_SEL_CLK_SRC_A1_256FSi_MSK        (0x3<<REG_SEL_CLK_SRC_A1_256FSi_POS)
#define REG_PAGA_I2S_TDM_PADSEL_POS           2
#define REG_PAGA_I2S_TDM_PADSEL_MSK          (0x3<<REG_PAGA_I2S_TDM_PADSEL_POS)
#define REG_PAGA_PAD_TDM_MD                  (1<<0 )

// 0x032560
#define REG_I2S_TDM_CFG_00 0x032560
#define REG_ENABLE_CODEC_I2S_RX_BCK_GEN      (1<<15)
#define REG_CODEC_I2S_RX_MS_MODE             (1<<14)
#define REG_CODEC_I2S_RX_TDM_MODE            (1<<13)
#define REG_CODEC_I2S_RX_ENC_WDTH            (1<< 9)
#define REG_CODEC_I2S_RX_CHLEN               (1<< 8)
#define REG_CODEC_I2S_RX_WS_FMT              (1<< 7) //TDM
#define REG_CODEC_I2S_RX_FMT                 (1<< 6)
#define REG_CODEC_I2S_RX_WS_INV              (1<< 5)
#define REG_CODEC_I2S_RX_BCK_INV             (1<< 4)
#define REG_CODEC_I2S_RX_BCK_DG_EN           (1<< 3)
#define REG_CODEC_I2S_RX_BCK_DG_NUM_POS       0
#define REG_CODEC_I2S_RX_BCK_DG_NUM_MSK      (0x7<<REG_CODEC_I2S_RX_BCK_DG_NUM_POS)

// 0x032562
#define REG_I2S_TDM_CFG_01 0x032562
#define REG_MISC_I2S_RX_WS_WDTH_POS           10
#define REG_MISC_I2S_RX_WS_WDTH_MSK          (0x1f<<REG_PAGA_I2S_TDM_PADSEL_POS)
#define REG_CODEC_I2S_RX_WS_WDTH_POS          5
#define REG_CODEC_I2S_RX_WS_WDTH_MSK         (0x1f<<REG_CODEC_I2S_RX_WS_WDTH_POS)
#define REG_BT_I2S_RX_WS_WDTH_POS             0
#define REG_BT_I2S_RX_WS_WDTH_MSK            (0x1f<<REG_BT_I2S_RX_WS_WDTH_POS)


// 0x032564
#define REG_I2S_TDM_CFG_02 0x032564
//#define REG_MISC_I2S_RX_CH_SWAP              = REG_I2S_TDM_CFG_02 [5:4];
//#define REG_BT_I2S_RX_CH_SWAP                = REG_I2S_TDM_CFG_02 [3:2];
//#define REG_CODEC_I2S_RX_CH_SWAP             = REG_I2S_TDM_CFG_02 [1:0];


// 0x032530 CLK
#define REG_PAGA_CTRL_18 0x032530
#define REG_I2S_TX_SYNTH_ENABLE               (1<<12)
#define REG_ENABLE_I2S_TX_BCK_GEN             (1<<11)
#define REG_ENABLE_CLK_I2S_TX_256FSi          (1<<10)
#define REG_ENABLE_CLK_CODEC_I2S_TXI          (1<< 9)
#define REG_ENABLE_CLK_CODEC_I2S_TX_BCK       (1<< 8)
#define REG_CODEC_MCK_SYNTH_EXP_EN            (1<< 7)
#define REG_ENABLE_CLK_NF_SYNTH_REF           (1<< 6)
#define REG_ENABLE_CLK_DE_GLITCH_REF          (1<< 5)
#define REG_ENABLE_CLK_CODEC_I2S_RX_BCK       (1<< 4)
#define REG_ENABLE_CLK_CODEC_I2S_MCK          (1<< 3)
#define REG_ENABLE_CLK_FS_SYNTH_REF           (1<< 2)
#define REG_ENABLE_CLK_MCK_SYNTH_REF          (1<< 1)
#define REG_ENABLE_CLK_SRC_A1_256FSi          (1<< 0)

// 0x0322532
#define REG_PAGA_CTRL_19 0x0322532
#define REG_INV_CLK_DE_GLITCH_REF             (1<<15)
#define REG_CLK_MCU_INV                       (1<<14)
#define REG_CLK_RIUBRG_INV                    (1<<13)
#define REG_CM4_TCK_INV                       (1<<12)
#define REG_INV_CLK_I2S_TX_256FSi             (1<< 8)
#define REG_INV_CLK_CODEC_I2S_TX_BCK          (1<< 7)
#define REG_INV_CLK_NF_SYNTH_REF              (1<< 6)
#define REG_INV_CLK_CODEC_I2S_TXI             (1<< 5)
#define REG_INV_CLK_CODEC_I2S_RX_BCK          (1<< 4)
#define REG_INV_CLK_CODEC_I2S_MCK             (1<< 3)
#define REG_INV_CLK_FS_SYNTH_REF              (1<< 2)
#define REG_INV_CLK_MCK_SYNTH_REF             (1<< 1)
#define REG_INV_CLK_SRC_A1_256FSi             (1<< 0)

// 0x032534
#define REG_PAGA_CTRL_1A 0x032534
// wire [2:0]  REG_SEL_CLK_SPDIF_TX_256FS            = REG_PAGA_CTRL_1A [14:12];
// wire [1:0]  REG_SEL_MISC_I2S_TX_BCK_FS            = REG_PAGA_CTRL_1A [11:10];
// wire [1:0]  REG_SEL_MISC_I2S_RX_BCK_FS            = REG_PAGA_CTRL_1A [ 9: 8];
// wire [1:0]  REG_SEL_BT_I2S_TX_BCK_FS              = REG_PAGA_CTRL_1A [ 7: 6];
// wire [1:0]  REG_SEL_BT_I2S_RX_BCK_FS              = REG_PAGA_CTRL_1A [ 5: 4];
// wire [1:0]  REG_SEL_CODEC_I2S_TX_BCK_FS           = REG_PAGA_CTRL_1A [ 3: 2];
#define REG_SEL_CODEC_I2S_RX_BCK_FS_POS         0
#define REG_SEL_CODEC_I2S_RX_BCK_FS_MSK        (0x3<<REG_SEL_CODEC_I2S_RX_BCK_FS_POS)


// 0x03252C REG_PAGA_CTRL_16
#define REG_CODEC_MCK_SYNTH_NF_H 0x03252C
#define REG_CODEC_MCK_SYNTH_NF_H_POS           0
#define REG_CODEC_MCK_SYNTH_NF_H_MSK          (0xff<<REG_CODEC_MCK_SYNTH_NF_H_POS)
#define REG_CODEC_EXPAND_LEVEL_POS             8
#define REG_CODEC_EXPAND_LEVEL_MSK            (0xff<<REG_CODEC_EXPAND_LEVEL_POS)

// 0x03252E REG_PAGA_CTRL_17
#define REG_CODEC_MCK_SYNTH_NF_L 0x03252E

// 0x0325E8
#define REG_CODEC_RX_NF_SYNTH_L 0x0325E8

// 0x0325EA
#define REG_CODEC_RX_NF_SYNTH_H 0x0325EA

// 0x0325EC
#define REG_NF_SYNTH_EN_TRIG 0x0325EC
#define REG_CODEC_MCK_EN_TIME_GEN            (1<<3)
#define REG_CODEC_MCK_NF_SYNTH_TRIG          (1<<2)
#define REG_CODEC_RX_EN_TIME_GEN             (1<<1)
#define REG_CODEC_RX_NF_SYNTH_TRIG           (1<<0)

// cytsai
#ifndef NULL
#define NULL    ((void*)0)
#endif

