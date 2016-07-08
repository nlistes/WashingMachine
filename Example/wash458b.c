#define _XTAL_FREQ  64000000
#include <pic18.h>	// Include header file for MCU
#include <1wire887.h>
#include <1wire887-2.h>
#include <stdio.h>
//#include <stdlib.h>
#include <lcd18_40x4buf.h>
#include <encoderwash.h>

/*
#pragma config FOSC = INTIO67, FCMEN = OFF,IESO = OFF,PLLCFG = ON
#pragma config PWRTEN = ON, WDTEN = OFF,BOREN = OFF
#pragma config MCLRE = INTMCLR, HFOFST = ON
#pragma config LVP = OFF
#pragma config CP0=OFF, CP1=OFF,CP2=OFF, CP3=OFF, CPB=OFF, CPD=OFF
#pragma config WRTC=OFF, WRTB=OFF,WRTD=OFF
#pragma config EBTR0=OFF , EBTR1=OFF , EBTR2=OFF , EBTR3=OFF , EBTRB=OFF
*/


unsigned long sys1=0, SysOld = 0, dispDel=0, SDel = 0, PWashT = 15, PWashT1 = 0, FDel = 0, CWDel = 0,
	RDelWF = 0, time1=0, CountOsc = 0, RDel = 0,RDelW = 0,
	RDelW1 = 0,WaitButtTime = 0, count = 0,freq = 0, DrumRpm = 0, SMT = 0, TWring = 0,TUpRpm1 = 0, TUpRpm = 0,
 RSDel = 3000,WaitButt1 = 0;

unsigned char encoder2roll=0, encoder2ButPrev = 1, encoder2pressed = 0, bat1press = 0, bat1pressprev = 1,
	WaitPress = 0, WaitPress1 = 0, WaitPressD = 0, WaitPD = 0, bat2press = 0, bat2pressprev = 1, WOk = 1;

int   RelayStartOsc = 0, Rotate = 0, PrevRotate =0, RDel1=0, StartTemp = 0, TP = 0,p = 0, veskg = 0,TimeVes = 500,
	TPol = 2, TempPol = 10, TP1 = 0, QR = 4,  TempFan = 0, TempWaterOut = 0,press=0,AZ=0,AZRD6 = 20,AZCCP = 30,
 relayStart = 0,I = 0, AdaptCCP = 0, TimeOldVes = 0;

unsigned int IntRelay = 0, CCPRPM = 0, SoundOn = 1, menuId = 0,menuIdOld = 0,
	TempWaterIn = 30, RelayStartWash = 0, CCP = 0, RpmDrumDsp = 1100, IPress = 10, Beetw = 0,Beetw1 = 0, CCPRPM1 = 0,
	BitStart = 0, id = 0, count1 = 0, Adapt = 4,TimeDisb = 1000;

char v[255];
bit off1 = 0, ControlStart = 0, MStop = 0, StartBeep = 0, ClOp = 0, CCPOff = 0, AdaptBit = 0, TempOk = 0;
long ITimePol = 0;


unsigned char rpm[] = 0x6F, 0xB2, 0x2F, 0xBC,0;
//unsigned char hello[] = 0xA8, 0x70, 0xB8, 0xB3, 0x65, 0xBF, 0x20, 0xC7, 0x20, 0x63, 0xBF, 0xB8, 0x70, 0x61, 0xBB, 0xC4, 0xBD, 0x61, 0xC7, 0x20, 0xBC, 0x61, 0xC1, 0xB8, 0xBD, 0xBA, 0x61, 0x2E, 0;
unsigned char hlopok[] = 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2A, 0x2A, 0x20, 0x58, 0xA7, 0x4F, 0xA8, 0x4F, 0x4B, 0x20, 0x2A, 0x2A, 0;
unsigned char TempWat[] = 0x54, 0x65, 0xBC, 0xBE, 0x65, 0x70, 0x61, 0xBF, 0x79, 0x70, 0x61, 0x20, 0xB3, 0x6F, 0xE3, 0xC3, 0x3A,0;
unsigned char TempMot[] = 0x54, 0x65, 0xBC, 0xBE, 0x2E, 0x20, 0xBC, 0x6F, 0xBF, 0x6F, 0x70, 0x61, 0x3A,0;
unsigned char ElapseTime[] = 0x4F, 0x63, 0xBF, 0x61, 0xB3, 0xC1, 0x65, 0x65, 0x63, 0xC7, 0x20, 0xB3, 0x70, 0x65, 0xBC, 0xC7, 0x3A,0;
unsigned char Plums[] = 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2A, 0x2A, 0x20, 0x43, 0xA7, 0xA5, 0x42, 0x20, 0x2A, 0x2A,0; //Слив
unsigned char PreWashTime[] = 0x42, 0x70, 0x65, 0xBC, 0xC7, 0x20, 0xBE, 0x70, 0x65, 0xE3, 0x20, 0x63, 0xBF, 0xB8, 0x70, 0xBA, 0xB8, 0x3A,0;
unsigned char WashTime[] = 0x43, 0xBF, 0xB8, 0x70, 0xBA, 0xB8, 0x3A,0;
unsigned char Wringing[] = 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2A, 0x2A, 0x20, 0x4F, 0xBF, 0xB6, 0xB8, 0xBC, 0x20, 0x2A, 0x2A,0; //Отжим
unsigned char WringingDsp[] = 0x4F, 0xBF, 0xB6, 0xB8, 0xBC, 0x3A, 0;
unsigned char Berejnaja[] = 0x20,0x20,0x20,0x20,0x20,0x20, 0x20, 0x20, 0x20, 0x2A, 0x2A, 0x20, 0xA0, 0x45, 0x50, 0x45, 0xA3, 0x48, 0x41, 0xB1, 0x20, 0x43, 0x54, 0xA5, 0x50, 0x4B, 0x41, 0x20, 0x20, 0x2A, 0x2A,0;
//unsigned char SpeedWash[] =0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2A, 0x2A, 0x20, 0xA0, 0xAE, 0x43, 0x54, 0x50, 0x41, 0xB1, 0x20, 0x43, 0x54, 0xA5, 0x50, 0x4B, 0x41, 0x20, 0x2A, 0x2A,0;
unsigned char Rinsing[] = 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2A, 0x2A, 0x20, 0xA8, 0x6F, 0xBB, 0x6F, 0x63, 0xBA, 0x61, 0xBD, 0xB8, 0x65, 0x20, 0x2A, 0x2A,0;
unsigned char RpmDrum[] = 0x4F, 0xB2, 0x6F, 0x70, 0x6F, 0xBF, 0xC3, 0x20, 0xB2, 0x61, 0x70, 0x61, 0xB2, 0x61, 0xBD, 0x61, 0x3A,0;
unsigned char QRing[] = 0xA8, 0x70, 0x6F, 0xB9, 0xE3, 0x65, 0xBD, 0x6F, 0x20, 0xBE, 0x6F, 0xBB, 0x6F, 0x63, 0xBA, 0x61, 0xBD, 0xB8, 0xB9, 0x3A, 0;
unsigned char QRing1[] = 0x4B, 0x6F, 0xBB, 0xB8, 0xC0, 0x65, 0x63, 0xBF, 0xB3, 0x6F, 0x20, 0xBE, 0x6F, 0xBB, 0x6F, 0x63, 0xBA, 0x61, 0xBD, 0xB8, 0xB9, 0x3A,0;
unsigned char TimePol[] = 0x42, 0x70, 0x65, 0xBC, 0xC7, 0x20, 0xBE, 0x6F, 0xBB, 0x6F, 0x63, 0xBA, 0x61, 0xBD, 0xB8, 0xC7, 0x20, 0xB7, 0x61, 0x20, 0xE5, 0xB8, 0xBA, 0xBB, 0x3A,0;
unsigned char MotorRpm[] = 0x4D, 0x6F, 0xBF, 0x6F, 0x70, 0x61, 0x3A,0;
unsigned char Millilitres[] = 0xBC, 0xBB, 0;
unsigned char Heater[] = 0x21, 0x48, 0x41, 0xA1, 0x50, 0x45, 0x42, 0x21, 0;
unsigned char Ves[] = 0x42, 0x65, 0x63, 0x20, 0xB2, 0x65, 0xBB, 0xC4, 0xC7, 0x3A,0;
unsigned char Gr[] = 0xB4, 0x70, 0;
unsigned char End[] = 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x20, 0xA8, 0xA5, 0xA4, 0xE0, 0x4F, 0x43, 0x20, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0;
unsigned char Global[] = 0x2A, 0x2A, 0x2A, 0x20, 0xA1, 0xA7, 0x4F, 0xA0, 0x41, 0xA7, 0x62, 0x48, 0xAE, 0x45, 0x20, 0x48, 0x41, 0x43, 0x54, 0x50, 0x4F, 0xA6, 0x4B, 0xA5, 0x2A, 0x2A, 0x2A,0;
unsigned char Pause[] = 0x2A, 0x2A, 0x2A, 0x2A, 0x20, 0xA8, 0x41, 0xA9, 0xA4, 0x41, 0x20, 0x2A, 0x2A, 0x2A, 0x2A,0;
unsigned char Door[] = 0xA7, 0xB0, 0x4B, 0x20, 0xA4, 0x41, 0x4B, 0x50, 0xAE, 0x54, 0;
unsigned char OpDoor[] = 0x2A, 0x2A, 0x2A, 0x20, 0x21, 0x21, 0xA4, 0x41, 0x4B, 0x50, 0x4F, 0xA6, 0x20, 0xA7, 0xB0, 0x4B, 0x20, 0x54, 0x42, 0x4F, 0xB0, 0x20, 0x4D, 0x41, 0x54, 0x62, 0x21, 0x21, 0x20, 0x2A, 0x2A, 0x2A, 0;
unsigned char OpenDoor[] = 0xA7, 0xB0, 0x4B, 0x20, 0x4F, 0x54, 0x4B, 0x50, 0xAE, 0x54, 0;
unsigned char PreWring[] = 0x2A, 0x2A, 0xA8, 0x50, 0x45, 0xE0, 0x4F, 0x54, 0xA3, 0xA5, 0x4D, 0x2A, 0x2A,0;



