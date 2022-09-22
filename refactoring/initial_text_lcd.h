/********************************************
* TEXT LCD Initial
* Liquid Crystal Dispay - 16x2(5x7 dots)
* PORTA - DR(Data Register)
* PORTC - IR(Instruction Register)
*********************************************/
#define LCD_DATA      PORTA     // LCD ������ ��Ʈ ����
#define LCD_INST      PORTA     // LCD ��ɾ� ��Ʈ ����
#define LCD_PIN 	  PINA      // LCD �Է� ��Ʈ ���� 

#define LCD_CTRL      PORTC     // LCD ������Ʈ ����

#define LCD_RS          0		// Register Select(0=instruction, 1=data) 
#define LCD_RW          1		// Read/Write (0 : write, 1: read)
#define LCD_EN          2  		// Enable Signal for Read/Write LCD

#define LCD_CLR             0      // DB0: clear display
#define LCD_HOME            1      // DB1: return to home position

#define LCD_ENTRY_MODE      2      // DB2: set entry mode
#define LCD_ENTRY_INC       1      // DB1: increment
#define LCD_ENTRY_SHIFT     0      // DB2: shift

#define LCD_ON_CTRL         3      // DB3: turn lcd/cursor on
#define LCD_ON_DISPLAY      2      // DB2: turn display on
#define LCD_ON_CURSOR       1      // DB1: turn cursor on
#define LCD_ON_BLINK        0      // DB0: blinking cursor

#define LCD_MOVE            4      // DB4: move cursor/display
#define LCD_MOVE_DISP       3      // DB3: move display (0-> move cursor)
#define LCD_MOVE_RIGHT      2      // DB2: move right (0-> left)

#define LCD_FUNCTION        5      // DB5: function set
#define LCD_FUNCTION_8BIT   4      // DB4: set 8BIT mode (0->4BIT mode)
#define LCD_FUNCTION_2LINES 3      // DB3: two lines (0->one line)
#define LCD_FUNCTION_10DOTS 2      // DB2: 5x10 font (0->5x7 font)

#define LCD_CGRAM           6      // DB6: set CG RAM address
#define LCD_DDRAM           7      // DB7: set DD RAM address
#define LCD_BUSY            7      // DB7: LCD is busy

#define BV(bit)			(1<<(bit))  	    //��� bit�� 1��Ʈ Shift ��Ű�� ����
#define cbi(reg,bit)	reg &= ~(BV(bit))  //Ư��(reg) bit�� 0���� ����� ��     
#define sbi(reg,bit)	reg |= (BV(bit))   //Ư��(reg) bit�� 1���� ����� ��

void portOut(void)
{
      DDRA = 0xFF;	// PORTA�� �������
      DDRC = 0x0F;	// PORTG�� ���� 4��Ʈ�� ������� //0xFF
}

void portIn(void)
{
      DDRA = 0x00;	// PORTA�� �Է�����
      DDRC = 0x0F;	// PORTG�� ���� 4��Ʈ�� ������� //0xFF
}

void writeCTRL(unsigned char ch)  //LCD IR�� ��ɾ� ���� 
{
	sbi(LCD_CTRL, LCD_EN);  // LCD Enable //portg = 0b00000100;
	cbi(LCD_CTRL, LCD_RW);
	cbi(LCD_CTRL, LCD_RS);  // RS==0���� ��ɾ� ���� ����Ŭ
	_delay_us(50);

	LCD_INST= ch;  //PORTA = 0b00000001;
	_delay_us(50);

	cbi(LCD_CTRL, LCD_EN);  //LCD Disable	 
}

void writeChar(unsigned char ch)  //LCD DR�� ������ ���
{
	sbi(LCD_CTRL, LCD_EN);  //LCD Enable //PORTC = 0b00010;
	cbi(LCD_CTRL, LCD_RW);
	sbi(LCD_CTRL, LCD_RS);  //RS=1, RW=0���� ������ ���� ����Ŭ
	_delay_us(50);

	LCD_DATA = ch;  //������ ���
	_delay_us(50);

	cbi(LCD_CTRL, LCD_EN);
	_delay_ms(1);
}

void writeSTR(unsigned char str[])  //���ڿ� ����Լ� 
{
	unsigned char i = 0;
	
	while(str[i] != 0)  
	{
		writeChar(str[i]);
		i++;
	}

}

/*
// writeSTR Reg2
void writeSTR(unsigned char *str)
{
    while(*str != 0)
	{
		writeChar(*str);
		str++;
    }
}
*/

unsigned char readBF()
{
	unsigned char ch;

	sbi(LCD_CTRL, LCD_EN);  //PORTG = 0b00000110;
	sbi(LCD_CTRL, LCD_RW);
	cbi(LCD_CTRL, LCD_RS);  //LCD Enable
	_delay_us(50);

	ch = LCD_PIN;

	_delay_us(50);

	cbi(LCD_CTRL, LCD_EN);  //LCD Disable

	return ch; 
}

