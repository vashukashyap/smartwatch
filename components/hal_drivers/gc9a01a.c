#include <stdio.h>
#include <malloc.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <gc9a01a.h>


// spi handle
static spi_device_handle_t handle;

static const char *TAG = "GC9A01A Driver";

// this is a virtual display buffer
uint8_t* v_display_buffer = NULL;



/*
    @brief this function initalized the spi connection required for GC9A01A display
    @param void
    @return void
*/
void gc9a01a_init_conn()
{
     //Configuration for the SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = GPIO_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = GC9A01A_TFTHEIGHT * GC9A01A_TFTWIDTH * 2 + 8 
    };

    ESP_ERROR_CHECK( spi_bus_initialize(SENDER_HOST, &buscfg, SPI_DMA_CH1) );
    
    ESP_LOGI(TAG, "master SPI bus is initalized");

    //Configuration for the SPI device on the other side of the bus
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 60000000,
        .mode = 0,
        .spics_io_num = GPIO_CS,      //Keep the CS low 3 cycles after transaction, to stop slave from missing the last bit when CS has less propagation delay than CLK
        .queue_size = 7,
        .flags = SPI_DEVICE_NO_DUMMY,
        .pre_cb = NULL
    };

    ESP_ERROR_CHECK( spi_bus_add_device(SENDER_HOST, &devcfg, &handle) );

    ESP_LOGI(TAG, "master SPI device is initalized");

    //Configuration for the D/C pin
    gpio_set_direction(GPIO_DC, GPIO_MODE_OUTPUT);


    ESP_LOGI(TAG, "display connection initalized");

}


/*
    @brief this function send command to GC9A01A display
    @param cmd 
                command of the GC9A01A display
    @return void
*/
void gc9a01a_send_cmd(uint8_t cmd)
{
    gpio_set_level(GPIO_DC, 0);                         // turning D/C low for sending command

    spi_transaction_t cmd_t = {
        .length = 8,
        .tx_buffer = &cmd
    };

    ESP_ERROR_CHECK( spi_device_transmit(handle, &cmd_t));
}


/*
    @brief this function send data to GC9A01A display
    @param data  
                address of data buffer
    @param len 
                length of the data
    @return void
*/
void gc9a01a_send_data(const uint8_t *data, int len)
{
    if(len==0) return;

    gpio_set_level(GPIO_DC, 1);                 // turning D/C high for sending command
    spi_transaction_t data_t = {
        .length = len * 8,                      // since spi send bits and data uint8_t therefor we are multiplying it by 8 for exact length
        .tx_buffer = data
    };

    ESP_ERROR_CHECK( spi_device_transmit(handle, &data_t));
}

/*
    @brief send and display the whole virtual buffer on GC9A01A display
    @param v_buffer  
                address of v_dsiplay_buffer
    @return void
*/
void gc9a01a_send_v_display_buffer(uint8_t *v_buffer)
{
    if(v_buffer==NULL){
        ESP_LOGW(TAG, "v_display_buffer is NULL");
        return;
    }
    gc9a01a_send_cmd(0x2C);
    gc9a01a_send_data(v_display_buffer, (GC9A01A_TFTHEIGHT*GC9A01A_TFTWIDTH*2));
}