/*
20 имп на 100мл.  литры = (Импульс * 5) / 1000
1 имп 5 мл.

Режим стирки

48 об мин крайняя скорость

Обороты вала к барабану 1 к 12


RE0 - Насос.
RE1 - Клапан 2. I лоток
RE2 - Клапан 1 II лоток.

RD0 - Термометр.
RD1-8 - Display.

RC0 - Звук.
RC1 - Термометр воды
RC2 - ШИМ.
RC3 - Датчик удара.
RC4 - Реверс, реле.
RC5 - Кнопка энкодера.
RC6 - RX.
RC7 - TX.

RB0 - Тахометр.
RB1 - Расходомер.
RB2 - Кнопка пуск.
RB3 - Вентилятор.
RB4 - ТЭН.
RB5 - концевик.
RB6 - Кнопка стоп.
RB7 - Энкодер.

RA0 - Датчик давления.
RA1 - Энкодер правый.
Ra2 - Нагреватель дверцы замка.
RA7 - Соленоид.
RA5 - Память.
*/





void Delay_ms(unsigned int tick)
{
	while(tick--)	// Loop counter delay time
	{
		__delay_ms(1);	// Delay 1 ms
	}
}


void Delay_sound(unsigned int tick)	// Delay 100 us per tick
{
	while(tick--)
	{
		__delay_us(90);    // Delay 90 us
	}
}

void Sound(unsigned int freq,unsigned int time)
{
	if (SoundOn == 1)
	{
		unsigned int dt=0,m=0;	// Keep value and
		TRISC0 = 0;    // Set RC0 output mode
		dt = 5000/freq;   	// Keep active logic delay
		time = (5*time)/dt;	// Keep counter for generate sound
		for(m=0;m<time;m++) // Loop for generate sound(Toggle logic P0.12)
		{
			LATC0 = 1;    // RC0=1
			Delay_sound(dt);	// Delay for sound
			LATC0 = 0;    // RC0=0
			Delay_sound(dt);	// Delay for sound
		}
	}
}



unsigned long RF = 0;
void RegFan()
{
	if (RF == 0) RF = sys1;
	if (RF!=0 && sys1 - RF > 500)
	{
		if (TempFan >= 50 && PORTBbits.RB3 == 0) LATB3 = 1;
		if (TempFan <= 45 && PORTBbits.RB3 == 1) LATB3 = 0;
		RF = 0;
	}
}


void CheckMotor()
{
	if (Rotate == PrevRotate && SMT==0) SMT=sys1;
	if (Rotate !=PrevRotate && SMT!=0)  SMT = 0;

	PrevRotate = Rotate;

	MStop = (SMT!=0 && sys1-SMT>500);
}
int IDoor = 0;






int pressure()
{
	CHS0 = 0;
	CHS1 = 0;
	CHS2 = 0;
	CHS3 = 0;
	CHS4 = 0;
	ADON = 1;
	GODONE = 1;		// Start conversion
	while(GODONE);	// Wait until conversion success
	return (ADRESH<<2)+(ADRESL>>6);
}
/*
int IAX() //RA1
{
	CHS0 = 1;
	CHS1 = 0;
	CHS2 = 0;
	CHS3 = 0;
	CHS4 = 0;
	ADON = 1;
	GODONE = 1;		// Start conversion
	while(GODONE);	// Wait until conversion success
	return (ADRESH<<2)+(ADRESL>>6);
}
*/
int IAZ() //RA3
{
	CHS0 = 1;
	CHS1 = 1;
	CHS2 = 0;
	CHS3 = 0;
	CHS4 = 0;
	ADON = 1;
	GODONE = 1;		// Start conversion
	while(GODONE);	// Wait until conversion success
	return (ADRESH<<2)+(ADRESL>>6);
}

unsigned int TempWater(void)
{
	unsigned char temp_buff[2];	// Keep raw temp.
	char err_status;	// Keep respond status
	err_status = onewire_reset();	// Reset onewire bus
	if(err_status==0)   	// Check connection to DS1820
	{
		onewire_write_byte(0xCC); 	// Skip ROM
		onewire_write_byte(0x44);  	// Start Conversion
		//	__delay_us(120);	// Delay 120 us for state
		onewire_reset();	// Restart
		onewire_write_byte(0xCC); 	// Skip ROM
		onewire_write_byte(0xBE); 	// Read Scratch Pad

		temp_buff[0] = onewire_read_byte(); // Keep raw data
		temp_buff[1] = onewire_read_byte(); // Keep raw data
	}
	return((temp_buff[1]<<8)+temp_buff[0]);	// Return raw data
}

unsigned int TempMoto(void)
{
	unsigned char temp_buff2[2];	// Keep raw temp.
	char err_status2;	// Keep respond status
	err_status2 = onewire_reset2();	// Reset onewire bus
	if(err_status2==0)   	// Check connection to DS1820
	{
		onewire_write_byte2(0xCC); 	// Skip ROM
		onewire_write_byte2(0x44);  	// Start Conversion
		//	__delay_us(120);	// Delay 120 us for state
		onewire_reset2();	// Restart
		onewire_write_byte2(0xCC); 	// Skip ROM
		onewire_write_byte2(0xBE); 	// Read Scratch Pad

		temp_buff2[0] = onewire_read_byte2(); // Keep raw data
		temp_buff2[1] = onewire_read_byte2(); // Keep raw data
	}
	return((temp_buff2[1]<<8)+temp_buff2[0]);	// Return raw data
}


void initDisplay()
{
	if (menuId == 0) //индикация главного окна.
	{

		lcd_clear();
		lcd_puts(1,128,hlopok);
		lcd_puts(1,192,TempWat);
		lcd_puts(1,215,TempMot);

		lcd_puts(2,128,ElapseTime);
		lcd_puts(2,192,Ves);


	}
	else if (menuId == 1 )
	{
		lcd_clear();
		lcd_puts(1,128,Berejnaja);
		lcd_puts(2,192,Ves);
		lcd_puts(2,128,ElapseTime);
		//lcd_puts(1,192,CurMode);
		//lcd_puts(1,217,MotorRpm);
		lcd_puts(1,192,TempWat);
		lcd_puts(1,215,TempMot);

	}
	else if (menuId == 4)
	{
		lcd_clear();
		lcd_puts(1,128,Plums); //Слив
		lcd_puts(1,192,TempWat);
		////lcd_puts1(128,ElapseTime);
	}
	else if (menuId == 5) //Отжим
	{
		lcd_clear();
		lcd_puts(1,128,Wringing);
		lcd_puts(1,192,TempMot);
		lcd_puts(2,192,RpmDrum);
		lcd_puts(2,217,MotorRpm);
		lcd_puts(2,128,Ves);
	}
	else if (menuId == 51) //Отжим
	{
		lcd_clear();
		lcd_puts(1,143,PreWring);
		lcd_puts(1,192,TempMot);
		lcd_puts(2,192,RpmDrum);
		lcd_puts(2,217,MotorRpm);
		lcd_puts(2,128,Ves);
	}
	else if (menuId == 6) // полоскание
	{
		lcd_clear();
		lcd_puts(1,128,Rinsing);
		lcd_puts(1,192,TempWat);
		lcd_puts(2,128,ElapseTime);
		lcd_puts(2,192,QRing);
		lcd_puts(1,215,TempMot);
		//lcd_puts(1,215,Ves);
	}
	else if (menuId == 7 ) // Конец стирки
	{
		lcd_clear();
		////lcd_puts();
		sprintf(v,End);
		lcd_puts(1,140,v);
	}
	else if (menuId == 8 ) //Пауза
	{
		lcd_clear();
		lcd_puts(1,136,Pause);
	}
	else if (menuId == 9 )
	{
		lcd_clear();
		lcd_puts(2,134,Global);
	}
	/*
	else if (menuId == 10)
	{

	}


	else if (menuId == 11)
	{

	}
	*/
	else if (menuId == 1001 ) //Установки стирки
	{
		lcd_clear();
		lcd_puts(1,128,TempWat);
		lcd_puts(1,192,PreWashTime);
		lcd_puts(1,217,WashTime);
		lcd_puts(2,128,WringingDsp);
		sprintf(v,"%d%s",TempWaterIn,"C");
		lcd_puts(1,145,v);
	}
	else if (menuId == 601)
	{
		lcd_clear();                       //Установки полоскания, количество полосканий
		lcd_puts(1,128,QRing1);
		lcd_puts(1,192,TimePol);
		lcd_puts(2,128,TempWat);
	}
}

void AllOff()
{
	CCPR1L=0;
	Delay_ms(500);
	LATB5 = 0;
	PORTB = 0b00000000;
	PORTE = 0b000;
	PORTD = 0b00000000;

}


void CloseDoor() //ClOp = 0 Открыть двер. 1 закр
{
	while (PORTBbits.RB5 != ClOp )  //RB5 - концевик.
	{
		IDoor++;
		if (PORTBbits.RB5 == 1) IDoor = 0;
		LATA7 = 1; //RA7 - Электромагнит дверцы
		Delay_ms(60);
		LATA7 = 0;
		Delay_ms(200);
		menuId = menuIdOld;
		initDisplay();
		if (IDoor >= 4)
		{
			BitStart = 3;
			ClOp = 0;
			IDoor = 0;
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Delay_ms(500);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			Sound(2000,10);
			Sound(3000,10);
			menuId=25;
			lcd_clear();
			sprintf(v,"Id:%d",menuId);
			lcd_puts(1,130,OpDoor);
			lcd_puts(2,128,v);
			BitStart = 3;
			break;
		}
	}
	//RA7 - Электромагнит дверцы
	//RB5 концевик
	//RA2 Нагреватель концевика люка
}



//------------------------------------------------------------Таймер для стирок----------------------
bit DelayWash(unsigned long TimeWash, signed int RDel1, int RInt) // В минутах
{
	if (RDel1 == RInt && RDel == 0)
		RDel = sys1;
	if (RDel1 == RInt && RDel!=0 && sys1 - RDel > TimeWash) //3 600 000 1 hour // Стирка
	{
		return 1;
	}
	return 0;
}
//---------------------------------------------------------Заливка воды---------------------------------

bit FillWater(int Level, int PreW) // Level = 1 или 2  // PREW Залив воды I или II отделение.
{
	IntRelay = 1;
	if (press < 1 && Level == 1) // Проверка по II уровню.
	{
		if (PreW == 0) LATE2 = 1;
		if (PreW == 1) LATE1 = 1;
	}

	if (press >= 7 && Level == 1)
	{
		LATE2 = 0; //Откл клапан
		LATE1 = 0;
		off1=0;
		return 1;
	}


	if (press < 1 && Level == 2) //Проверка по III уровню.
	{
		if (PreW == 0) LATE2 = 1;
		if (PreW == 1) LATE1 = 1;
	}
	if (press >= 19 && Level == 2)
	{
		LATE1 = 0;
		off1=0;
		LATE2 = 0; //Откл клапан
		return 1;
	}
	return 0;
}


