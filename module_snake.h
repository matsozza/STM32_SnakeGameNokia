#ifndef MODULE_SNAKE
#define MODULE_SNAKE

void moduleSnake_initGame();
void moduleSnake_runGame();
void IO_sendToLCD();
void snake_initSnakeObj();
void snake_updateSnakePos();
void snake_printSnakeToBoard();
void board_initLayers();
void board_setPixel(uint8_t rowIdx, uint8_t colIdx, uint8_t pixelVal, uint8_t boardLayer);

#endif