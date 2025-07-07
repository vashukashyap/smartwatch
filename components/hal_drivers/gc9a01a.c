#include <stdio.h>
#include <malloc.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <gc9a01a.h>


#ifndef CONFIG_LOG_MAXIMUM_LEVEL
#define CONFIG_LOG_MAXIMUM_LEVEL ESP_LOG_WARN
#endif


static spi_device_handle_t handle;

static const char *TAG = "GC9A01A Driver";





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

    ESP_ERROR_CHECK( spi_bus_initialize(SENDER_HOST, &buscfg, SPI_DMA_CH_AUTO) );
    
    ESP_LOGI(TAG, "master SPI bus is initalized");

    //Configuration for the SPI device on the other side of the bus
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 30000000,
        .mode = 0,
        .spics_io_num = GPIO_CS,      //Keep the CS low 3 cycles after transaction, to stop slave from missing the last bit when CS has less propagation delay than CLK
        .queue_size = 3
    };

    ESP_ERROR_CHECK( spi_bus_add_device(SENDER_HOST, &devcfg, &handle) );

    ESP_LOGI(TAG, "master SPI device is initalized");

    //Configuration for the D/C pin
    gpio_set_direction(GPIO_DC, GPIO_MODE_OUTPUT);


    ESP_LOGI(TAG, "display connection initalized");

}

void gc9a01a_send_cmd(uint8_t cmd)
{
    gpio_set_level(GPIO_DC, 0);                         // turning D/C low for sending command

    spi_transaction_t cmd_t = {
        .length = 8,
        .tx_buffer = &cmd
    };

    ESP_ERROR_CHECK( spi_device_transmit(handle, &cmd_t));
}

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

   
    uint8_t line_buffer[GC9A01A_TFTWIDTH*2];

    for(int i=0; i<GC9A01A_TFTWIDTH; i++)
    {
        line_buffer[i*2] = (GC9A01A_BLACK >> 8) & 0xFF;
        line_buffer[i*2+1] = (GC9A01A_BLACK) & 0xFF;
    }

    gc9a01a_draw_cursor_set(0,240,0,240);

    gc9a01a_send_cmd(0x2C);
    for(int i=0; i<=GC9A01A_TFTHEIGHT; i++)
    {
        gc9a01a_send_data(line_buffer, sizeof(line_buffer));
    }

    ESP_LOGI(TAG, "Display is initalized with vendor commands");
}

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

void gc9a01a_draw_cursor_set(uint16_t start_col, uint16_t end_col, uint16_t start_row, uint16_t end_row)
{

    uint8_t col_paras[4] = { 
        ((start_col >> 8) & 0xFF), (start_col & 0xFF),
        ((end_col >> 8) & 0xFF), (end_col & 0xFF)
    };

    uint8_t row_paras[4] = {
        ((start_row >> 8) & 0xFF), (start_row & 0xFF),
        ((end_row >> 8) & 0xFF), (end_row & 0xFF)
    };

    gc9a01a_send_cmd(0x2A);
    gc9a01a_send_data(col_paras, 4);
    gc9a01a_send_cmd(0x2B);
    gc9a01a_send_data(row_paras, 4);

    
}

void gc9a01a_draw_screen_partial(uint16_t start_row, uint16_t end_row)
{

    uint8_t paras[4] = {
        ((start_row >> 8) & 0xFF), (start_row & 0xFF),
        ((end_row >> 8) & 0xFF), (end_row & 0xFF)
    };

    gc9a01a_send_cmd(0x30);
    gc9a01a_send_data(paras, 4);
    
}

void gc9a10a_display_buffer(uint8_t *buffer, size_t size)
{
    spi_transaction_t buffer_tx = {
        .length = size * 8,
        .tx_buffer = buffer
    };
    
    gc9a01a_send_cmd(GC9A01A_RAMWR);
    gpio_set_level(GPIO_DC, 1);
    ESP_ERROR_CHECK( spi_device_transmit(handle, &buffer_tx));
    printf("BUFFER SENT");
}

void gc9a01a_draw_screen_pixel(uint16_t pox_x, uint16_t pos_y, uint16_t color)
{
    uint8_t col_paras[4] = { 
        ((pox_x >> 8) & 0xFF), (pox_x & 0xFF),
        ((pox_x >> 8) & 0xFF), (pox_x & 0xFF)
    };

    uint8_t row_paras[4] = {
        ((pos_y >> 8) & 0xFF), (pos_y & 0xFF),
        ((pos_y >> 8) & 0xFF), (pos_y & 0xFF)
    };

    gc9a01a_send_cmd(0x2A);
    gc9a01a_send_data(col_paras, 4);
    gc9a01a_send_cmd(0x2B);
    gc9a01a_send_data(row_paras, 4);

    gc9a01a_send_cmd(0x2C);
    
    uint16_t pixel[1] = {(color >> 8) | (color <<8)};
    gc9a10a_display_buffer(pixel, 1);
}


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