//------------------------------------------------- СЛИВ --------------------------------------------
int ISl = 0;
bit Sliv()
{

	if (relayStart == 1)
	{
		if (off1==0)
		{
			off1 = 1;
		}

		if (press >= 1)
		{
			LATE0 = 1; //Вкл насос
			SDel = 0;
			ISl = 1;
		}

		if (press <= 0 && ISl == 0) return 1;

		if (press == 0 && SDel == 0)
			SDel = sys1;  //Проверка на наличие воды по I уровню прессостата.
		if (press == 0 && SDel!=0 && sys1 - SDel > 7000) //Cлив еще 10сек после срабатывания прессостата.
		{
			LATE0 = 0; // Откл насос.
			Sound(3000, 50);
			off1 = 0;
			SDel = 0;
			ISl = 0;
			return 1; //Закончили слив
		}
	}
	return 0;
}


void ProgSliv()
{
	if (relayStart == 1 && menuId == 4)
	{
		if (RDel1==0) {
			if (Sliv())
			{
				RDel1=1;
				AllOff();
				off1 = 0;

			}
		}
	}
}

//---------------------------------------- РЕГУЛИРОВКА ТЕМПЕРАТУРЫ ---------------------------------

void TempReg()
{
	if (StartTemp == 1 && press > 4) //Если есть вода в баке включаем ТЭН.
	{
		if (TempWaterOut < TempWaterIn && menuId == 0)
		{
			id = 0;
			LATB4=1;  //13 светодиод.
			lcd_puts(2,155,Heater);
			if (TempOk == 0) RDel = sys1;
		}
		if (TempWaterOut < TempPol && menuId == 6)
		{
			id = 0;
			LATB4=1;  //13 светодиод.
			lcd_puts(2,155,Heater);
			if (TempOk == 0) RDel = sys1;
		}
		if (TempWaterOut >= TempWaterIn && id == 0)
		{
			TempOk = 1;
			id++;
			lcd_puts(2,155,"        ");
		}
		if (TempWaterOut >= TempWaterIn && menuId == 0) LATB4 = 0;
		if (TempWaterOut >= TempWaterIn && menuId == 1) LATB4 = 0;
		if (TempWaterOut >= TempPol && menuId == 6) LATB4 = 0;
	}
	if (StartTemp == 0 || (PORTBbits.RB4 != 0 && press < 4)) LATB4=0;
}



//---------------------------------------------------- Процедура стирки ----------------------------

bit PWash(unsigned long TimePWash, int RelayWash, int RelayOsc, int PreW) // Миллисекунды
{
	if (relayStart == 1)
	{
		if (RDel1==0)
		{
			if (Sliv()) RDel1=2;
		}

		if (RDel1 == 2)
		{

			if (FillWater(1,PreW))
			{
				RDel1 = 4;
				RDel=0;
				StartTemp = 1;
				LATB3 = 1;
				RelayStartWash = RelayWash;
				RelayStartOsc = RelayOsc;
			}
		}

		if (DelayWash(TimePWash * 60000,RDel1,4))
		{
			StartTemp = 0;
			RDel1 = 5;
			RDel = 0;
			RelayStartWash = 0;
			RelayStartOsc = 0;
			AllOff();
			RDel = 0;
			IntRelay = 0;
		}

		if (RDel1 == 5)
			if (Sliv())
			{
				StartTemp = 0;
				RelayStartWash = 0;
				RelayStartOsc = 0;
				RDelW = 0;
				RDelW1 = 0;
				AllOff();
				RDel = 0;
				RDel1 = 0;
				IntRelay = 0;
				return 1;

			}
	}
	return 0;
}


//---------------------------------------------------- Доливка воды ------------------------------------


void ControlWater()
{

	if (IntRelay == 0)
	{
		LATE1 = 0; //Откл клапан.
		LATE2 = 0;
		IntRelay = 1;
	}

	if (IntRelay >= 3 && RDel1 == 4)
	{
		RSDel = 0;
		RelayStartWash = 0;
		RelayStartOsc = 0;
		CCPR1L = 0;
		if (press <= 6 && MStop == 1)
		{
			if (RDelWF == 0) LATE1 = 1;
			else if (RDelWF == 1) LATE2 = 1;
			CWDel = 0;
			RDel = sys1;
		}

		if (CWDel == 0) CWDel = sys1;
		if (press >= 7 && CWDel != 0 && sys1 - CWDel > 2000)
		{
			LATE1 = 0;
			LATE2 = 0;
			CWDel = 0;
			IntRelay = 0;
			RDelW1 = 0;
			RDelW = 0;
			if (menuId == 0 && menuId == 1) RDel = sys1; // Сброс счетчика стирки если вода доливалась.
			if (menuId == 0 || menuId == 6) RelayStartWash = 1;
			if (menuId == 1) RelayStartOsc = 1;
		}
	}
}


//------------------------------------------ Хлопок-----------------------------------------------



void Hlopok()
{
	if (menuId == 0 && relayStart == 1)
	{
		if (PWashT == 0) RDelWF = 1;
		if (RDelWF == 0)
		{
			if (PWash(PWashT,1,0,1) == 1)
			{
				RDelWF = 1; //60 000 * на количество минут
				PWashT = 0;
			}
		}

		if (RDelWF == 1)
		{
			if (PWash(PWashT1,1,0,0) == 1)
			{
				menuId = 6;
				RDelWF = 0;
				initDisplay();
			}
		}
	}
}




void CarefulWash() //------------------------------------Бережная----------------------------------
{
	if (menuId == 1 && relayStart == 1)
	{
		if (PWashT == 0) RDelWF = 1;
		if (RDelWF == 0)
		{
			if (PWash(PWashT,0,1,1) == 1) RDelWF = 1;
		}

		if (RDelWF == 1)
		{
			if (PWash(PWashT1,0,1,0) == 1)
			{
				menuId = 6;
				RDelWF = 0;
				initDisplay();
			}
		}
	}
}



void RelayOsc()    // режим вращения барабана пол оборота
{
	if (RelayStartOsc == 1)
	{
		if (RDelW1 == 0 && RDelW == 0) RDelW = sys1;
		if (RDelW1 == 0 && RDelW!=0 && sys1 - RDelW > 3600)
		{
			IntRelay++;
			CountOsc = 0;
			LATC4=1;
			CCPR1L = 10;
			RDelW1 = 1;
			RDelW = 0;
		}

		if (RDelW1 == 1 && CountOsc >= 30)
		{
			CCPR1L = 0;
			RDelW1 = 2;
			RDelW = 0;
		}
		if (RDelW1 == 2 && RDelW == 0) RDelW = sys1;
		if (RDelW1 == 2 && RDelW!=0 && sys1 - RDelW > 3600)
		{
			CountOsc = 0;
			LATC4=0;
			CCPR1L = 10;
			RDelW1 = 3;
			RDelW = 0;
		}

		if (RDelW1 == 3 && CountOsc >= 30)
		{
			CCPR1L = 0;
			RDelW1 = 0;
			RDelW = 0;
		}
	}
}




void RelayOsc2()  // Режим вращения глажки
{
	if (RelayStartOsc == 2)
	{
		if (RDelW1 == 0 && RDelW == 0) RDelW = sys1;
		if (RDelW1 == 0 && RDelW!=0 && sys1 - RDelW > RSDel)
		{
			RSDel = 3000;
			IntRelay++;
			LATC4=1;
			CCPR1L = 5+CCP;
			RDelW1 = 1;
			RDelW = 0;
		}


		if (RDelW1 == 1 && RDelW == 0) RDelW = sys1;
		if (RDelW1 == 1 && RDelW!=0 && sys1 - RDelW > 10000)
		{
			CCPR1L = 0;
			RDelW1 = 2;
			RDelW = 0;
		}

		if (RDelW1 == 2 && RDelW == 0) RDelW = sys1;
		if (RDelW1 == 2 && RDelW!=0 && sys1 - RDelW > RSDel)
		{
			RSDel = 3000;
			LATC4=0;
			CCPR1L = 5+CCP;
			RDelW1 = 3;
			RDelW = 0;
		}

		if (RDelW1 == 3 && RDelW == 0) RDelW = sys1;
		if (RDelW1 == 3 && RDelW!=0 && sys1 - RDelW > 10000)
		{
			CCPR1L = 0;
			RDelW1 = 0;
			RDelW = 0;
			CCP++;
			if (CCP>3)
			{
				CCPR1L = 0;
				RelayStartOsc = 0;
				menuId = 7;
				initDisplay();
				ControlStart = 0;
				StartBeep = 1;
			}
		}
	}
}



void relay() //стирка
{

	if (RelayStartWash==1)
	{
		if (RDelW1 == 0 && RDelW == 0) RDelW = sys1;
		if (RDelW1 == 0 && RDelW!=0 && sys1 - RDelW > RSDel)
		{
			RSDel = 3000;
			IntRelay++;
			LATC4=1;
			CCPR1L = 10+CCP;
			RDelW1 = 1;
			RDelW = 0;
		}


		if (RDelW1 == 1 && RDelW == 0) RDelW = sys1;
		if (RDelW1 == 1 && RDelW!=0 && sys1 - RDelW > 10000)
		{
			CCPR1L = 0;
			RDelW1 = 2;
			RDelW = 0;
		}

		if (RDelW1 == 2 && RDelW == 0) RDelW = sys1;
		if (RDelW1 == 2 && RDelW!=0 && sys1 - RDelW > RSDel)
		{
			RSDel = 3000;
			LATC4=0;
			CCPR1L = 10+CCP;
			RDelW1 = 3;
			RDelW = 0;
		}

		if (RDelW1 == 3 && RDelW == 0) RDelW = sys1;
		if (RDelW1 == 3 && RDelW!=0 && sys1 - RDelW > 10000)
		{
			CCPR1L = 0;
			RDelW1 = 0;
			RDelW = 0;
			CCP++;
			if (CCP>1) CCP = 0;
		}
	}
}


void Beeper()
{
	if (StartBeep == 1)
	{
		Sound(5000,100);
		Delay_ms(20000);
	}
}




