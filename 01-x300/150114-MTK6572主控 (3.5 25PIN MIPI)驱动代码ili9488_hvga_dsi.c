#ifdef BUILD_LK
    #include <platform/mt_gpio.h>
    #define print(x...) printf(x)
#else
    #include <linux/string.h>
    #include <mach/mt_gpio.h>
    #define print(x...) printk(x)
#endif

#include "lcm_drv.h"
#include <cust_gpio_usage.h>


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(320)
#define FRAME_HEIGHT 										(480)

#define REGFLAG_DELAY             							0XAA
#define REGFLAG_END_OF_TABLE      							0xAB   // END OF REGISTERS MARKER

#define LCM_DSI_CMD_MODE                                    0

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

//#define ILI9806E    1
//#define ILI9806C    1

#define _BACKLIGHT_PIN      GPIO134
#define BACKLIGHT_HIGH()    mt_set_gpio_out(_BACKLIGHT_PIN, GPIO_OUT_ONE)
#define BACKLIGHT_LOW()     mt_set_gpio_out(_BACKLIGHT_PIN, GPIO_OUT_ZERO)
// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   			lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)        

static unsigned int lcm_read(void);

static struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[128];
};


static struct LCM_setting_table lcm_initialization_setting[] = {
	
    //************* Start Initial Sequence **********//
//************* Start Initial Sequence **********//
#if 0
    {0xFF,3, {0xFF,0x98,0x16}},

    {0xBA,1,{0x60}},

    {0xB0,1,{0x01}},


    {0xBC,18,{0x01,0x0F,0x61,0x39,0x01,0x01,0x1B,0x11,0x38,0x63,0xFF,0xFF,0x01,0x01,0x0D,0x00,0xFF,0XF2}}, // GIP 1

    {0xBD,8,{0x01,0x23,0x45,0x67,0x01,0x23,0x45,0x67}}, // GIP 2

    {0xBE,17,{0x13,0x22,0x22,0x22,0x22,0xBB,0xAA,0xDD,0xCC,0x22,0x66,0x22,0x88,0x22,0x22,0x22,0x22}}, // GIP 3

    {0xED,2,{0x7F,0x0F}}, // en_volt_reg measure VGMP

    {0xF3,1,{0x70}},

    {0XB4,1,{0x02}}, // Display Inversion Control

    {0XC0,3,{0x0F,0x0B,0x0A}}, // Power Control 1

    {0XC1,4,{0x17,0x80,0x80,0x20}}, // Power Control 2

    {0XD8,1,{0x50}}, // VGLO Selection

    {0XFC,1,{0x08}}, // VGLO Selection

    {0XE0,16,{0x00,0x06,0x0E,0x0C,0x10,0x0B,0XD6,0x05,0x08,0x0C,0x10,0x0F,0x0C,0x15,0x11,0x00}}, // Positive Gamma Control

    {0XE1,16,{0x00,0x0C,0x10,0x0C,0x0C,0x08,0X77,0x04,0x08,0x0C,0x10,0x0F,0x0A,0x08,0x05,0x00}}, // Negative Gamma Control

    {0XD5,8,{0x0A,0x09,0x0F,0x06,0xCB,0XA5,0x01,0x04}}, // Source Timing Adjust

    {0XF7,1,{0x8A}}, // Resolution

    {0XC7,1,{0x70}}, // Vcom

    {0X11,1,{0x00}}, // Exit Sleep
    {REGFLAG_DELAY, 120, {}},
    {0X29,1,{0x00}}, // Display On
        
