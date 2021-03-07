#define LCD_DATA	PORTA 
#define LCD_INST	PORTA 
#define LCD_PIN 	PINA 
#define LCD_CTRL	PORTC 

#define LCD_RS	0
#define LCD_RW	1
#define LCD_EN	2

#define LCD_CLR			0     
#define LCD_HOME		1     
#define LCD_ENTRY_MODE		2     
#define LCD_ENTRY_INC		1     
#define LCD_ENTRY_SHIFT		0     
#define LCD_ON_CTRL		3     
#define LCD_ON_DISPLAY		2     
#define LCD_ON_CURSOR		1     
#define LCD_ON_BLINK		0     
#define LCD_MOVE		4     
#define LCD_MOVE_DISP		3     
#define LCD_MOVE_RIGHT		2     
#define LCD_FUNCTION		5     
#define LCD_FUNCTION_8BIT	4     
#define LCD_FUNCTION_2LINES	3     
#define LCD_FUNCTION_10DOTS	2     
#define LCD_CGRAM		6     
#define LCD_DDRAM		7     
#define LCD_BUSY		7     

#define BV(bit)		(1<<(bit))
#define cbi(reg,bit)	reg &= ~(BV(bit))
#define sbi(reg,bit)	reg |= (BV(bit))

void portOut(void)
{
	DDRA = 0xFF;	
	DDRC = 0x0F;
}

void portIn(void)
{
	DDRA = 0x00;	
	DDRC = 0x0F;
}

void writeCTRL(unsigned char ch)
{    
	cbi(LCD_CTRL, LCD_RS);
	cbi(LCD_CTRL, LCD_RW);
	sbi(LCD_CTRL, LCD_EN);
	_delay_us(50);        
	LCD_INST=ch;
	_delay_us(50);
	cbi(LCD_CTRL, LCD_EN); 
}

void writeChar(unsigned char ch) 
{
	sbi(LCD_CTRL, LCD_RS);      
	cbi(LCD_CTRL, LCD_RW);
	sbi(LCD_CTRL, LCD_EN);
	_delay_us(50);        
	LCD_DATA = ch; 
	_delay_us(50); 
	cbi(LCD_CTRL, LCD_EN); 
}

unsigned char readBF_AC(void) {
	unsigned char rlt; 

	cbi(LCD_CTRL, LCD_RS);   
	sbi(LCD_CTRL, LCD_RW);
	sbi(LCD_CTRL, LCD_EN); 
	_delay_us(50);         
	rlt = LCD_PIN;                        
	_delay_us(50);         
	cbi(LCD_CTRL, LCD_EN); 

	return rlt;
}

unsigned char readChar(void) {
	unsigned char rlt; 

	sbi(LCD_CTRL, LCD_RS);   
	sbi(LCD_CTRL, LCD_RW);
	sbi(LCD_CTRL, LCD_EN);      
	_delay_us(50);              
	rlt = LCD_PIN;
	_delay_us(50);
	cbi(LCD_CTRL, LCD_EN); 

	return rlt;
}

void writeSTR(unsigned char *str)
{
	while(*str != '\0')
	{
		writeChar(*str);
		//_delay_ms(1);
		str++;
	}
}

void setPosition(unsigned char row, unsigned char col) 
{
	unsigned char addr;

	switch(row)
	{
		case 0: addr = 0x00 + col; break;
		case 1: addr = 0x40 + col; break;
		default: addr = 0x00 + col;
	}

	writeCTRL(1<<LCD_DDRAM | addr);
}

void clearLCD(void)
{
	writeCTRL(1<<LCD_CLR);
	_delay_ms(2);
}

void returnHome(void)
{
	writeCTRL(1<<LCD_HOME);
	_delay_ms(2);
}

void blinkCursor(void)
{
	writeCTRL(1<<LCD_ON_DISPLAY | 1<<LCD_ON_BLINK);
	_delay_ms(2);
}

void displayShiftLeft(void){
	writeCTRL(1<<LCD_MOVE | 1<<LCD_MOVE_DISP);
	_delay_ms(2);
}

void displayShiftRight(void){
	writeCTRL(1<<LCD_MOVE | 1<<LCD_MOVE_DISP | 1<<LCD_MOVE_RIGHT);
	_delay_ms(2);
}

void cursorShiftLeft(void){
	writeCTRL(1<<LCD_MOVE);
	_delay_ms(2);
}

void cursorShiftRight(void){
	writeCTRL(1<<LCD_MOVE | 1<<LCD_MOVE_RIGHT);
	_delay_ms(2);
}

void cursorReward(void) {
	writeCTRL(1<<LCD_ENTRY_MODE); 
	_delay_ms(2);
}

void cursorForward(void) {
	writeCTRL(1<<LCD_ENTRY_MODE | 1<<LCD_ENTRY_INC);
	_delay_ms(2);
}

void entryShift(void) {
	writeCTRL(1<<LCD_ENTRY_MODE | 1<<LCD_ENTRY_INC | 1<<LCD_ENTRY_SHIFT); 
	_delay_ms(2);
}

void waitBusy(void){
	portIn();
	while ((readBF_AC() & (1<<LCD_BUSY)) !=0);
	portOut();
}

void setCGRAM_ADDR(void) {
	writeCTRL(1<<LCD_CGRAM);
	_delay_ms(2);
}

void setDDRAM_ADDR(void) {
	writeCTRL(1<<LCD_DDRAM);
	_delay_ms(2);
}

void set10DOTS(void) {
	writeCTRL(1<<LCD_FUNCTION | 1<<LCD_FUNCTION_8BIT | 0<<LCD_FUNCTION_2LINES | 1<<LCD_FUNCTION_10DOTS);	
	_delay_ms(2);
}

void LCDInit(void)
{
	writeCTRL(1<<LCD_FUNCTION | 1<<LCD_FUNCTION_8BIT | 1<<LCD_FUNCTION_2LINES  | 0<<LCD_FUNCTION_10DOTS);	
	_delay_ms(2);
	writeCTRL(1<<LCD_ON_CTRL | 1<<LCD_ON_DISPLAY | 0<<LCD_ON_CURSOR );	
	_delay_ms(2);		   
	clearLCD();		
	_delay_ms(2);
	writeCTRL(1<<LCD_ENTRY_MODE | 1<<LCD_ENTRY_INC); 
	_delay_ms(2);
}