void Wring()   //------------------------------------------------- Отжим-------------------------------
{
	if (veskg < 0) veskg = 0;
	if ((menuId == 5 && relayStart == 1) || (menuId == 51 && relayStart == 1))
	{
		LATC4 = 0;
		if (press >= 3 && CCPR1L == 0) // Задается уровень давления от которого включается таймер скапывания.
		{
			RDel = 1;
			RDel1 = 0;
		}
		else if (press <= 1  && RDel1 == 0 && CCPR1L == 0)
		{
			RDel = 0;
			RDel1 = 100;
		}

		if (DelayWash(60000,RDel1,100)) // Режим скапывания пока срабатывает насос не запускается стирка.60000мс
		{
			Sound(3000,100);
			Delay_ms(50);
			Sound(2000,100);
			Delay_ms(60);
			Sound(3000,100);
			Delay_ms(70);
			Sound(2000,100);
			CCPR1L = 5;
			RDel = 0;
			RDel1 = 1;
			CCPR1L = 8;
		}


		if (TUpRpm1 == 0) TUpRpm1 = sys1; //Распределение по баку
		if (RDel1 == 1 && TUpRpm1!=0 && sys1 - TUpRpm1 > 1500)
		{
			TUpRpm1 = sys1;
			CCPR1L ++;
			if (CCPR1L >= 17)
			{
				RDel1 = 6;
				RDel = 0;
				TUpRpm1 = 0;
			}
		}


		if (RDel1 == 6 && RDel == 0) RDel = sys1;
		if (RDel1 == 6 && RDel!=0 && sys1 - RDel > 2000)
		{
			if (AZ > AZRD6 && CCPR1L >= 10 || PORTCbits.RC3 == 0)  // Режим перемешивания и адаптации дисбаланса.
			{
				Sound(500,50);
				Sound(1000,50);
				CCPR1L = 0;
				RDel = 0;
				LATC4 = 1;
				Delay_ms(1000);
				CCPR1L = 9;
				Delay_ms(6000);
				CCPR1L = 0;
				Delay_ms(2000);
				LATC4 = 0;
				RDel1 = 5;
			}
		}

		if (MStop == 1 && RDel1 == 5)
		{
			CCPR1L = 8;
			RDel1 = 1;
			RDel = 0;
		}



		if (DelayWash(1000,RDel1,6))
		{
			if (AZ <= AZRD6-5)
			{
				Sound(4000, 50);
				RDelW = 0;
				RDel = 0;
				RDel1 = 7;
			}
		}



		if (TUpRpm1 == 0) TUpRpm1 = sys1; //Ускорение барабана до дисбаланса
		if (RDel1 == 7 && TUpRpm1!=0 && sys1 - TUpRpm1 > TimeDisb)
		{
			TUpRpm1 = sys1;
			CCPR1L ++;
			if (CCPR1L >= 30)
			{
				RDel1 = 8;
				ClOp = 1;
				RDelW = 0;
				RDel = 0;
				TUpRpm1 = 0;
			}
		}

		if (CCPR1L >= 100 && CCPOff == 0)
		{
			AdaptBit = 0;
			CCPOff = 1;
		}
		if ((PORTCbits.RC3 == 0 && CCPR1L <= 57) || (CCPR1L >= 17 && CCPR1L <= 30 && AZ >= AZCCP && CCPOff == 0 && AdaptCCP < 8)|| (Beetw == 0 && CCPR1L == 100 && AZ >= 250 && CCPOff == 0)) // || CCPR1L == 110 && AZ >= 250
		{

			if (Adapt == 4) AZCCP = AZCCP + 5;
			if (Adapt >= 8 && AdaptBit == 0)
			{
				TimeOldVes = TimeVes;
				CCPRPM = 55;
				AZCCP = 95;
				TimeVes = 100;
				CCPOff = 1;
				AdaptBit = 1;
				TimeDisb = 200;
				if (AdaptCCP == 1)
				{
					CCPRPM = 255;
					AdaptBit = 0;
				}
			}
			/*
			if (Adapt == 8) AZCCP = AZCCP + 5;
			if (Adapt == 12) AZCCP = AZCCP + 5;
			if (Adapt == 16) AZCCP = AZCCP + 5;
			if (Adapt == 20) AZCCP = AZCCP + 5;
			if (Adapt == 24) AZCCP = AZCCP + 5;
			if (Adapt == 28) AZCCP = AZCCP + 5;
			if (Adapt == 32) AZCCP = AZCCP + 5;
			if (Adapt == 36) AZCCP = AZCCP + 5;
			if (Adapt == 40) AZCCP = AZCCP + 5;
			if (Adapt == 44) AZCCP = AZCCP + 5;
			if (Adapt == 48) AZCCP = AZCCP + 5;
			if (Adapt == 52) AZCCP = AZCCP + 5;
			*/
			CCPR1L = 0;
			RDel = 0;
			TUpRpm1 = 0;
			Sound(3000, 200);
			Delay_ms(50);
			Sound(4000, 100);
			RDel1 = 101;
			Delay_ms(3000);
		}

		if (MStop == 1 && RDel1 == 222)
		{
			Sound(1000,50);
			Sound(900,50);
			Sound(800,50);
			Sound(700,50);
			Sound(800,50);
			Sound(900,50);
			Sound(1000,50);
			RDel1 = 6;
		}


		if (MStop == 1 && RDel1 == 101)
		{
			Adapt++;
			RDel = 0;
			LATC4 = 1;
			Delay_ms(2000);
			CCPR1L = 11;
			Delay_ms(6000);
			CCPR1L = 0;
			Delay_ms(2000);
			LATC4 = 0;
			RDel1 = 5;

		}

		if (DelayWash(1000,RDel1,8)) //20 000
		{
			TimeVes = veskg / 2;
			if (veskg <= 500 && AdaptBit == 0) TimeVes = 500;
			if (TUpRpm == 0) TUpRpm = sys1;
			if (RDel1 == 8 && TUpRpm!=0 && sys1 - TUpRpm > TimeVes) //Ускорение барабана
			{
				TUpRpm = sys1;
				CCPR1L++;
				if (CCPR1L >= 100) CCPR1L = 254;
				if (CCPR1L >= CCPRPM1 && Beetw == 1)
				{
					RDel = 0;
					RDel1 = 9;
				}

				if (CCPR1L >= CCPRPM && Beetw == 0)
				{
					if (AdaptBit == 0)
					{
						RDel = 0;
						RDel1 = 9;
					}
					if (AdaptBit == 1)
					{
						AdaptCCP++;
						Sound(1000,1000);
						CCPR1L = 0;
						TimeDisb = 1000;
						CCPOff = 0;
						AdaptCCP = 0;
						TimeVes = TimeOldVes;
						CCPRPM = 254;
						RDel1 = 222;
						AdaptBit = 0;
						Adapt = 0;
					}
				}
			}
		}


		if (Beetw == 0 && RDel1 == 9)
		{
			RDel1 = 10;
			CCPOff = 1;
			Sound(500,500);
		}


		if (Beetw == 1 && RDel1 == 9)
		{
			CCPR1L = 0;
			CCPOff = 1;
			if (MStop == 1)
			{
				WOk = 1;
				menuId = 6;
				RDel1 = 0;
				initDisplay();
			}
		}



		if (RDel1 == 10)
		{
			CCPR1L = CCPRPM;
		}


		if (RDel1 == 10 && TWring == 0) TWring = sys1;  // Сброс счетчика если включается насос и отдим дольше продолж
		if (RDel1 == 10 && TWring!=0 && sys1 - TWring > 60000)
		{
			RDel1 = 11;
			CCPR1L = 0;
		}


		if (MStop == 1 && RDel1 == 11)
		{
			ClOp = 0;
			ControlStart = 0;
			RelayStartOsc = 0;
			LATE0 = 0;
			Adapt = 0;
			relayStart = 0;
			//RelayStartOsc = 2;
			BitStart = 0;
			StartBeep = 1;
		}


	}
}



void Faltering() //Прерывистый режим насоса, автоматический слив.
{
	if ((relayStart == 1 && menuId == 5) || (relayStart == 1 && menuId == 51))
	{
		if (TUpRpm1 == 0) TUpRpm1 = sys1;
		if (TUpRpm1!=0 && press >= 3 && sys1 - TUpRpm1 > 500)
		{
			TUpRpm1 = sys1;
			{
				TWring = 0;
				LATE0 = 1; //Вкл насос
				FDel = sys1;
			}
		}
		if (press == 0 &&  FDel ==0 ) FDel = sys1;  //Проверка на наличие воды по давлению.
		if (press == 0 && FDel!=0 && sys1 - FDel > 7000) //Cлив еще сек после срабатывания прессостата.
		{
			LATE0 = 0; // Откл насос.
			FDel=0;
		}
	}
}
//------------------------------------------------Полоскание--------------------------------------------

void Poloskanie()
{

	if (relayStart == 1 && menuId == 6)
	{
		if (RDel1 == 0 && Sliv() == 1)
		{
			if (Beetw1 == 1) // Промежуточный отжим
			{
				CCPR1L = 0;
				RDel1 = 0;
				IntRelay = 0;
				RelayStartWash = 0;
				StartTemp = 0;
				LATE0 = 0;
				RDel=0;
				AllOff();
				Beetw = 1;
				Beetw1 = 0;
				Adapt = 0;
				AZCCP = 50;
				AZRD6 = 30;
				CCPOff = 1;
				CCPRPM1 = 80;
				menuId = 51;
				initDisplay();
			}
			if (I == 0)
			{
				I = 1;
				if (veskg >= 1500 && veskg <= 2000)
				{
					TPol+=1;
					if (TP == 0) IPress+=2;
				}
				if (veskg >= 2001 && veskg <= 2500)
				{
					if (TP == 0) IPress+=3;
					TPol+=2;
				}
				if (veskg >= 2501 && veskg <= 3000)
				{
					if (TP == 0) IPress+=4;
					QR+=1;
					// TPol+=3;
				}
				if (veskg >= 3001 && veskg <= 3500)
				{
					if (TP == 0) IPress+=5;
					QR+=2;
					// TPol+=3;
				}
				if (veskg >= 3501 && veskg <= 4000)
				{
					if (TP == 0) IPress+=6;
					QR+=2;
					// TPol+=3;
				}
				if (veskg >= 4001 && veskg <= 4500)
				{
					if (TP == 0) IPress+=7;
					QR+=3;
					//TPol+=3;
				}
				if (veskg >= 4501 && veskg <= 5000)
				{
					if (TP == 0) IPress+=8;
					//TPol+=3;
					QR+=4;
				}
				if (veskg >= 5001 && veskg <= 5500)
				{
					if (TP == 0) IPress+=9;
					//TPol+=3;
					QR+=4;
				}
				if (veskg >= 5501 && veskg <= 6000)
				{
					if (TP == 0) IPress+=10;
					//TPol+=4;
					QR+=4;
				}
				if (veskg >= 6001)
				{
					if (TP == 0) IPress+=12;
					//TPol+=4;
					QR+=4;
				}
			}
			CCPOff = 0;
			LATE0 = 0; // Откл насос.
			if (WOk == 1) RDel1=2;
			RDel=0;
			if (TP1 >= QR)
			{
				CCPR1L = 0;
				IntRelay = 0;
				RelayStartWash = 0;
				StartTemp = 0;
				LATE0 = 0;
				AllOff();
				RDel1 = 0;
				Beetw = 0;
				Beetw1 = 1;
				menuId = 5;
				CCPOff = 0;
				Adapt = 0;
				AZCCP = 30;
				AZRD6 = 20;
				initDisplay();
			}
		}
		if (RDel1 == 2)
		{
			if (press == 0) //Проверка на наличие воды в баке II уровень. Если  != 0  заливать.
			{
				IntRelay = 1;
				LATE1 = 1; //Заливка
				if (TP1 >= (QR-2)) LATE2 = 1; // Заливка в ополаскиватель.
				RelayStartWash = 0;
				CCPR1L = 0;
			}

			if (IPress > 25) IPress = 25;
			if (press >= IPress && TP == 0)  // 3-й уровень
			{
				LATE2 = 0; //Откл клапан
				LATE1 = 0;
				RelayStartWash = 1;
				StartTemp = 1;
				RDel1 = 3;
				RDel=0;
			}
			if (press >= 6 && TP == 1) // 2-й уровень 6
			{
				LATE2 = 0; //Откл клапан
				LATE1 = 0;
				RelayStartWash = 1;
				StartTemp = 1;
				RDel1 = 3;
				RDel=0;
			}
		}

		if (TempWaterOut >= TempPol && RDel1 == 3) RDel1 = 4; //Между заливкой воды
		if (DelayWash(TPol*60000,RDel1,4))
		{
			RDel = 0;
			Sound(2000,100);
			RelayStartWash = 0;
			if (TP1 <= QR) RDel1 = 0;
			TP++;
			TP1++;
			if (TP > 1) TP = 0;
			StartTemp = 0;
			LATE0 = 1;
			CCPR1L = 11;
			Beetw = 0;
			Beetw1 = 1;
			WOk = 0;
			AZCCP = 50;
			AZRD6 = 30;
		}
	}
}