void waitBusy() 
{
	portIn(); 
	while((readBF() & (1<<LCD_BUSY)) !=0);
	portOut();
}


unsigned char readChar()  //�о���̴� �Լ�//�Ű�����//��ɾ �о�´�//DDRAM���� �����а� �Լ� 
{
	unsigned char ch;

	sbi(LCD_CTRL, LCD_EN);  //PORTG = 0b00000110;
	sbi(LCD_CTRL, LCD_RW);
	sbi(LCD_CTRL, LCD_RS);  //LCD Enable
	_delay_us(50);

	ch = LCD_PIN;
	_delay_us(50);

	cbi(LCD_CTRL, LCD_EN);  //LCD Disable

	return ch; 
}

// * Clear Display Reg
void LCDClear()  
{ 
	writeCTRL(1<<LCD_CLR);
	_delay_us(2);
}

void returnHome()
{
	writeCTRL(1<<LCD_HOME);
	_delay_us(2);
}

// * Shift Reg
void dispShiftRight()
{
	writeCTRL((1<<LCD_MOVE) | (1<<LCD_MOVE_DISP) | (1<<LCD_MOVE_RIGHT));
	_delay_ms(2);
}

void dispShiftLeft()
{
	writeCTRL((1<<LCD_MOVE) | (1<<LCD_MOVE_DISP) | (0<<LCD_MOVE_RIGHT));
	_delay_ms(2);
}

void cursorShiftRight()  //Ŀ���� ���������� 1���̵�
{
	writeCTRL((1<<LCD_MOVE) | (0<<LCD_MOVE_DISP) | (1<<LCD_MOVE_RIGHT));
	_delay_ms(2);
}

void cursorShiftLeft()  //Ŀ���� �������� 1���̵� 
{
	writeCTRL((1<<LCD_MOVE) | (0<<LCD_MOVE_DISP) | (0<<LCD_MOVE_RIGHT)); //�������� ���÷��̰� �ƴ� Ŀ���� ������ �ƴ� ���������� 
	_delay_ms(2);
}

void entryShift()   
{
	writeCTRL((1<<LCD_ENTRY_MODE) | (1<<LCD_ENTRY_INC) | (1<<LCD_ENTRY_SHIFT));
	_delay_us(2);
}

void entryReward()  //������ 
{
	writeCTRL((1<<LCD_ENTRY_MODE) | (0<<LCD_ENTRY_INC) | (0<<LCD_ENTRY_SHIFT));
	_delay_us(2);
}

void entryForward()  //������ 
{
	writeCTRL((1<<LCD_ENTRY_MODE) | (1<<LCD_ENTRY_INC) | (0<<LCD_ENTRY_SHIFT));
	_delay_us(2);
}

// * Set DDRAM address Reg
void setPosition(unsigned char row, unsigned char col)  //LCD ������ ����//275p
{
	unsigned char addr; 
		
	if(row == 0)  //row = ������
		addr = col;
	else if(row ==1)  
		addr = 0x40 + col; //col = ���ڿ�
	else 
		addr = 0;

	writeCTRL((1<<LCD_DDRAM) | addr);
	_delay_ms(2);
}

/*
// * Set DDRAM address Reg2
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
*/

void setCGRAM_ADDR()
{
	writeCTRL(1<<LCD_CGRAM);
	_delay_ms(2);

}
void setDDRAM_ADDR()
{
	writeCTRL(1<<LCD_DDRAM);
	_delay_ms(2);
}

void LCD10DOTS()
{
	writeCTRL((1<<LCD_FUNCTION) | (1<<LCD_FUNCTION_8BIT) | (0<<LCD_FUNCTION_2LINES) | (1<<LCD_FUNCTION_10DOTS));

	_delay_ms(2);
}


void Init_LCD()
{
	writeCTRL((1<<LCD_FUNCTION) | (1<<LCD_FUNCTION_8BIT) | (1<<LCD_FUNCTION_2LINES) | (0<<LCD_FUNCTION_10DOTS));
	_delay_ms(2); // FUNCTION SET
	writeCTRL((1<<LCD_ON_CTRL) | (1<<LCD_ON_DISPLAY) | (0<<LCD_ON_CURSOR) | (0<<LCD_ON_BLINK));
	_delay_ms(2); // DISPLAY ON/OFF CONTROL
	writeCTRL(1<<LCD_CLR);
	_delay_ms(2); // CLEAR DISPLAY
	writeCTRL((1<<LCD_ENTRY_MODE) | (1<<LCD_ENTRY_INC) | (0<<LCD_ENTRY_SHIFT));
	_delay_ms(2); // ENTRY MODE SET
}
 
