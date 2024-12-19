// CpE 3201 Embedded Systems
// Design Project
// Version 1.0 (5-23-2024)

// Authors:
//		Matt Caballero
//		Paul Corsino
//		Pedrito Edzel Cesar Resano Jr.

#include <xc.h>
#include <stdio.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

void delay(int count);

void configLCD(unsigned char cmd);
void clearLCD(void);
void dispSymLCD(unsigned char sym);
void dispStringLCD(unsigned char *string);

int keyToInt(unsigned char port);
unsigned char keyToChar(unsigned char port);
int isArrayEmpty(int array[2][5]);
int isArrayFull(int array[2][5]);

bit myINTF = 0;
bit myTMR0IF = 0;

int MenuState = 0;	// Menu states: 0 - main menu
					//				1 - stock tracker
					//				2 - add id
					//				3 - remove id
					//				4 - browse ids

int IDs[2][5] = {{-1, -1, -1, -1, -1}, {0, 0, 0, 0, 0}};	// -1 means empty



void interrupt ISR() {
	GIE = 0;

	if (INTF) {
		INTF = 0;
		myINTF = 1;
	}
	else if (TMR0IF) {
		TMR0IF 	 = 0;
		myTMR0IF = 1;
	}

	GIE = 0;
}

void delay(int count) {
	int ovrf_count = 0;
	
	while (ovrf_count < count)
	{
		if (myTMR0IF)
		{
			ovrf_count++;
			myTMR0IF = 0;
		}
	}
}


// ---------------------------
// LCD FUNCTIONS
// ---------------------------
void configLCD(unsigned char cmd)
{
	RB5 = 0;
	PORTD = cmd;
	RB7 = 1;
	delay(1);
	RB7 = 0;
}

void clearLCD()
{
	configLCD(0x00); // Clear everything
	configLCD(0x01); // Return cursor home
}

void dispSymLCD(unsigned char sym)
{
	RB5 = 1;
	PORTD = sym;
	RB7 = 1;
	delay(1);
	RB7 = 0;
}

void dispStringLCD(unsigned char *string)
{
	while(*string)
	{
		dispSymLCD(*string++);
	}
}
// ---------------------------
// END OF LCD FUNCTIONS
// ---------------------------



// ---------------------------
// UTILS FUNCTIONS
// ---------------------------
int keyToInt(unsigned char port)
{
	if (port == 0x0D) // "0"
	{
		return 0;
	}
	else if (port == 0x00) // "1"
	{
		return 1;
	}
	else if (port == 0x01) // "2"
	{
		return 2;
	}
	else if (port == 0x02) // "3"
	{
		return 3;
	}
	else if (port == 0x04) // "4"
	{
		return 4;
	}
	else if (port == 0x05) // "5"
	{
		return 5;
	}
	else if (port == 0x06) // "6"
	{
		return 6;
	}
	else if (port == 0x08) // "7"
	{
		return 7;
	}
	else if (port == 0x09) // "8"
	{
		return 8;
	}
	else if (port == 0x0A) // "9"
	{
		return 9;
	}
	else if (port == 0x0C) // *
	{
		return 10;
	}
	else if (port == 0x0E) // #
	{
		return 11;
	}
	else
	{
		return 0x00;
	}
}

unsigned char keyToChar(unsigned char port)
{
	if (port == 0x0D) // "0"
	{
		return 0x30;
	}
	else if (port == 0x00) // "1"
	{
		return 0x31;
	}
	else if (port == 0x01) // "2"
	{
		return 0x32;
	}
	else if (port == 0x02) // "3"
	{
		return 0x33;
	}
	else if (port == 0x04) // "4"
	{
		return 0x34;
	}
	else if (port == 0x05) // "5"
	{
		return 0x35;
	}
	else if (port == 0x06) // "6"
	{
		return 0x36;
	}
	else if (port == 0x08) // "7"
	{
		return 0x37;
	}
	else if (port == 0x09) // "8"
	{
		return 0x38;
	}
	else if (port == 0x0A) // "9"
	{
		return 0x39;
	}
}


int isArrayEmpty(int array[2][5])
{
	for (int i = 0; i < 5; i++)
	{
		if (array[0][i] != -1)
		{
			return 0;
		}	
	}

	return 1;
}

int isArrayFull(int array[2][5])
{
	for (int i = 0; i < 5; i++)
	{
		if (array[0][i] == -1)
		{
			return 0;
		}	
	}

	return 1;
}
// ---------------------------
// END OF UTILS FUNCTIONS
// ---------------------------