void StepUp2(void) {
	encoder2roll = 1;
}

void StepDn2(void) {
	encoder2roll = 2;
}

/*
0 - Основной дисплей
1 - Настройка вакума
2 - Настройка гистрезиса
3 - Таймер верх
4 - Таймер низ
5 - Обороты вентилятра, температуры
6 - Таймер отчета времени
7 - Установка Таймер верх
8 - Установка Таймер низ
9 - Установка Таймер отчета времени
*/


void execEncoder1But(void)
{


	if (encoder2ButPrev==1 && PORTCbits.RC5==0 && ControlStart == 0)
	{
		//Кнопку только что нажали
		WaitButtTime = sys1; //Запустили таймер кнопки
		WaitPress1 = 0;
	}

	//Обработка долгого нажатия
	if (WaitPress1 == 0 && ControlStart == 0 && encoder2ButPrev==0 && PORTCbits.RC5==0 && sys1 - WaitButtTime > 1000)
	{
		//	Sound(4000,300);
		WaitPress++;
		WaitPress1 = 1;
		if (WaitPress > 1) WaitPress = 0;
		if (WaitPress == 0)
		{
			Sound(500,80);
			Delay_ms(80);
			Sound(200,90);
			SoundOn = 0;
			/*
			menuId = 0;
			initDisplay();
			*/
		}
		if (WaitPress == 1)
		{
			SoundOn = 1;
			Sound(200,90);
			Delay_ms(80);
			Sound(500,90);
			/*
			menuId = 9;
			initDisplay();
			*/
		}
	}


	if (encoder2ButPrev==0 && PORTCbits.RC5==1) {
		//Кнопку отпустили после нажатия
		if (sys1 - WaitButtTime <= 1000) {
			encoder2pressed =1; //Подняли флаг что кнопка нажата
		}
	}
	if (encoder2ButPrev != PORTCbits.RC5) encoder2ButPrev = PORTCbits.RC5;

	// Режим закрыть и открыть люк
	if (bat1pressprev==1 && PORTCbits.RC7==0)
	{
		WaitButt1 = sys1;
		WaitPressD = 0;
	}
	if (WaitPressD == 0 && bat1pressprev == 0 && PORTBbits.RB3 == 0 && sys1 - WaitButt1 > 2000)
	{
		WaitPD++;
		WaitPressD = 1;
		if (WaitPD > 1) WaitPD = 0;
		if (WaitPD == 0)
		{
			Sound(1000,100);
			Delay_ms(80);
			Sound(2000,100);
			ClOp = 1;
		}

		if (WaitPD == 1)
		{
			Sound(2000,100);
			Delay_ms(80);
			Sound(1000,100);
			ClOp = 0;
		}
	}


	if (bat1pressprev==0 && PORTCbits.RC7==1)
	{   //Кнопку отпустили после нажатия
		//	bat1press =1; //Подняли флаг что кнопка нажата
		if (sys1 - WaitButt1 <= 2000)
		{
			bat1press =1; //Подняли флаг что кнопка нажата
			WaitButt1 = sys1;
		}

	}

	if (bat1pressprev != PORTCbits.RC7) bat1pressprev = PORTCbits.RC7;


	if (bat2pressprev==0 && PORTBbits.RB2==1) {
		//Кнопку отпустили после нажатия
		bat2press =1; //Подняли флаг что кнопка старт нажата
	}


	if (bat2pressprev != PORTBbits.RB2) bat2pressprev = PORTBbits.RB2;
}


void inputProcess(){
	signed int menuIdprev = menuId; //Запомним текущее меню

	if (encoder2roll)
	{

		if (ControlStart == 1)
		{
			Sound(2000,20);
			if (menuId == 5)//Обороты барабана на ходу
			{
				RpmDrumDsp += (encoder2roll==1)? 100 : -100;
				if (RpmDrumDsp < 200) RpmDrumDsp = 200;
				else  if (RpmDrumDsp > 1100) RpmDrumDsp = 1100;
			}
			if (RpmDrumDsp == 200) CCPRPM = 31;
			if (RpmDrumDsp == 300) CCPRPM = 37;
			if (RpmDrumDsp == 400) CCPRPM = 47;
			if (RpmDrumDsp == 500) CCPRPM = 55;
			if (RpmDrumDsp == 600) CCPRPM = 64;
			if (RpmDrumDsp == 700) CCPRPM = 74;
			if (RpmDrumDsp == 800) CCPRPM = 80;
			if (RpmDrumDsp == 900) CCPRPM = 91;
			if (RpmDrumDsp == 1000) CCPRPM = 104;
			if (RpmDrumDsp == 1100) CCPRPM = 254;
			encoder2roll = 0;
		}

		if (ControlStart == 0)
		{
			Sound(2000,20);
			if (menuId != 603 && menuId != 602 && menuId != 601 && menuId!=1001 && menuId!=1002 && menuId!=1003 && menuId!=1004 &&menuId!=1005 && menuId!=110 && menuId!=210 && menuId!=310 && menuId!=410 && menuId!=510 && menuId!=610 && menuId!=710 && menuId!=810 && menuId!=910 && menuId!=1010 && menuId!=1110 && menuId!=1210)
			{
				if (encoder2roll==1)  //Первая часть меню.
				{
					if (menuId==0) menuId=1;
					else if (menuId==1) menuId=4;
					//  else if (menuId==2) menuId=3;
					//  else if (menuId==3) menuId=4;
					else if (menuId==4) menuId=5;
					else if (menuId==5) menuId=6;
					// else if (menuId==6) menuId=7;
					else menuId = 0;
				}
				else
				{

					if (menuId==0) menuId=6;
					else if (menuId==1) menuId=0;
					else if (menuId==4) menuId=1;
					//	else if (menuId==3) menuId=2;
					//	else if (menuId==4) menuId=3;
					else if (menuId==5) menuId=4;
					else if (menuId==6) menuId=5;
					//else if (menuId==7) menuId=6;
					else menuId = 6;
				}
			}
			else
			{
				if (menuId == 1001)  // Подменю menuId 0.
				{
					//Меняем температуру воды!!
					TempWaterIn += (encoder2roll==1)? 5 : -5;
					if (TempWaterIn < 20) TempWaterIn = 20;
					else  if (TempWaterIn > 95) TempWaterIn = 95;
				}


				else if (menuId == 1002) //Предварительная стирка
				{
					PWashT += (encoder2roll==1)? 5 : -5;
					if (PWashT <= 5) PWashT = 5;
					else  if (PWashT > 60) PWashT = 5;
				}

				else if (menuId == 1003)//Вторая стирка
				{
					PWashT1 += (encoder2roll==1)? 5 : -5;
					if (PWashT1 <= 0) PWashT1 = 0;
					else  if (PWashT1 > 60) PWashT1 = 5;
				}

				else if (menuId == 1004)//Обороты барабана
				{
					RpmDrumDsp += (encoder2roll==1)? 100 : -100;
					if (RpmDrumDsp < 200) RpmDrumDsp = 200;
					else  if (RpmDrumDsp > 1100) RpmDrumDsp = 1100;

				}

				else if (menuId==601)
				{
					QR+=(encoder2roll==1)? 1:-1;
					if (QR > 10) QR = 10;
					if (QR < 1) QR = 1;
				}

				else if (menuId==602)
				{
					TPol+=(encoder2roll==1)? 1:-1;
					if (TPol > 10) TPol = 10;
					if (TPol < 1) TPol = 1;
				}

				else if (menuId==603)
				{
					TempPol+=(encoder2roll==1)? 5:-5;
					if (TempPol > 95) TempPol = 95;
					if (TempPol < 10) TempPol = 10;
				}
				else if (menuId==210)
				{

					//Меняем значение Установка таймер отчета времени!!
				}

				else if (menuId==310)
				{

					//Меняем значение Установка таймер отчета времени!!
				}
			}
			encoder2roll = 0;
		}
	}


	//<<----------------------------------------

	//Нажали энкодер2 ---------------------------------------------------------------
	if (encoder2pressed==1)
	{
		Sound(2000,30); // Sound 2000Hz,0.5 sec
		//	isIdle = 0; //Сбрасываем флаг простоя
		if (menuId == 0) menuId = 1001;
		else if (menuId == 1001) menuId = 1002;
		else if (menuId == 1002) menuId = 1003;
		else if (menuId == 1003) menuId = 1004;
		else if (menuId == 1004) menuId = 0;
		//  else if (menuId == 1005) menuId = 0;


		if (menuId == 6) menuId = 601;
		else if (menuId == 601) menuId = 602;
		else if (menuId == 602) menuId = 603;
		else if (menuId == 603) menuId = 6;

		encoder2pressed = 0;
	}



	//Нажали кнопку старт -----------------------------------------------------------
	if (bat2press == 1)
	{
		if (BitStart >= 2) BitStart = 0;
		Sound(1000,30); // Sound 2000Hz,0.5 sec

		if (BitStart == 1)
		{
			ClOp=0;
			menuIdOld = menuId;
			CloseDoor();
			menuId=8;
			initDisplay();
			Sound(3000,50); // Sound 2000Hz,0.5 sec
			Delay_ms(100);
			Sound(3000,50); // Sound 2000Hz,0.5 sec
			Delay_ms(100);
			Sound(3000,50); // Sound 2000Hz,0.5 sec
			SysOld = sys1;
			LATB5 = 0;
			StartTemp = 0;
			CCPR1L = 0;
			bat2press = 0;
			LATB4 = 0;
			LATE0 = 0;  //- Насос.
			LATE1 = 0;  //- Клапан 2. I лоток
			LATE2 = 0;  // Клапан 1
			while (1)
			{
				lcdRedraw();
				if (PORTBbits.RB2 == 0)
				{
					Sound(500,100);
					BitStart = 0;
					StartTemp = 1;
					bat2press = 0;
					sys1 = SysOld;
					menuId = menuIdOld;
					initDisplay();
					ClOp = 1;
					break;
				}
			}
		}

		if (BitStart == 0)
		{
			TimeDisb = 1000;
			CCPOff = 0;
			TimeVes = 500;
			CCPRPM = 0;
			Adapt = 0;
			ClOp = 1;
			menuIdOld = menuId;
			CloseDoor();
			if (PORTBbits.RB5 == 1)
			{
				Sound(1000,50);
				relayStart=1;
				ControlStart = 1;
				initDisplay();

				I = 0;
				if (RpmDrumDsp == 200) CCPRPM = 31;
				if (RpmDrumDsp == 300) CCPRPM = 37;
				if (RpmDrumDsp == 400) CCPRPM = 47;
				if (RpmDrumDsp == 500) CCPRPM = 55;
				if (RpmDrumDsp == 600) CCPRPM = 64;
				if (RpmDrumDsp == 700) CCPRPM = 74;
				if (RpmDrumDsp == 800) CCPRPM = 80;
				if (RpmDrumDsp == 900) CCPRPM = 91;
				if (RpmDrumDsp == 1000) CCPRPM = 104;
				if (RpmDrumDsp == 1100) CCPRPM = 254;

			}
		}
		bat2press = 0;
		BitStart++;
	}
	//<<----------------------------------------

	//Нажали кнопку пауза стоп -------------------------------------------------------
	if (bat1press == 1)
	{
		Adapt = 0;
		Sound(2000,30); // Sound 2000Hz,0.5 sec
		id = 0;
		//	if (menuId == 1001 && menuId == 1002 && menuId == 1003 && menuId == 1004) menuId = 7;
		StartBeep = 0;
		TempOk = 0;
		TimeDisb = 1000;
		CCPOff = 0;
		TimeVes = 500;
		CCPRPM = 0;
		Adapt = 0;
		BitStart = 0;
		ControlStart = 0;
		CCPR1L = 0;
		StartTemp=0;
		relayStart=0;
		RDelWF = 0;
		RDelW, RDelW1 = 0;
		RDel = 0;
		RDel1 = 0;
		TP1 = 0;
		TP = 0;
		RelayStartWash = 0;
		RelayStartOsc = 0;
		AllOff();
		bat1press = 0;
		menuId = 7;
		initDisplay();
		ClOp = 0;
	}


	if (menuIdprev != menuId)
		initDisplay();
}