    {REGFLAG_DELAY, 200, {}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
#endif

    /**********************ili9488_dsi*********************/

    // Adjust Control 3
    {0xF7, 4, {0xA9,0x51,0x2C,0x82}},

    // Adjust Control 4
    {0xF8, 2, {0x21,0x05}},

    // Power Control
    {0xC0, 2, {0x11,0x09}},

    // Power Control
    {0xC1, 1,{0x41}},

    // Power Control
    {0xC5, 3,{0x00,0x2A,0x80}},

    // Frame Rate Control
    {0xB1, 2,{0xB0,0x11}},

    // Display Inversion Control
    {0xB4, 1,{0x02}},

    // Display Function Control
    {0xB6, 3,{0xB0,0x22,0x3b}},

    // Entry Mode Set
    {0xB7, 1,{0xc6}},

    // HS Lanes Control
    {0xBE, 2,{0x00,0x04}},

    // Set Image Function

    {0xE9, 1,{0x01}},

    // Memory Access Control
    {0x36, 1,{0x08}},

    // Interface Pixel Format 
    {0x3A, 1,{0x66}},

    // Positive Gamma Control
    {0xE0, 15,{0x00,0x07,0x10,0x09,0x17,0x0B,0x41,0x89,0x4B,0x0A,0x0C,0x0E,0x18,0x1B,0x0F}},

    // Negative Gamma Control
    {0xE1, 15,{0x00,0x17,0x1A,0x04,0x0E,0x06,0x2F,0x45,0x43,0x02,0x0A,0x09,0x32,0x36,0x0F}},

    {0x11,  0,  {}},
    {REGFLAG_DELAY, 100, {}}, //sleep out,delay 200 MS
    {0x29,  0,  {}},
    {REGFLAG_DELAY, 50, {}},  //display on

    // Setting ending by predefined flag
    {REGFLAG_END_OF_TABLE, 0x00, {}}

};


static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
   // {0x2C, 0, {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 0, {0x00}},
    {REGFLAG_DELAY, 150, {}},

    // Display ON
	{0x29, 0, {0x00}},
    {REGFLAG_DELAY, 20, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28, 0, {0x00}},
    {REGFLAG_DELAY, 50, {}},

    // Sleep Mode On
	{0x10, 0, {0x00}},
    {REGFLAG_DELAY, 120, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_backlight_level_setting[] = {
	{0x51, 1, {0xFF}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
    }
	
}

static void lcd_reset(void)
{
//    mt_set_gpio_mode(GPIO67, GPIO_MODE_GPIO);
//    mt_set_gpio_dir(GPIO67, GPIO_DIR_OUT);
//    mt_set_gpio_out(GPIO67, GPIO_OUT_ZERO);
//
/*
    mt_set_gpio_mode(GPIO18, GPIO_MODE_GPIO);
    mt_set_gpio_dir(GPIO18, GPIO_DIR_OUT);

    mt_set_gpio_out(GPIO18, GPIO_OUT_ONE);
	MDELAY(10);
    mt_set_gpio_out(GPIO18, GPIO_OUT_ZERO);
	MDELAY(20);
    mt_set_gpio_out(GPIO18, GPIO_OUT_ONE);
	MDELAY(120);
*/
}

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED;//LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else	
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif		
        // DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_ONE_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		// Video mode setting		
		params->dsi.intermediat_buffer_num = 2;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	    params->dsi.word_count = 480*3;

//LG4573B
		params->dsi.vertical_sync_active				= 50;
		params->dsi.vertical_backporch					= 2;
		params->dsi.vertical_frontporch					= 20;
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 50;
		params->dsi.horizontal_backporch				= 80;
		params->dsi.horizontal_frontporch				= 350;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		// Bit rate calculation
		params->dsi.pll_div1=0;		// fref=26MHz, fvco=fref*(div1+1)	(div1=0~63, fvco=500MHZ~1GHz)
		params->dsi.pll_div2=1; 		// div2=0~15: fout=fvo/(2*div2)
	    params->dsi.fbk_div =19;		// fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)		
		//params->dsi.pll_div1=28;    	// fref=26MHz, fvco=fref*(div1+1)	(div1=0~63, fvco=500MHZ~1GHz)
		//params->dsi.pll_div2=1; 		// div2=0~15: fout=fvo/(2*div2)

        params->dsi.lcm_int_te_monitor = FALSE;
        
        params->dsi.lcm_int_te_period = 1;

        if(params->dsi.lcm_int_te_monitor)
            params->dsi.vertical_frontporch *= 2;

        params->dsi.lcm_ext_te_monitor = FALSE;

        params->dsi.noncont_clock = TRUE;
        params->dsi.noncont_clock_period = 2;
}


static void lcm_init(void)
{
    unsigned int data_array[16];
#if 0
    
    mt_set_gpio_mode(_BACKLIGHT_PIN, GPIO_MODE_00);
    mt_set_gpio_dir(_BACKLIGHT_PIN, GPIO_DIR_OUT);
    mt_set_gpio_pull_select(_BACKLIGHT_PIN, GPIO_PULL_UP);
    mt_set_gpio_pull_enable(_BACKLIGHT_PIN, GPIO_PULL_ENABLE);
    BACKLIGHT_LOW();
#endif
	SET_RESET_PIN(1);
	MDELAY(5);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(50);

   
    //BACKLIGHT_HIGH();
//    lcd_reset();

	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

    //mt_set_gpio_dir(_BACKLIGHT_PIN, GPIO_DIR_OUT);
    //mt_set_gpio_mode(_BACKLIGHT_PIN, GPIO_MODE_00);
    //mt_set_gpio_pull_select(_BACKLIGHT_PIN, GPIO_PULL_UP);
    //mt_set_gpio_pull_enable(_BACKLIGHT_PIN, GPIO_PULL_ENABLE);
    //BACKLIGHT_LOW();


    //MDELAY(2000);

    //BACKLIGHT_HIGH();
}

static void lcm_suspend(void)
{
    //BACKLIGHT_LOW();
#if 0
    mt_set_gpio_mode(GPIO67, GPIO_MODE_GPIO);
    mt_set_gpio_dir(GPIO67, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO67, GPIO_OUT_ZERO);
#endif

	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);

