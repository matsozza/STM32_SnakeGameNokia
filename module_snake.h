#ifndef MODULE_SNAKE
#define MODULE_SNAKE

void _moduleSnake_initGame();
void _moduleSnake_runGame();
void _IO_sendToLCD();
void _snake_initSnakeObj();
void _snake_updateSnakePos();
void _snake_printSnakeToBoard();
void _board_initLayers();
void _board_setPixel(uint8_t rowIdx, uint8_t colIdx, uint8_t pixelVal, uint8_t boardLayer);

#endif