void redrawDisplay()
{
	if (sys1 - dispDel < 200) return;
	dispDel = sys1;

	if (menuId == 0 )  //Хлопок
	{
		int Time = 0;

		sprintf(v,"%02d%s",TempWaterOut,"C");
		lcd_puts(1,210,v);
		sprintf(v,"%02d%s",TempFan,"C");
		lcd_puts(1,229,v);

		sprintf(v,"Press:%02d",press);
		lcd_puts(1,128,v);

		//sprintf(v,"AZ:%03d",AZ);
		//lcd_puts(2,215,v);

		sprintf(v,"%04d %s",veskg,Gr);
		lcd_puts(2,202,v);

		Time = (PWashT + PWashT1 + (TPol * QR)) - ((sys1 - RDel) / 60000);
		sprintf(v,"%02d Min",Time);
		lcd_puts(2,146,v);

		sprintf(v,"%04d %s",count1 * 5,Millilitres);
		lcd_puts(2,225,v);

		if (PORTBbits.RB5 == 1)lcd_puts(1,157,Door);
		if (PORTBbits.RB5 == 0)lcd_puts(1,157,OpenDoor);

		sprintf(v,"C:%03d",CCPR1L);
		lcd_puts(1,137,v);
		/*
		sprintf(v,"%04ld%s",freq,rpm);
		lcd_puts(2,224,v);
		sprintf(v,"%04ld%s",DrumRpm,rpm);
		lcd_puts(2,209,v);
		*/
	}

	else if (menuId == 1 ) //Бережная шерсть
	{
		int Time = 0;
		sprintf(v,"%05d%s",count1 * 5,Millilitres);
		lcd_puts(2,224,v);

		if (PORTBbits.RB5 == 1)lcd_puts(1,157,Door);
		if (PORTBbits.RB5 == 0)lcd_puts(1,157,OpenDoor);

		sprintf(v,"%04d%s ",veskg,Gr);
		lcd_puts(2,202,v);



		Time = (PWashT + PWashT1 + (TPol * QR)) - ((sys1 - RDel) / 60000);
		sprintf(v,"%02d min",Time);
		lcd_puts(2,146,v);

		sprintf(v,"CCP:%03d",CCPR1L);
		lcd_puts(1,159,v);


		sprintf(v," Pres:%02d ",press);
		lcd_puts(1,128,v);

		sprintf(v,"%02d%s",TempWaterOut,"C");
		lcd_puts(1,210,v);
		sprintf(v,"%02d%s",TempFan,"C");
		lcd_puts(1,229,v);
	}


	else if (menuId == 4 ) //Слив
	{
		sprintf(v,"%d%s",TempFan,"C");
		lcd_puts(1,209,v);
		sprintf(v,"%02d ",press);
		lcd_puts(2,146,v);
	}

	else if (menuId == 5 ) //Отжим
	{
		//sprintf(v,"ID:%03d ",menuId);
		//lcd_puts(1,128,v);

		sprintf(v,"%04d%s ",RpmDrumDsp,rpm);
		lcd_puts(1,221,v);

		sprintf(v,"RDl:%03d",RDel1);
		lcd_puts(1,136,v);
		sprintf(v,"AZ:%03d",AZ);
		lcd_puts(1,210,v);

		sprintf(v,"Ad:%d",Adapt);
		lcd_puts(2,145,v);

		sprintf(v,"CRP:%d",CCPRPM);
		lcd_puts(2,151,v);


		sprintf(v,"%04d%s ",veskg,Gr);
		lcd_puts(2,138,v);

		if (PORTBbits.RB5 == 1)lcd_puts(1,157,Door);
		if (PORTBbits.RB5 == 0)lcd_puts(1,157,OpenDoor);

		if (MStop == 1)
		{
			freq = 0;
			DrumRpm = 0;
		}
		sprintf(v,"%04ld%s",freq,rpm);
		lcd_puts(2,224,v);
		sprintf(v,"%04ld%s",DrumRpm,rpm);
		lcd_puts(2,209,v);
		sprintf(v,"Pres:%02d ",press,"L ");
		lcd_puts(1,128,v);
		sprintf(v,"%02d%s",TempFan,"C");
		lcd_puts(1,205,v);
		if (RDel1 == 100)
		{
			//	sprintf(v,"Drops Time:%02ld ",(sys1 - RDel) / 1000);
			//	lcd_puts(2,152,v);
		}

		if (RDel1 == 10)
		{
			sprintf(v,"Recycle Time:%02ld ",(sys1 - TWring) / 1000);
			lcd_puts(2,152,v);
		}
	}

	else if (menuId == 51)
	{
	//	sprintf(v,"ID:%03d ",menuId);
	//	lcd_puts(1,128,v);

		sprintf(v,"%04d%s ",RpmDrumDsp,rpm);
		lcd_puts(1,221,v);

	//	sprintf(v,"RDl:%03d",RDel1);
	//	lcd_puts(1,136,v);
		sprintf(v,"AZ:%03d",AZ);
		lcd_puts(1,210,v);

		if (PORTBbits.RB5 == 1)lcd_puts(1,157,Door);
		if (PORTBbits.RB5 == 0)lcd_puts(1,157,OpenDoor);

	//	sprintf(v,"A:%d",Adapt);
	//	lcd_puts(2,145,v);

//		sprintf(v,"C:%d",AZCCP);
//		lcd_puts(2,150,v);


		sprintf(v,"%04d%s ",veskg,Gr);
		lcd_puts(2,138,v);

		if (MStop == 1)
		{
			freq = 0;
			DrumRpm = 0;
		}
		sprintf(v,"%04ld%s",freq,rpm);
		lcd_puts(2,224,v);
		sprintf(v,"%04ld%s",DrumRpm,rpm);
		lcd_puts(2,209,v);
		sprintf(v,"Press:%02d ",press,"L ");
		lcd_puts(1,156,v);
		sprintf(v,"%02d%s",TempFan,"C");
		lcd_puts(1,205,v);
		if (RDel1 == 100)
		{
			sprintf(v,"Drops Time:%02ld ",(sys1 - RDel) / 1000);
			lcd_puts(2,152,v);
		}

		if (RDel1 == 10)
		{
			sprintf(v,"Recycle Time:%02ld ",(sys1 - TWring) / 1000);
			lcd_puts(2,152,v);
		}
	}

	else if (menuId == 6 ) // ******************************************ПОЛОСКАНИЕ*******************************
	{

		ITimePol = (TPol * (QR-TP1))  - ((sys1 - RDel) / 60000);
		if (ITimePol < 0 || ITimePol > 100) ITimePol = 0;

		sprintf(v,"%02d%s",TempWaterOut,"C");
		lcd_puts(1,210,v);
		sprintf(v,"%02d%s",TempFan,"C");
		lcd_puts(1,228,v);

		sprintf(v,"Lev:%02d L ",press);
		lcd_puts(1,128,v);

		sprintf(v,"%02ld min ",ITimePol);
		lcd_puts(2,145,v);
		sprintf(v,"%02d ",TP1); //Пройдено полосканий
		lcd_puts(2,213,v);
		sprintf(v,"%04d%s ",veskg,Gr);
		lcd_puts(2,225,v);

		if (PORTBbits.RB5 == 1)lcd_puts(1,157,Door);
		if (PORTBbits.RB5 == 0)lcd_puts(1,157,OpenDoor);

	}

	else if (menuId == 1001 )
	{
		sprintf(v,"%d%s",TempWaterIn,"C");
		lcd_puts(1,145,v);

	}

	else if (menuId == 601 )
	{
		sprintf(v,"%02d ",QR);
		lcd_puts(1,151,v);

	}
	else if (menuId == 602 )
	{
		sprintf(v,"%02d ",TPol);
		lcd_puts(1,218,v);
	}
	else if (menuId == 603 )
	{
		sprintf(v,"%02d ",TempPol);
		lcd_puts(2,146,v);

	}

	else if (menuId == 1002)
	{
		sprintf(v,"%02ld%s",PWashT," Min ");
		lcd_puts(1,210,v);
	}

	else if (menuId == 1003)
	{
		sprintf(v,"%02ld%s",PWashT1," Min ");
		lcd_puts(1,224,v);
	}
	else if (menuId == 1004)
	{
		sprintf(v,"%04d%s",RpmDrumDsp,"Rpm ");
		lcd_puts(2,134,v);
	}
}

