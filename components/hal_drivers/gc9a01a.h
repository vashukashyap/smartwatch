#pragma once
#include <stdio.h>
#include <stdint.h>



// -------SPI CONFIGRATION MACROS---------
#define GPIO_CS             4
#define GPIO_DC             2
#define GPIO_MOSI           23
#define GPIO_SCLK           18
#define SENDER_HOST         SPI3_HOST
#define SPI_DEFAULT_FREQ    40000000


// -------DSIPLAY CONFIGRATION MACROS---------

// ----DIMENSIONS OF DISPLAY 
#define GC9A01A_TFTWIDTH 240 ///< Display width in pixels
#define GC9A01A_TFTHEIGHT 240 ///< Display height in pixels


// ------ DISPLAY COMMANDS
#define GC9A01A_SWRESET 0x01   ///< Software Reset (maybe, not documented)
#define GC9A01A_RDDID 0x04     ///< Read display identification information
#define GC9A01A_RDDST 0x09     ///< Read Display Status
#define GC9A01A_SLPIN 0x10     ///< Enter Sleep Mode
#define GC9A01A_SLPOUT 0x11    ///< Sleep Out
#define GC9A01A_PTLON 0x12     ///< Partial Mode ON
#define GC9A01A_NORON 0x13     ///< Normal Display Mode ON
#define GC9A01A_INVOFF 0x20    ///< Display Inversion OFF
#define GC9A01A_INVON 0x21     ///< Display Inversion ON
#define GC9A01A_DISPOFF 0x28   ///< Display OFF
#define GC9A01A_DISPON 0x29    ///< Display ON
#define GC9A01A_CASET 0x2A     ///< Column Address Set
#define GC9A01A_RASET 0x2B     ///< Row Address Set
#define GC9A01A_RAMWR 0x2C     ///< Memory Write
#define GC9A01A_PTLAR 0x30     ///< Partial Area
#define GC9A01A_VSCRDEF 0x33   ///< Vertical Scrolling Definition
#define GC9A01A_TEOFF 0x34     ///< Tearing Effect Line OFF
#define GC9A01A_TEON 0x35      ///< Tearing Effect Line ON
#define GC9A01A_MADCTL 0x36    ///< Memory Access Control
#define GC9A01A_VSCRSADD 0x37  ///< Vertical Scrolling Start Address
#define GC9A01A_IDLEOFF 0x38   ///< Idle mode OFF
#define GC9A01A_IDLEON 0x39    ///< Idle mode ON
#define GC9A01A_COLMOD 0x3A    ///< Pixel Format Set
#define GC9A01A_CONTINUE 0x3C  ///< Write Memory Continue
#define GC9A01A_TEARSET 0x44   ///< Set Tear Scanline
#define GC9A01A_GETLINE 0x45   ///< Get Scanline
#define GC9A01A_SETBRIGHT 0x51 ///< Write Display Brightness
#define GC9A01A_SETCTRL 0x53   ///< Write CTRL Display
#define GC9A01A1_POWER7 0xA7   ///< Power Control 7
#define GC9A01A_TEWC 0xBA      ///< Tearing effect width control
#define GC9A01A1_POWER1 0xC1   ///< Power Control 1
#define GC9A01A1_POWER2 0xC3   ///< Power Control 2
#define GC9A01A1_POWER3 0xC4   ///< Power Control 3
#define GC9A01A1_POWER4 0xC9   ///< Power Control 4
#define GC9A01A_RDID1 0xDA     ///< Read ID 1
#define GC9A01A_RDID2 0xDB     ///< Read ID 2
#define GC9A01A_RDID3 0xDC     ///< Read ID 3
#define GC9A01A_FRAMERATE 0xE8 ///< Frame rate control
#define GC9A01A_SPI2DATA 0xE9  ///< SPI 2DATA control
#define GC9A01A_INREGEN2 0xEF  ///< Inter register enable 2
#define GC9A01A_GAMMA1 0xF0    ///< Set gamma 1
#define GC9A01A_GAMMA2 0xF1    ///< Set gamma 2
#define GC9A01A_GAMMA3 0xF2    ///< Set gamma 3
#define GC9A01A_GAMMA4 0xF3    ///< Set gamma 4
#define GC9A01A_IFACE 0xF6     ///< Interface control
#define GC9A01A_INREGEN1 0xFE  ///< Inter register enable 1

