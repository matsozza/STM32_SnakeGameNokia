#ifndef MODULE_SNAKE
#define MODULE_SNAKE

void module_initGame();
void module_runGame();
void IO_sendToLCD();
void snake_initSnakeObj();
void snake_updateSnakePos();
void snake_printSnakeToBoard();
void board_initBGLayer();
void board_setPixel(uint8_t rowIdx, uint8_t colIdx, uint8_t pixelVal, uint8_t boardLayer);

#endif