int calcP(int val)
{
	float v = (5.0 / 1022.0)*val;
	return (v/4.99-0.04)/0.009;
}



void interrupt CCP1_INT(void)
{
	if (INT1IF) // n - INT1 = RB1 - Расходомер.
		// n - (INTCON3) bit 0 INT1IF Флаг прерывания от внешнего входа INT1: 1 – произошло прерывание(сбрасывать надо программно); 0 – прерывания не было
	{
		INT1IF = 0;
		if (menuId != 6)
		{
			count1++;
			veskg = count1 * 5 - 4600;
			veskg = veskg / 3;
			if (veskg < 0) veskg = 0;
		}
	}

	if (INT0IF) // n - INT0 = RB0 - Тахометр.
		// n - (INTCON) bit 1 INT0IF Флаг прерывания от внешнего входа INT0: 1 – произошло прерывание(сбрасывать надо программно); 0 – прерывания не было
	{
		INT0IF = 0;
		count++;
		Rotate++;
		if (Rotate > 10000) Rotate = 0;
		CountOsc++;
		if (time1 == 0) time1 = sys1;
		else if (sys1 - time1 > 100)
		{
			freq = (count * 600) / 8;
			DrumRpm = freq / 12;
			time1 = sys1;
			count = 0;
		}
	}

	if (TMR0IF)
		// n - Как только значение таймера 0 станет 0xFF в 8 - ми битном режиме или 0xFFFF в 16 - ти битном, и придет новый импульс,
		// таймер 0, автоматически сбросит значения регистров в 0 и выставит флаг прерывания в регистре INTCON(бит 2 – TMR0IF).
		// Если разрешены прерывания, то программа уйдет в обработчик прерываний.
		// n - (INTCON) bit 2 TMR0IF Флаг прерывания от переполнения таймера 0: 1 – произошло прерывание(сбрасывать надо программно); 0 – прерывания не было
	{
		//sys1+=2;  //13
		sys1 = sys1 + 2;
		TMR0 = 131;   //3
		TMR0IF = 0;
		// n - Когда таймер 0 включен, он начинает инкрементироваться с каждым входным импульсом.
		// Поэтому, внимание, чтобы точно отрабатывался временной промежуток, который Вы хотите запрограммировать,
		// перед включением таймера 0 необходимо сбросить значения регистров TMR0H, TMR0L или только регистр TMR0L, если режим 8-ми битный!!!
		// А также сбросить флаг прерывания в регистре INTCON (бит 2 – TMR0IF).
	}
}



