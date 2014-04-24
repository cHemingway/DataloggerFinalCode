typedef signed char		s8;  /*  8 bits */
typedef signed short int	s16; /* 16 bits */
typedef signed long int	s32; /* 32 bits */

#define NUM_DSPI                3
#define DSPI0                   0
#define DSPI1                   1
#define DSPI2                   2

/* Bit definitions and macros for SPI_SR */
#define SPI_SR_RFDF             (0x00020000)
#define SPI_SR_RFOF             (0x00080000)
#define SPI_SR_TFFF             (0x02000000)
#define SPI_SR_TFUF             (0x08000000)
#define SPI_SR_EOQF             (0x10000000)
#define SPI_SR_TXRXS            (0x40000000)
#define SPI_SR_TCF              (0x80000000)

/* Bit definitions and macros for SPI_RSER */
#define SPI_RSER_RFDF_DIRS      (0x00010000)
#define SPI_RSER_RFDF_RE        (0x00020000)
#define SPI_RSER_RFOF_RE        (0x00080000)
#define SPI_RSER_TFFF_DIRS      (0x01000000)
#define SPI_RSER_TFFF_RE        (0x02000000)
#define SPI_RSER_TFUF_RE        (0x08000000)
#define SPI_RSER_EOQF_RE        (0x10000000)
#define SPI_RSER_TCF_RE         (0x80000000)

#define SPI_CTAR0_LSBFE         (0x01000000)
#define SPI_CTAR0_CPHA          (0x02000000)
#define SPI_CTAR0_CPOL          (0x04000000)

typedef struct{
uint32_t mstr;
uint32_t pcs;
uint32_t hlt;
uint32_t fmsz;
uint32_t cpol;
uint32_t cpha;
uint32_t lsbfe;
uint32_t br;
uint32_t tcf;
uint32_t eoqf;
uint32_t cont;
uint32_t ctas;
uint32_t delay;
uint32_t eoq;
uint32_t pcsx;
uint32_t txdata;
uint32_t buffer;
        
} dspi_ctl;

enum DSPI_COMMAND
{
    DSPI_PINS_ENB,//-->0
    MEM_WREN,//-->1
    SECTOR_ERASE,//-->2
    WAIT_WR_OVER,//-->3
    CHK_MEM_SR,//-->4
    CHK_WREN_SR,//-->5
    WRITE_TX_DATA,//-->6
    READ_DATA,//-->7
    EOQF_INT_CONFIG,//-->8
    TCF_INT_CONFIG,//-->9
    TFFF_INT_CONFIG,//-->10
    RFOF_INT_CONFIG,//-->11
    RFDF_INT_CONFIG,//-->12
    TFFF_DMA_CONFIG,//-->13
    RFDF_DMA_CONFIG,//-->14
    DSPI_STOP_MODE,//-->15
    MOD_READ_DATA, //-->16
    MOD_CHK_WREN_SR,//-->17
    MOD_CHK_MEM_SR,//-->18
    MOD_MEM_WREN, //--->19
    CONT_SCK_READ_DATA, //---20
    DELAY_READ_DATA,  //-----21	        
    MEM_RDID
          
};



void dspi_config(int COMMAND ,dspi_ctl *dspi_value);
/*dspi_value->mstr = 0x0000;
dspi_value->pcs = 0x0001;
dspi_value->hlt = 0x0000;
dspi_value->fmsz = 0x0000;
dspi_value->cpol = 0x0000;
dspi_value->cpha = 0x0000;
dspi_value->lsbfe = 0x0000;
dspi_value->br = 0x0000;
dspi_value->tcf = 0x0000;
dspi_value->eoqf = 0x0000;
dspi_value->cont = 0x0000;
dspi_value->ctas = 0x0000;
dspi_value->eoq = 0x0000;
dspi_value->pcsx = 0x0000;
dspi_value->txdata = 0x0000;*/