        SET_RESET_PIN(0);
        MDELAY(10);
        SET_RESET_PIN(1);
        MDELAY(10);
}



static void lcm_resume(void)
{
    /*
    mt_set_gpio_mode(GPIO67, GPIO_MODE_GPIO);
    mt_set_gpio_dir(GPIO67, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO67, GPIO_OUT_ONE);

    */
#if 1
    //lcd_reset();

#if 0
	SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(130);
#endif
	lcm_init();

	//push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);


#else
    lcm_init();
#endif
}


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00053902;
	data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[5]= (y1_LSB);
	data_array[6]= 0x002c3909;

	dsi_set_cmdq(&data_array, 7, 0);

}


static void lcm_setbacklight(unsigned int level)
{
	unsigned int default_level = 145;
	unsigned int mapped_level = 0;

	//for LGE backlight IC mapping table
	if(level > 255) 
			level = 255;

	if(level >0) 
			mapped_level = default_level+(level)*(255-default_level)/(255);
	else
			mapped_level=0;

	// Refresh value of backlight level.
	lcm_backlight_level_setting[0].para_list[0] = mapped_level;

	push_table(lcm_backlight_level_setting, sizeof(lcm_backlight_level_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_setpwm(unsigned int divider)
{
	// TBD
}


static unsigned int lcm_getpwm(unsigned int divider)
{
	// ref freq = 15MHz, B0h setting 0x80, so 80.6% * freq is pwm_clk;
	// pwm_clk / 255 / 2(lcm_setpwm() 6th params) = pwm_duration = 23706
	unsigned int pwm_clk = 23706 / (1<<divider);	
	return pwm_clk;
}

static unsigned int lcm_read(void)
{
	unsigned int id = 0;
	unsigned char buffer[2]={0x88};
	unsigned int array[16];

	array[0] = 0x00013700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);
//	id = read_reg(0xF4);
	read_reg_v2(0x52, buffer, 1);
	id = buffer[0]; //we only need ID
#ifndef BUILD_UBOOT
	printk("\n\n\n\n\n\n\n\n\n\n[soso]%s, lcm_read = 0x%08x\n", __func__, id);
#endif
    //return (LCM_ID == id)?1:0;
}


LCM_DRIVER ili9488_hvga_dsi_drv = 
{
    .name			= "ili9488_hvga_dsi",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
};