void main()
{
	TRISA = 0b01011011; //n - Регистр TRISx задает направление пинов порта. Если бит этого регистра установлен в единицу, то данный пин будет сконфигурирован, как вход. Если ноль – как выход.
	TRISB = 0b11100111; //n - Обязательно, сначала формируйте значения выходов, а потом конфигурируйте сами пины, чтобы не появилось мгновенных неверных значений на выходах.
	TRISC = 0b10101000;
	TRISD = 0b00000001;
	TRISE = 0b00000000; //n - IR IEVEROTS NOSACIJUMS KA VISPIRMS KONFIGURET IZEJAS/IEEJAS TIPU UN PEC TAM PINUS

	// n - Регистр PORTx используется обычно для чтения входных дискретных сигналов
	PORTA = 0b00000000; //n - Первым идет PORTA.  Как видим, он состоит из 6-ти битов RA0-RA6.
	PORTB = 0b00000000; //n - Для входов PORTB можно использовать дополнительную подтяжку ~50кОм. Подключается она сбросом в ноль бита  RBPU регистра INTCON2.
	PORTC = 0b00000000;
	PORTE = 0b00000000;

	// n - чтобы изменить значения на выводах порта, необходимо использовать регистр LATx
	LATD = 0b00000000;
	LATA0 = 1;
	LATA2 = 1; //Нагреватель концевика
	LATA7 = 0; // Соленоид
	LATB4 = 0;
	LATB5 = 0;
	LATC4 = 0;
	LATC7 = 0;
	LATE0 = 0;
	/*
	LATB3 = 0;
	LATE0 = 1;
	Delay_ms(5000);
	LATE2=1;
	LATE1=1;
	LATE0=1;
	Delay_ms(5000);
	LATE1=0;
	LATE2=0;
	Delay_ms(17000);
	LATE0=0;
	initDisplay();
	*/

	// n - (TMR0) Таймер 0
	// n - Основной регистр управления данного таймера T0CON. Рассмотрим назначение битов данного регистра:
	T0CONbits.T0PS = 0b111;
	//n - (T0CON) bit 2-0 T0PS2-0 3 бита, которые задают делитель входной частоты : 111 – частота делится на 256; 110 – частота делится на 128;
	//101 – частота делится на 64; 100 – частота делится на 32; 011 – частота делится на 16; 010 – частота делится на 8; 001 – частота делится на 4; 000 – частота делится на 2;
	T0CONbits.T0CS	= 0;
	// n - (T0CON) bit 5 T0CS 1 – таймер 0 инкрементируется импульсами, идущими с RA4 / T0CKI; 0 – таймер 0 инкрементируется внутренними импульсами;
	TMR0ON = 1;
	// n - (T0CON) bit 7 TMR0ON 1 – включить таймер 0; 0 – остановить таймер 0;

	// n - (TMR1) таймер 1
	// n - (TMR1) Основной регистр управления данного таймера T1CON. Рассмотрим назначение битов этого регистра:
	T1CON &= 0b11111101;
	// n - (T1CON) bit 7 RD16 1 – доступ к значениям таймера одной 16 - ти битной операцией; 0 – доступ к  значениям таймера двумя 8 - ми битными операциями
	// n - (T1CON) bit 6 - не используется
	// n - (T1CON) bit 5-4 T1CKPS1-0 2 бита для конфигурирования делителя входной частоты(прескалер): 00 – 1:1; 01 – 1:2; 10 – 1:4; 11 – 1:8;
	// n - (T1CON) bit 3 T1OSCEN 1 – включить внутреннюю схему тактирования от внешнего кварца; 0 – выключить.RC0, 1 используются, как дискретные входы / выходы
	// n - (T1CON) bit 2 T1SYNC 1 – не синхронизировать частоту внешнего генератора; 0 – синхронизировать; Если тактирование таймера от Fosc / 4, то этот бит игнорируется
	// n - (T1CON) bit 1 TMR1CS 1 – тактирование таймера от внешнего генератора; 0 – тактирование таймера от частоты командных циклов(Fosc / 4);
	// n - Если необходимо подключить схему тактирования от внешнего кварца, то, помимо T1OSCEN, данный бит, также, должен быть установлен.
	// n - (T1CON) bit 0 TMR1ON 0 –  остановить таймер1; 1 –  включить таймер 1;
	// n - (TMR1) Таймер может работать в режиме счетчика, когда настроен на тактирование от внешнего генератора (пин T1CKI).
	// n - Тогда, инкремент таймера происходит с каждым нарастающим фронтом входного импульса.

	// n - (TMR2) таймер 2
	// n - (TMR2) Основной регистр управления данного таймера T2CON. Рассмотрим назначение битов регистра:
	T2CON |= 0b00000100;		// Start timer2
	// n - (T2CON) bit 7 – не используется
	// n - (T2CON) bit 6-3 TOUTPS3-0 4 бита для конфигурирования делителя выходной частоты с таймера: 0000 – 1:1 постскалер; 0001 – 1:2 постскалер; 0010 – 1:3 постскалер; ...; 1111 – 1:16 постскалер
	// n - (T2CON) bit 2 TMR2ON 1 – включить таймер 2; 0 – выключить таймер 2;
	// n - (T2CON) bit 1-0 T2CKPS1-0 2 бита, которые задают делитель входной частоты на таймер: 00 –  входная частота не делится; 01 –  входная частота делится на 4;
	// n - 10 –  входная частота делится на 16; 11 –  входная частота делится на 16;
	PR2 = 180;
	// n - (TMR2) Но есть у него один регистр, который позволяет таймеру 2 формировать промежутки, практически, с любым периодом!
	// n - Это регистр PR2. Как мы помним из выпуска №12 рассылки, флаг прерывания таймера 0 взводился, когда происходило переполнение значения таймера (FFh->00h).
	// n - Так вот, в таймере 2 флаг прерывания (PIR1.TMR2IF) взводится, когда значение TMR2  совпадет со значением, находящимся в регистре PR2, и количество совпадений,
	// n - станет равно делителю выходной частоты таймера 2. После совпадения TMR2 и PR2, TMR2 автоматически сбрасывается в ноль, и на постскалер приходит очередной импульс.
	// n - Не забываем самое важное, что любая программная модификация регистров TMR2 или T2CON приводит к сбросу прескалера и постскалера, который грозит нарушением длительности периода!!!

	// n - (INT) Interrupts
	// n - (INT)  Регистр, с которого я хотел бы начать, RCON.
	IPEN = 0;
	// n - (RCON) В этом регистре нас, в рамках данной темы, интересует только 7-ой бит – IPEN.
	// n - Данный бит разрешает или запрещает работу с приоритетами прерываний. Если он установлен в единицу, то приоритетные прерывания разрешены.
	// n - Если, сброшен, то приоритеты отключены. Тогда система прерываний работает в совмещенном режиме. Биты регистров выбора приоритета прерываний (IPR)
	// n - игнорируются, и обработчик прерываний назначается только на вектор 0x08.
	// n - (INT) Следующий регистр INTCON.Самый основной регистр системы прерываний, т.к.здесь расположены глобальные биты разрешения/запрета прерываний.
	GIE = 1;
	// n - (INTCON) bit 7 GIE/GIEH
	//Если IPEN = 1 (приоритеты разрешены) GIEH: 0 – запрещены все высокоприоритетные прерывания; 1 – разрешено отрабатывать высокоприоритетные прерывания.
	//Если IPEN = 0 (приоритеты запрещены) GIE: 0 – запрещены все прерывания; 1 – разрешено отрабатывать прерывания.
	PEIE = 1;
	//n - (INTCON) bit 6 PEIE/GIEL
	//n - Если IPEN = 1 (приоритеты разрешены) GIEL: 0 – запрещены все низкоприоритетные прерывания; 1 – разрешено отрабатывать низкоприоритетные прерывания.
	//n - Если IPEN = 0 (приоритеты запрещены) PEIE: 0 – запрещены все периферийные прерывания;	1 – разрешено отрабатывать периферийные прерывания.
	//n - Периферийные прерывания – это прерывания, которые управляются регистрами PIE, PIR, IPR, которые мы рассмотрим в следующем выпуске.
	TMR0IE = 1;
	// n - (INTCON) bit 5 TMR0IE 1 – разрешить прерывание от переполнения таймера 0; 0 – запретить прерывание от переполнения таймера 0;
	INT0IE = 1; //Включить прерывание RB0
	// n - (INTCON) bit 4 INT0IE 1 – разрешить прерывание от внешнего входа INT0; 0 – запретить прерывание от внешнего входа INT0;
	RBIE = 0;
	// n - (INTCON) bit 3 RBIE 1 – разрешить прерывание от изменения состояния входов RB7-4; 0 – запретить прерывание от изменения состояния входов RB7-4;
	// n - (INTCON) bit 2 TMR0IF Флаг прерывания от переполнения таймера 0: 1 – произошло прерывание(сбрасывать надо программно); 0 – прерывания не было;
	// n - (INTCON) bit 1 INT0IF Флаг прерывания от внешнего входа INT0: 1 – произошло прерывание(сбрасывать надо программно); 0 – прерывания не было;
	// n - (INTCON) bit 0 RBIF Флаг прерывания от изменения состояния входов RB7-4: 1 – произошло прерывание(сбрасывать надо программно); 0 – прерывания не было;
	// n - Чтобы сбросить этот флаг, необходимо предварительно прочитать программой состояние PORTB, а потом только сбрасывать флаг.

	// n - (INT) Следующий регистр INTCON2.
	RBPU = 0;
	// n - (INTCON2) bit 7 RBPU 0 – разрешена подтяжка внутренних резисторов на + 5В, когда пины PORTB настроены на вход; 1 – подтяжка на все пины PORTB отключена.
	// n - (INTCON2) bit 6 INTEDG0 Бит настройки внешнего прерывания INT0: 0 – прерывание происходит от спадающего фронта входного сигнала; 1 – прерывание происходит от нарастающего фронта входного сигнала
	// n - (INTCON2) bit 5 INTEDG1 Бит настройки внешнего прерывания INT1: 0 – прерывание происходит от спадающего фронта входного сигнала; 1 – прерывание происходит от нарастающего фронта входного сигнала
	// n - (INTCON2) bit 4 INTEDG2 Бит настройки внешнего прерывания INT2: 0 – прерывание происходит от спадающего фронта входного сигнала; 1 – прерывание происходит от нарастающего фронта входного сигнала
	// n - (INTCON2) bit 3 - не используется
	TMR0IP = 1; // timer 0
	// n - (INTCON2) bit 2 TMR0IP Бит управления приоритетом прерывания переполнения таймера 0: 1 – высокий приоритет; 0 – низкий приоритет
	// n - (INTCON2) bit 1 - не используется
	RBIP = 0;
	// n - (INTCON2) bit 0 RBIP Бит управления приоритетом прерывания изменения состояния RB7-4: 1 – высокий приоритет; 0 – низкий приоритет

	// n - (INT) Следующий регистр INTCON3.
	// n - (INTCON3) bit 7 INT2IP Бит управления приоритетом прерывания внешнего входа INT2: 1 – высокий приоритет; 0 – низкий приоритет
	// n - (INTCON3) bit 6 INT1IP Бит управления приоритетом прерывания внешнего входа INT1: 1 – высокий приоритет; 0 – низкий приоритет
	// n - (INTCON3) bit 5 - не используется
	// n - (INTCON3) bit 4 INT2IE 1 – разрешить прерывание от внешнего входа INT2; 0 – запретить прерывание от внешнего входа INT2
	INT1IE = 1; // Прерывание RB1
	// n - (INTCON3) bit 3 INT1IE 1 – разрешить прерывание от внешнего входа INT1; 0 – запретить прерывание от внешнего входа INT1;
	// n - (INTCON3) bit 2 - не используется
	// n - (INTCON3) bit 1 INT2IF Флаг прерывания от внешнего входа INT2: 1 – произошло прерывание(сбрасывать надо программно); 0 – прерывания не было
	// n - (INTCON3) bit 0 INT1IF Флаг прерывания от внешнего входа INT1: 1 – произошло прерывание(сбрасывать надо программно); 0 – прерывания не было

	PIE1 = 0b00000000;
	// n - биты управления прерываниями периферийных устройств, которые расположены в регистрах PIR1, PIE1, IPR1 и PIR2, PIE2, IPR2
	// n - биты формата хххIE располагаются в регистрах PIE1 и PIE2. Это биты разрешения прерываний на каждый конкретный периферийный модуль.
	// n - Если бит, установлен в единицу, то прерывание от  соответствующего модуля разрешено, 0 – нет.


	// n -  У каждого модуля захвата/сравнения, также, есть регистр управления – CCPxCON.
	CCP1CON = 0b001100;	 //CCP1 PWM mode,CCP1M3:CCP1M0=1100,P1M1:P1M0=10
	CCP2CON |= 0b001100;
	// n - (CCPxCON) bit 7-6 - Не используются
	// n - (CCPxCON) bit 5-4 DCxB1-DCxB0 Данные биты используются только в режиме ШИМ, и являются младшими битами  b1 и b0  10-ти разрядной скважности ШИМ.
	// n - Старшие биты скважности b9–b2 находятся в регистре CCPRxL. (что такое скважность, разберем в выпуске рассылки, где будем рассматривать режим ШИМ).
	// n - (CCPxCON) bit 3-0 CCPxM3-CCPxM0: 0000 – модуль CCPx отключен; 0001 – данная комбинация не используется; 0010 – режим сравнения.Инвертируется состояние выхода при возникновении условия сравнения;
	// n - 0011 – данная комбинация не используется; 0100 – режим захвата.Захват по приходу каждого спадающего фронта; 0101 – режим захвата.Захват по приходу каждого нарастающего фронта;
	// n - 0110 – режим захвата.Захват по приходу каждого 4 - го нарастающего фронта; 0111 – режим захвата.Захват по приходу каждого 16 - го нарастающего фронта;
	// n - 1000 – режим сравнения.При возникновении условия сравнения на выходе формируется нарастающий фронт;
	// n - 1001 – режим сравнения.При возникновении условия сравнения на выходе формируется спадающий фронт; 1010 – режим сравнения.Генерация прерывания, при этом, выход CCPx не используется;
	// n - 1011 – режим сравнения.Установка триггера специального события; 11xx – режим ШИМ.

	CCPR1L = 0; //15

	OSCCONbits.IRCF = 0b111;
	OSCTUNEbits.PLLEN = 1;
	Delay_ms(1000);
	Sound(3000,50); // Sound 2000Hz,0.5 sec
	CM1CON = 0b00000111;   //Дисплей не пашет если  нет этого параметра.
	ANSELB = 0b00000000;
	ANSELC = 0b00000000;
	ANSELD = 0b00000000;
	ANSELE = 0b00000000;
	ANSELA = 0b00001011;
	ADCON1 = 0b10000000; //n - При конфигурации портов ввода / вывода необходимо учитывать, что при включении питания или после сброса по default выставляются часть пинов, как аналоговые входы!
	ADCON2 = 0b00000111; //n - Если Вы не используете аналоговую периферию (например, АЦП), то необходимо программно установить в регистре управления АЦП ADCON1 значение,
	//которое переконфигурирует пины на цифровые входы/выходы.


	PMD2 = 0b1110; //Modulator, CAN, Comparator 0-en 1-Dis
	PMD1 = 0b11111110;
	PMD0 = 0b11111100;
	WPUB = 0b11111111;
	SLRE = 0;
	SLRD = 0;
	SLRC = 0;
	SLRB = 0;
	SLRA = 0;
	di();
	IOCB = 0b1111;
	PSA = 0;            //Делитель
	ei();			// start interrupts
	menuId = 0;
	menuId = 0;
	lcd_init();
	lcd_clear();
	initDisplay();

	while(1)
	{

		CloseDoor();
		Encoder_Exe2();
		execEncoder1But();
		inputProcess();
		redrawDisplay();
		Hlopok();
		relay();
		ProgSliv();
		Wring();
		CarefulWash();
		TempReg();
		RegFan();
		pressure();
		Faltering();
		RelayOsc();
		RelayOsc2();
		Poloskanie();
		ControlWater();
		TempFan = TempWater()>>1;
		TempWaterOut = TempMoto()>>1;
		p = pressure();
//		AX = 469-IAX();
		AZ = 342-IAZ();
		press = calcP(p);
		if (press <= 0) press = 0;
		CheckMotor();
		Beeper();
		lcdRedraw();
	}
}

/*
48 об мин крайняя скорость

RE0 - Насос.
RE1 - Клапан 2. I лоток
RE2 - Клапан 1 II лоток.

RD0 - Термометр.
RD1-8 - Display.

RC0 - Звук.
RC1 - Термометр воды
RC2 - ШИМ.
RC3 - Датчик удара.
RC4 - Реверс, реле.
RC5 - Кнопка энкодера.
RC6 - RX.
RC7 - TX.

RB0 - Тахометр.
RB1 - Расходомер.
RB2 - Кнопка пуск.
RB3 - Вентилятор.
RB4 - ТЭН.
RB5 - Kонцевик.
RB6 - Кнопка стоп.
RB7 - Энкодер правый.

RA0 - Датчик давления.
RA6 - Энкодер левый.
Ra2 - Нагреватель дверцы замка.
RA7 - Соленоид.
RA5 - Память.


Значения акселерометра в покое

AX - 467-469
AZ - 339-342
при дисбалансе значения уменьшаются.
*/