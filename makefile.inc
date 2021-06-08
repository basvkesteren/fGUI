# fGUI code
FGUI_SRC     += fgui/fgui.c fgui/fgui_clearregion.c fgui/fgui_helper.c fgui/fgui_line.c fgui/fgui_sprite.c fgui/fgui_text.c fgui/fgui_extra.c

# Display driver
ifeq ($(FGUI_DISPLAY),ks0108)
FGUI_SRC     += fgui/hw/ks0108.c
FGUI_DEFINES = FGUI_DISPLAY_KS0108
endif

ifeq ($(FGUI_DISPLAY),ssd1322)
FGUI_SRC     += fgui/hw/ssd1322.c
FGUI_DEFINES = FGUI_DISPLAY_SSD1322
endif

ifeq ($(FGUI_DISPLAY),uart_display)
FGUI_SRC     += fgui/hw/uart_display.c
FGUI_DEFINES = FGUI_DISPLAY_UART
endif

ifeq ($(FGUI_DISPLAY),ws2812)
FGUI_SRC     += fgui/hw/ws2812.c
FGUI_DEFINES = FGUI_DISPLAY_WS2812
endif