// ----- DISPLAY ORIENTATION AND COLOR CONFIGURATIONS
#define MADCTL_MY 0x80  ///< Bottom to top
#define MADCTL_MX 0x40  ///< Right to left
#define MADCTL_MV 0x20  ///< Reverse Mode
#define MADCTL_ML 0x10  ///< LCD refresh Bottom to top
#define MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define MADCTL_MH 0x04  ///< LCD refresh right to left

// ----- COLOR DEFINATIONS
#define GC9A01A_BLACK 0x0000ULL       ///<   0,   0,   0
#define GC9A01A_NAVY 0x000F        ///<   0,   0, 123
#define GC9A01A_DARKGREEN 0x03E0   ///<   0, 125,   0
#define GC9A01A_DARKCYAN 0x03EF    ///<   0, 125, 123
#define GC9A01A_MAROON 0x7800      ///< 123,   0,   0
#define GC9A01A_PURPLE 0x780F      ///< 123,   0, 123
#define GC9A01A_OLIVE 0x7BE0       ///< 123, 125,   0
#define GC9A01A_LIGHTGREY 0xC618   ///< 198, 195, 198
#define GC9A01A_DARKGREY 0x7BEF    ///< 123, 125, 123
#define GC9A01A_BLUE 0x001F        ///<   0,   0, 255
#define GC9A01A_GREEN 0x07E0       ///<   0, 255,   0
#define GC9A01A_CYAN 0x07FF        ///<   0, 255, 255
#define GC9A01A_RED 0xF800         ///< 255,   0,   0
#define GC9A01A_MAGENTA 0xF81F     ///< 255,   0, 255
#define GC9A01A_YELLOW 0xFFE0      ///< 255, 255,   0
#define GC9A01A_WHITE 0xFFFF       ///< 255, 255, 255
#define GC9A01A_ORANGE 0xFD20      ///< 255, 165,   0
#define GC9A01A_GREENYELLOW 0xAFE5 ///< 173, 255,  41
#define GC9A01A_PINK 0xFC18        ///< 255, 130, 198

// structure for the display configs

typedef enum
{
    GC9A01A_NORMAL_MODE = GC9A01A_NORON,
    GC9A01A_PARTIAL_MODE = GC9A01A_PTLON
} gc9a01a_modes_t;

typedef enum
{
    GC9A01A_SLEEP_ON = GC9A01A_SLPIN,
    GC9A01A_SLEEP_OFF = GC9A01A_SLPOUT
} gc9a01a_sleep_modes_t;

typedef enum
{
    GC9A01A_FRAME_ENABLE = GC9A01A_DISPON,
    GC9A01A_FRAME_DISABLE = GC9A01A_DISPOFF
} gc9a01a_frame_modes_t;

typedef enum
{
    GC9A01A_INVERSION_ENABLE = GC9A01A_INVON,
    GC9A01A_INVERSION_DISABLE = GC9A01A_INVOFF
} gc9a01a_inversion_modes_t;
 

void gc9a01a_send_v_display_buffer(uint8_t *v_buffer);

void gc9a01a_init_conn();

void gc9a01a_send_cmd(uint8_t cmd);

void gc9a01a_send_data(const uint8_t *data, int len);

void gc9a01a_init();

void gc9a01a_mode(gc9a01a_modes_t mode);

void gc9a01a_draw_cursor_set(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

void gc9a01a_draw_screen_partial(uint16_t start_row, uint16_t end_row);

void gc9a01a_draw_screen_pixel(uint16_t pox_x, uint16_t pos_y, uint16_t color);

void gc9a10a_frame_show(gc9a01a_frame_modes_t mode);

void gc9a10a_inversion(gc9a01a_inversion_modes_t mode);

void gc9a10a_sleep(gc9a01a_sleep_modes_t mode);