void main() 
{
	TRISA = 0x01;
	TRISB = 0x01;
	TRISC = 0x0F;
	TRISD = 0x00;

	PCFG3 = 0;
	PCFG2 = 1;
	PCFG1 = 1;
	PCFG0 = 0;

	OPTION_REG = 0xC4;
	INTF = 0;
	INTE = 1;
	TMR0IF = 0;
	TMR0IE = 1;
	GIE = 1;

	PORTD = 0x00;
	PORTC = 0x00;
	PORTB = 0x00;

	int keyOut = 0;
	int keyWasPressed = 0;
	int screenState = 1;
	int screenScrollState = 1;

	int trackMenu_isInBrowsing = 0;
	int trackMenu_isInCrement = 0;
	int trackMenu_isInTracking = 0;

	int displayFirstMessage = 1;
	int charLimit = 0;

	int IDToTrack = 0;
	int crementType = 0;

	char * messages_MainMenu[4] = {"1: Track Stock", "2: Add Item ID", "3: Remove Item ID", "4: Browse IDs"};
	char buffer [sizeof(int)*8+1] = " ";
	int inputtedID[4] = {0, 0, 0, 0};

	configLCD(0x06); 	// Entry mode increment, shift off
	configLCD(0x0C);	// Display on, cursor on, blink off

	while (1) 
	{ 
		// intial startup for MenuState 0
		if (MenuState == 0) 
		{ 
			keyWasPressed = 1;	
		}		
		while (MenuState == 0) // while we are in the main menu
		{	
			// controls for main menu
			// pressing # will scroll through the options forward
			// pressing * will scroll back
			// 0 will enter the option
			if (myINTF) 
			{
				myINTF = 0;

				if (keyToInt(PORTC) == 11) 
				{	// # is pressed
					screenState++;
					if (screenState > 4) 
					{
						screenState = 4;
					}

					keyWasPressed = 1;
				}
				else if (keyToInt(PORTC) == 10) // * is pressed
				{	
					screenState--;
					if (screenState < 1) 
					{
						screenState = 1;
					}

					keyWasPressed = 1;
				}
				else if (keyToInt(PORTC) == 8) // 8 is pressed
				{	
					switch (screenState) {
						case 1:
							MenuState = 1;
							keyWasPressed = 0;
							break;
						case 2:
							MenuState = 2;
							keyWasPressed = 0;
							break;
						case 3:
							MenuState = 3;
							keyWasPressed = 0;
							break;
						case 4:
							MenuState = 4;
							keyWasPressed = 0;
							break;
						default:
							keyWasPressed = 0;
							break;
					}
				}
			}

			// refresh main menu screen
			if (keyWasPressed) 
			{
				clearLCD(); // clear the LCD screen	
				dispStringLCD((unsigned char *)"Main Menu");
				dispStringLCD((unsigned char *)"            ");
				dispStringLCD((unsigned char *)messages_MainMenu[screenState - 1]);
				keyWasPressed = 0;
			}
		}


		if (MenuState == 1) 
		{ 
			screenScrollState = 1;
			keyWasPressed = 1;
			trackMenu_isInBrowsing = 1;	
		}
		while (MenuState == 1) // while we are in the track stock section
		{	
			// Check if array is empty
			// If empty, show error and quit back to main menu
			if (isArrayEmpty(IDs))
			{
				clearLCD();
				dispStringLCD((unsigned char *)"No Item IDs!");
				delay(288);

				MenuState = 0;
				screenState = 1;
				
				break;
			}

			while (trackMenu_isInBrowsing)
			{
				if (myINTF) 
				{
					myINTF = 0;
	
					if (keyToInt(PORTC) == 11 && screenScrollState < 5) // # is pressed
					{	
						screenScrollState++;
						keyWasPressed = 1;
					}
					else if (keyToInt(PORTC) == 10 && screenScrollState > 1) // * is pressed
					{	
						screenScrollState--;
						keyWasPressed = 1;
					}
					else if (keyToInt(PORTC) == 0) // 0 is pressed
					{	
						MenuState 			   = 0;
						trackMenu_isInBrowsing = 0;
					}
					else if (keyToInt(PORTC) == 8 && IDs[0][screenScrollState - 1] != -1) // 8 was pressed
					{
						IDToTrack = screenScrollState - 1;
						trackMenu_isInBrowsing = 0;
						trackMenu_isInCrement  = 1;
					}
				}
				
				
				if (keyWasPressed)
				{
					clearLCD();
					dispStringLCD((unsigned char *)"Choose ID to track:");	
					dispStringLCD((unsigned char *)"        ");	

					if (IDs[0][screenScrollState - 1] == -1)
					{
						dispStringLCD((unsigned char *)"None");			
					}
					else
					{
						sprintf(buffer, "%d", IDs[0][screenScrollState - 1]);
						dispStringLCD((unsigned char *)"ID ");
						dispStringLCD((unsigned char *)buffer);
					}

					keyWasPressed = 0;
				} 	
			}


			if (trackMenu_isInCrement)
			{
				screenScrollState = 1;
				keyWasPressed     = 1;
				screenScrollState = 1;
			}
			while (trackMenu_isInCrement)
			{
				if (myINTF)
				{
					myINTF = 0;
					if (keyToInt(PORTC) == 11 && screenScrollState < 2)
					{
						screenScrollState++;
						keyWasPressed = 1;						
					}
					else if (keyToInt(PORTC) == 10 && screenScrollState > 1)
					{
						screenScrollState--;
						keyWasPressed = 1;
					}
					else if (keyToInt(PORTC) == 8)
					{
						crementType = screenScrollState - 1;
						trackMenu_isInCrement = 0;
						trackMenu_isInTracking = 1;
					}
					else if (keyToInt(PORTC) == 0)
					{
						MenuState 				= 0;
						trackMenu_isInBrowsing 	= 0;
						trackMenu_isInCrement 	= 0;
						trackMenu_isInTracking 	= 0;
					}
				}
				
				if (keyWasPressed) 
				{
					if (screenScrollState == 1) 
					{
						clearLCD();
						dispStringLCD((unsigned char *)"Count dir.:          Decrease");	
					}
					else if (screenScrollState == 2) 
					{
						clearLCD();
						dispStringLCD((unsigned char *)"Count dir.:          Increase");	
					}
	
					keyWasPressed = 0;
				}
			}

			if (trackMenu_isInTracking)
			{
				sprintf(buffer, "%d", IDs[0][IDToTrack]);
				clearLCD();
				dispStringLCD((unsigned char *)"Tracking...");
				dispStringLCD((unsigned char *)"            ID ");
				dispStringLCD((unsigned char *)buffer);
				dispStringLCD((unsigned char *)" : ");
				sprintf(buffer, "%d", IDs[1][IDToTrack]);
				dispStringLCD((unsigned char *)buffer);
				dispStringLCD((unsigned char *)" stk");
			}
			while (trackMenu_isInTracking)
			{
				if (myINTF)
				{
					myINTF = 0;

					if (keyToInt(PORTC) == 0)
					{
						MenuState 				= 0;
						screenState				= 1;
						trackMenu_isInBrowsing 	= 0;
						trackMenu_isInCrement 	= 0;
						trackMenu_isInTracking 	= 0;						
					}
				}

				if (RA0 == 0)
				{
					if (crementType == 0)
					{
						if (IDs[1][IDToTrack] > 0)
						{
							IDs[1][IDToTrack] -= 1;
						}
					}
					else if (crementType == 1)
					{
						if (IDs[1][IDToTrack] < 1000)
						{
							IDs[1][IDToTrack] += 1;
						}
					}

					sprintf(buffer, "%d", IDs[0][IDToTrack]);
					clearLCD();
					dispStringLCD((unsigned char *)"Tracking...");
					dispStringLCD((unsigned char *)"            ID ");
					dispStringLCD((unsigned char *)buffer);
					dispStringLCD((unsigned char *)" : ");
					sprintf(buffer, "%d", IDs[1][IDToTrack]);
					dispStringLCD((unsigned char *)buffer);
					dispStringLCD((unsigned char *)" stk");

					while (RA0 == 0) { };
				}
			}
		}








		if (MenuState == 2)
		{
			displayFirstMessage = 1;
			charLimit = 0;
		}
		while (MenuState == 2) // we are now in the add item thing
		{	
			if (isArrayFull(IDs))
			{
				clearLCD();
				dispStringLCD((unsigned char *)"ID list full!");
				delay(288);

				MenuState = 0;
				screenState = 2;
				
				break;
			}

			if (displayFirstMessage == 1)
			{
				clearLCD();
				dispStringLCD((unsigned char *)"Add ID:  ");
				displayFirstMessage = 0;
			}

			if (myINTF)
			{
				myINTF = 0;

				if (keyToInt(PORTC) == 11)		// # is pressed (enter button)
				{	
					if (charLimit == 0)	// if entry is empty, whine and go back to main menu
					{
						clearLCD();
						dispStringLCD((unsigned char *)"Entry empty!");
						delay(288);
						MenuState = 0;
						screenState = 2;
					}
					else
					{
						int thous	= inputtedID[3] * 1000;
						int hund	= inputtedID[2] * 100;
						int tenth	= inputtedID[1] * 10;
						int ones	= inputtedID[0];

						int newID = thous + hund + tenth + ones;

						for (int b = 0; b < 5; b++)
						{
							if (IDs[0][b] == -1)
							{
								IDs[0][b] = newID;
								break;
							}
						}

						clearLCD();
						dispStringLCD((unsigned char *)"ID added!");
						delay(288);

						for (int a = 0; a < 4; a++)
						{
							inputtedID[a] = 0;
						}
						MenuState = 0;
						screenState = 2;
					}
				}
				else if (keyToInt(PORTC) == 10)
				{	
					for (int a = 0; a < 4; a++)
					{
						inputtedID[a] = 0;
					}

					MenuState = 0;
					screenState = 2;
				}
				else 
				{
					if (charLimit < 4)
					{
						dispSymLCD(keyToChar(PORTC));

						for (int k = 4; k > 0; k--)
						{
							inputtedID[k] = inputtedID[k - 1];
						}

						inputtedID[0] = keyToInt(PORTC);

						charLimit++;
					}
				}
			}
		}



		if (MenuState == 3) { 
			keyWasPressed = 1;	
		}
		while (MenuState == 3) // while we are in the remove item ids section
		{	
			if (isArrayEmpty(IDs))
			{
				clearLCD();
				dispStringLCD((unsigned char *)"No Item IDs!");
				delay(288);

				MenuState = 0;
				screenState = 3;
				
				break;
			}

			if (myINTF) 
			{
				myINTF = 0;

				if (keyToInt(PORTC) == 11 && screenScrollState < 5) // # is pressed
				{	
					screenScrollState++;
					keyWasPressed = 1;
				}
				else if (keyToInt(PORTC) == 10 && screenScrollState > 1) // * is pressed
				{	
					screenScrollState--;
					keyWasPressed = 1;
				}
				else if (keyToInt(PORTC) == 8 && IDs[0][screenScrollState - 1] != -1)
				{
					IDs[0][screenScrollState - 1] = -1;
					IDs[1][screenScrollState - 1] = 0;

					clearLCD();
					dispStringLCD((unsigned char *)"ID removed!");
					delay(288);

					MenuState   	  = 0;
					screenState 	  = 3;
					screenScrollState = 1;
				}
				else if (keyToInt(PORTC) == 8) // 8 is pressed
				{	
					MenuState   	  = 0;
					screenState 	  = 3;
					screenScrollState = 1;
				}
			}


			if (keyWasPressed) 
			{
				clearLCD();
				dispStringLCD((unsigned char *)"Remove Item ID");
				dispStringLCD((unsigned char *)"       ");	

				if (IDs[0][screenScrollState - 1] == -1)
				{
					dispStringLCD((unsigned char *)"None");			
				}
				else
				{
					sprintf(buffer, "%d", IDs[0][screenScrollState - 1]);
					dispStringLCD((unsigned char *)"ID ");
					dispStringLCD((unsigned char *)buffer);
				}

				dispStringLCD((unsigned char *)" : ");
				sprintf(buffer, "%d", IDs[1][screenScrollState - 1]);
				dispStringLCD((unsigned char *)buffer);
				dispStringLCD((unsigned char *)" stk");	

				keyWasPressed = 0;
			}
		}
			


		if (MenuState == 4) 
		{
			screenScrollState = 1; 
			keyWasPressed = 1;	
		}
		while (MenuState == 4) // while we are in the browse item ids section	
		{	
			// check if id list is empty
			// throw a tantrum and go back to main menu if empty
			if (isArrayEmpty(IDs))
			{
				clearLCD();
				dispStringLCD((unsigned char *)"No Item IDs!");	
				delay(288);
				MenuState = 0;
				break;			
			}

			if (myINTF) 
			{
				myINTF = 0;

				if (keyToInt(PORTC) == 11 && screenScrollState < 5) // # is pressed
				{	
					screenScrollState++;
					keyWasPressed = 1;
				}
				else if (keyToInt(PORTC) == 10 && screenScrollState > 1) // * is pressed
				{	
					screenScrollState--;
					keyWasPressed = 1;
				}
				else if (keyToInt(PORTC) == 0) // 0 is pressed
				{	
					MenuState 	      = 0;
					screenState 	  = 4;
					screenScrollState = 1;
				}
			}


			if (keyWasPressed) 
			{
				clearLCD();
				dispStringLCD((unsigned char *)"Browse Item ID");
				dispStringLCD((unsigned char *)"       ");	

				if (IDs[0][screenScrollState - 1] == -1)
				{
					dispStringLCD((unsigned char *)"None");			
				}
				else
				{
					sprintf(buffer, "%d", IDs[0][screenScrollState - 1]);
					dispStringLCD((unsigned char *)"ID ");
					dispStringLCD((unsigned char *)buffer);
				}

				dispStringLCD((unsigned char *)" : ");
				sprintf(buffer, "%d", IDs[1][screenScrollState - 1]);
				dispStringLCD((unsigned char *)buffer);
				dispStringLCD((unsigned char *)" stk");	

				keyWasPressed = 0;
			}
		}
	}
}