/*
    @brief initalized GC9A01A display with the vendor commands
    @return void
*/
void gc9a01a_init()
{
    // -------- this is the startup commands provided by the vendor.
    static const uint8_t initcmd[224] = {
        GC9A01A_INREGEN2, 0,
        0xEB, 1, 0x14, // ?
        GC9A01A_INREGEN1, 0,
        GC9A01A_INREGEN2, 0,
        0xEB, 1, 0x14, // ?
        0x84, 1, 0x40, // ?
        0x85, 1, 0xFF, // ?
        0x86, 1, 0xFF, // ?
        0x87, 1, 0xFF, // ?
        0x88, 1, 0x0A, // ?
        0x89, 1, 0x21, // ?
        0x8A, 1, 0x00, // ?
        0x8B, 1, 0x80, // ?
        0x8C, 1, 0x01, // ?
        0x8D, 1, 0x01, // ?
        0x8E, 1, 0xFF, // ?
        0x8F, 1, 0xFF, // ?
        0xB6, 2, 0x00, 0x00, // ?
        GC9A01A_MADCTL, 1, MADCTL_MX | MADCTL_BGR,
        GC9A01A_COLMOD, 1, 0x05,
        0x90, 4, 0x08, 0x08, 0x08, 0x08, // ?
        0xBD, 1, 0x06, // ?
        0xBC, 1, 0x00, // ?
        0xFF, 3, 0x60, 0x01, 0x04, // ?
        GC9A01A1_POWER2, 1, 0x13,
        GC9A01A1_POWER3, 1, 0x13,
        GC9A01A1_POWER4, 1, 0x22,
        0xBE, 1, 0x11, // ?
        0xE1, 2, 0x10, 0x0E, // ?
        0xDF, 3, 0x21, 0x0c, 0x02, // ?
        GC9A01A_GAMMA1, 6, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2A,
        GC9A01A_GAMMA2, 6, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6F,
        GC9A01A_GAMMA3, 6, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2A,
        GC9A01A_GAMMA4, 6, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6F,
        0xED, 2, 0x1B, 0x0B, // ?
        0xAE, 1, 0x77, // ?
        0xCD, 1, 0x63, // ?
        // Unsure what this line (from manufacturer's boilerplate code) is
        // meant to do, but users reported issues, seems to work OK without:
        //0x70, 9, 0x07, 0x07, 0x04, 0x0E, 0x0F, 0x09, 0x07, 0x08, 0x03, // ?
        GC9A01A_FRAMERATE, 1, 0x34,
        0x62, 12, 0x18, 0x0D, 0x71, 0xED, 0x70, 0x70, // ?
                    0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70,
        0x63, 12, 0x18, 0x11, 0x71, 0xF1, 0x70, 0x70, // ?
                    0x18, 0x13, 0x71, 0xF3, 0x70, 0x70,
        0x64, 7, 0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07, // ?
        0x66, 10, 0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00, // ?
        0x67, 10, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98, // ?
        0x74, 7, 0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00, // ?
        0x98, 2, 0x3e, 0x07, // ?
        GC9A01A_TEON, 0,
        GC9A01A_INVON, 0,
        GC9A01A_SLPOUT, 0x80, // Exit sleep
        GC9A01A_DISPON, 0x80, // Display on
        0x00                  // End of list
    };
    
    size_t i = 0;
    while (i<224)
    {
        uint8_t cmd = initcmd[i++];
        uint8_t arg_count = initcmd[i++] & 0x7F;
        gc9a01a_send_cmd(cmd);
        if(arg_count>0)
        {
            gc9a01a_send_data(&initcmd[i], arg_count);
            i+=arg_count;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // allocating the virtual memory to the v_display_buffer from DMA
    v_display_buffer = (uint8_t*) heap_caps_malloc(GC9A01A_TFTHEIGHT*GC9A01A_TFTWIDTH*2, MALLOC_CAP_DMA);

    if(v_display_buffer==NULL)
    {
        ESP_LOGW(TAG, "unable to allocate memory for virtual buffer");
    }
    
    // initalized the v_display_buffer with the black color
    uint8_t upperbyte = (GC9A01A_BLACK >> 8) & 0xFF;
    uint8_t lowerbyte = (GC9A01A_BLACK) & 0xFF;

    for(int i=0; i<GC9A01A_TFTWIDTH*GC9A01A_TFTHEIGHT; i++)
    {
        v_display_buffer[i*2] = upperbyte;
        v_display_buffer[i*2+1] = lowerbyte;
        if(i%240==0) vTaskDelay(pdMS_TO_TICKS(10));
    }

    gc9a01a_draw_cursor_set(0,0,GC9A01A_TFTWIDTH-1,GC9A01A_TFTHEIGHT-1);

    
    gc9a01a_send_v_display_buffer(v_display_buffer);

    

    ESP_LOGI(TAG, "Display is initalized with vendor commands");
}

/*
    @brief change buffer display mode of GC9A01A display
    @param mode  
                the mode in which  data to be display "GC9A01A_NORMAL_MODE" or "GC9A01A_PARTIAL_MODE"
    @return void
    @note   read datasheet of GC9A01A display for knowing about the Normal mode and Partial mode.
*/
void gc9a01a_mode(gc9a01a_modes_t mode)
{
    switch (mode)
    {
    case GC9A01A_NORMAL_MODE:
        gc9a01a_send_cmd(GC9A01A_NORON);
        break;
    
    case GC9A01A_PARTIAL_MODE:
        gc9a01a_send_cmd(GC9A01A_PTLON);
        break;
    }
}

/*
    @brief set the cursor position of the GC9A01A display ram to send buffer
    @param x0  
                stating x-coordinate
    @param y0  
                stating y-coordinate
    @param x1  
                ending x-coordinate
    @param y1  
                ending y-coordinate
    @return void
*/
void gc9a01a_draw_cursor_set(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t col_paras[4] = { 
        ((x0 >> 8ULL) & 0xFF), (x0 & 0xFF),
        ((x1 >> 8ULL) & 0xFF), (x1 & 0xFF)
    };

    uint8_t row_paras[4] = {
        ((y0 >> 8ULL) & 0xFF), (y0 & 0xFF),
        ((y1 >> 8ULL) & 0xFF), (y1 & 0xFF)
    };

    gc9a01a_send_cmd(0x2A);
    gc9a01a_send_data(col_paras, 4);
    gc9a01a_send_cmd(0x2B);
    gc9a01a_send_data(row_paras, 4);

    
}

/*
    @brief set the cursor position of the GC9A01A display ram to send buffer for PARTIAL MODE
    @param start_row  
                vertical pixel to start with.
    @param` end_row  
                vertical pixel to end with.
    @return void
    @note   Columm or horizontal pixels are set by the ' gc9a01a_draw_cursor_set( ) ' function.
*/
void gc9a01a_draw_screen_partial(uint16_t start_row, uint16_t end_row)
{

    uint8_t paras[4] = {
        ((start_row >> 8) & 0xFF), (start_row & 0xFF),
        ((end_row >> 8) & 0xFF), (end_row & 0xFF)
    };

    gc9a01a_send_cmd(0x30);
    gc9a01a_send_data(paras, 4);
    
}

/*
    @brief this function draw the pixel on the GC9A01A display without any v_display_buffer
    @param  pos_x  
                x coordinate of pixel
    @param  pos_y  
                y coordinate of pixel
    @param  color
                Color value in hexadecimal (size : 2 byte)
    @return void
*/
void gc9a01a_draw_screen_pixel(uint16_t pos_x, uint16_t pos_y, uint16_t color)
{
    gc9a01a_draw_cursor_set(pos_x, pos_y, pos_x, pos_y);

    gc9a01a_send_cmd(0x2C);
    
    uint8_t pixel[2] = {(color >> 8) & 0xFF, (color) & 0xFF};
    gc9a01a_send_data(pixel, 2);
}

/*
    @brief this function disable/enable the GC9A01A display ram data display on the screen
    @param  mode  
                for enabling ' GC9A01A_FRAME_ENABLE ' ,  for disabling ' GC9A01A_FRAME_DISABLE '
    @param  end_row  
                vertical pixel to end with.
    @return void
    @note   for more details read datasheet of GC9A01A display.
*/
void gc9a10a_frame_show(gc9a01a_frame_modes_t mode)
{
    switch (mode)
    {
    case GC9A01A_FRAME_ENABLE:
        gc9a01a_send_cmd(GC9A01A_DISPON);
        break;
    
    case GC9A01A_FRAME_DISABLE:
        gc9a01a_send_cmd(GC9A01A_DISPON);
        break;
    default:
        ESP_LOGI(TAG, "unknown mode parameter is passed to the function.");
    }
}

/*
    @brief this function put the GC9A01A display in sleep mode.
    @param  mode  
                for sleep ' GC9A01A_SLEEP_ON ' ,  for wakeup ' GC9A01A_SLEEP_OFF '
    @return void
    @note   for more details read datasheet of GC9A01A display.
*/
void gc9a10a_sleep(gc9a01a_sleep_modes_t mode)
{
    switch (mode)
    {
    case GC9A01A_SLEEP_ON:
        gc9a01a_send_cmd(GC9A01A_SLPIN);
        break;
    
    case GC9A01A_SLEEP_OFF:
        gc9a01a_send_cmd(GC9A01A_SLPOUT);
        break;
    default:
        ESP_LOGI(TAG, "unknown mode parameter is passed to the function.");
    }
}


/*
    @brief this function inverse the GC9A01A display data.
    @param  mode  
                for inversion ' GC9A01A_INVERSION_ENABLE ' ,  for normal ' GC9A01A_INVERSION_DISABLE '
    @return void
    @note   for more details read datasheet of GC9A01A display.
*/
void gc9a10a_inversion(gc9a01a_inversion_modes_t mode)
{
    switch (mode)
    {
    case GC9A01A_INVERSION_ENABLE:
        gc9a01a_send_cmd(GC9A01A_INVON);
        break;
    
    case GC9A01A_INVERSION_DISABLE:
        gc9a01a_send_cmd(GC9A01A_INVON);
        break;
    default:
        ESP_LOGI(TAG, "unknown mode parameter is passed to the function.");
    }
}
