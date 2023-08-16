// I2c.cpp: implementation of the I2c class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "I2c.h"
#include <conio.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

I2c::I2c()
{
	InitChipCode();				//Initialiseer chipcodes
	SelectI2CprinterPort(1);	//Select LPT1 on mainboard
	SetDelay(20);		//I2C-bus speed control factor
	I2CBusNotBusy();

  // Initialise the DAC Channels
  OutputDACchannel(1, 0);
  OutputDACchannel(2, 0);
  OutputDACchannel(3, 0);
  OutputDACchannel(4, 0);
  OutputDACchannel(5, 0);
  OutputDACchannel(6, 0);
  OutputDACchannel(7, 0);
  OutputDACchannel(8, 0);
  
  // Initialise the DA channel
  OutputDAchannel(1,0);

}

I2c::~I2c()
{
	I2CBusNotBusy();
}


//	**************************************************
//	* PROCEDURES OM KANALEN TE LEZEN EN TE SCHRIJVEN *
//	**************************************************
	
void I2c::SetDAC(int Channel_no,int Waarde)
{
	DAC[Channel_no] = Waarde;
}
	

int I2c::GetDAC(int Channel_no)
{
	return DAC[Channel_no];
}


//	******************************
//	* RADIX CONVERSION FUNCTIONS *
//	******************************

//	Conversion of decimal number (0...255) to 8 bit binary string.
//	--------------------------------------------------------------

void I2c::DecToBin(int DecNumber, char *Bin)
{
	int i;
	char *one = "1", *zero = "0";

	DecNumber <<= 8;				//Shift dec. number 8 bits left
	if (DecNumber < 0)
		strcpy(Bin, one);
	else
		strcpy(Bin, zero);
	DecNumber <<= 1;
	for (i=1; i<8; i++)
	{
		if (DecNumber < 0)			//Write highest bit into bin string
			strcat(Bin, one);
		else
			strcat(Bin, zero);
		DecNumber <<= 1;			//Shift decimal number 1 bit left
	};
}


//	Conversion of binary string (max 8 bit) into decimal number.
//	------------------------------------------------------------

int I2c::BinToDec(char *BinNumber)
{
	int i, Weight, dec;

	Weight = 1;						//Set weight factor at lowest bit
	dec = 0;						//Reset decimal number
	for (i=strlen(BinNumber)-1;i>=0;i--)
	{												//Convert all bits from bin. string
		dec = dec + (BinNumber[i] == '1') * Weight;	//If bit=1 then add weigth factor
		Weight = Weight << 1;						//Multiply weight factor by 2
	};
	return dec;
}


//	Conversion of decimal number (0...255) to hexadecimal string (2 digits).
//	------------------------------------------------------------------------

void I2c::DecToHex(int DecNumber, char *Hex)
{
	strcpy(Hex,"0x");						//Set length hex string at 2
	itoa(DecNumber / 16, Hex+2, 16);		//MSD = dec. number / 16
	itoa(DecNumber % 16, Hex+3, 16);		//LSD = remainder dec number / 16
}


//	Conversion of hexadecimal string (2 digits) to decimal number.
//	--------------------------------------------------------------

int I2c::HexToDec(char *HexNumber)
{
	int Strlength, dech, decl;

	Strlength = strlen(HexNumber);			//Calculate length of hex string
	switch(HexNumber[Strlength-1])
	{
		case 'a': case 'A': decl = 10; break;
		case 'b': case 'B': decl = 11; break;
		case 'c': case 'C': decl = 12; break;
		case 'd': case 'D': decl = 13; break;
		case 'e': case 'E': decl = 14; break;
		case 'f': case 'F': decl = 15; break;
		default: decl = HexNumber[Strlength-1]-'0';
	};
	
	if (Strlength == 2)
	{ 			//If second digit exist convert it
		switch(HexNumber[0])
		{
			case 'a': case 'A': dech = 10; break;
			case 'b': case 'B': dech = 11; break;
			case 'c': case 'C': dech = 12; break;
			case 'd': case 'D': dech = 13; break;
			case 'e': case 'E': dech = 14; break;
			case 'f': case 'F': dech = 15; break;
			default: dech = HexNumber[0]-'0';
		};
	};
	return ((dech << 4) + decl);				//Shift MSD 4 bit left & add to LSD
}

//	*****************************************
//	* UPDATE IOdata & IO ARRAY PROCEDURES *
//	*****************************************

//	Update the 'IOdata' & 'IO' array with data for selected chip.
//	-------------------------------------------------------------

void I2c::UpdateIOdataArray(int Chip_no, int Data)
{

	//Update 'IOdata' array
	IOdata[Chip_no] = (IOdata[Chip_no] & IOconfig[Chip_no]);
	IOdata[Chip_no] = IOdata[Chip_no] | (Data & (~IOconfig[Chip_no]));

	UpdateIOArray(Chip_no);
}
//	Clear the 'IOdata' & 'IO' array from the selected chip.
//	-------------------------------------------------------

void I2c::ClearIOdataArray(int Chip_no)
{
	//Update 'IOdata' array
	IOdata[Chip_no] = IOdata[Chip_no] & IOconfig[Chip_no];
	
	UpdateIOArray(Chip_no);
}

//	Setthe 'IOdata' & 'IO' array from the selected chip.
//	----------------------------------------------------

void I2c::SetIOdataArray(int Chip_no)
{
	//Update 'IOdata' array
	IOdata[Chip_no] = IOdata[Chip_no] | ( ~IOconfig[Chip_no]);
	UpdateIOArray(Chip_no);
}

//	Set the 'IOdata' & 'IO' array from the selected channel.
//	--------------------------------------------------------

void I2c::SetIOchArray(int Channel_no)
{
	int Chip_no, Channel, Data;

	Chip_no = (Channel_no - 1) / 8;		//Calculate chip no
	Channel = (Channel_no - 1) % 8;		//Calculate channel of IOchip
	Data = IOdata[Chip_no] | (0x01 << Channel);	//Data for correct IOchannel to set
	UpdateIOdataArray(Chip_no, Data);		//Update IOdata & IO array
}

//	Clear the 'IOdata' & 'IO' array from the selected channel.
//	----------------------------------------------------------

void I2c::ClearIOchArray(int Channel_no)
{
	int Chip_no, Channel, Data;

	Chip_no = (Channel_no - 1) / 8;		//Calculate chip no
	Channel = (Channel_no - 1) % 8;		//Calculate channel of IOchip
	Data = IOdata[Chip_no] & (~(0x01 << Channel));	//Data for IOchannel to clear
	UpdateIOdataArray(Chip_no, Data);		//Update IOdata & IO array
}


//	*********************
//	* OUTPUT PROCEDURES *
//	*********************

//	Output data to selected IO-port and update the 'IOdata' & 'IO' array.
//	---------------------------------------------------------------------

void I2c::IOoutput(int Chip_no, int Data)
{
	Data = (~Data) | IOconfig[Chip_no];	//Mask input channels

	GenerateStartCondition();

	//Serial output of Chipcode
	OutputByteToI2C(IOchipCode[Chip_no]);

	GenerateAcknowledge();

	//Serial output of data
	OutputByteToI2C(Data);

	GenerateAcknowledge();

	GenerateStopCondition();

	//Update 'IOdata' array
	IOdata[Chip_no] = (IOdata[Chip_no] & IOconfig[Chip_no]) | (~Data);

	UpdateIOArray(Chip_no);
}


//	Update all IO channels with value stored into the 'IOdata' array.
//	-----------------------------------------------------------------

void I2c::UpdateAllIO()
{
	int Chip_no;

	for (Chip_no = 0; Chip_no <= MaxIOchip; Chip_no++)
		IOoutput(Chip_no, IOdata[Chip_no]);	//Update all IO-ports
}

//	Clear all IO channels and update the 'IOdata' & 'IO' array.
//	-----------------------------------------------------------

void I2c::ClearAllIO()
{
	int Chip_no;

	for (Chip_no = 0; Chip_no <= MaxIOchip; Chip_no++)
		IOoutput(Chip_no, 0);	//Clear all IO-port
}

//	Set all IO channels and update the 'IOdata' & 'IO' array.
//	---------------------------------------------------------

void I2c::SetAllIO()
{
	int Chip_no;

	for (Chip_no = 0; Chip_no <= MaxIOchip; Chip_no++)
		IOoutput(Chip_no, 0x0FF);	//Set all IO-port
}


//	Update IO chip with value stored into the 'IOdata' array.
//	---------------------------------------------------------

void I2c::UpdateIOchip(int Chip_no)
{
	IOoutput(Chip_no, IOdata[Chip_no]);	//Update IO port
}


//	Clear IO chip and update the 'IOdata' & 'IO' array.
//	---------------------------------------------------

void I2c::ClearIOchip(int Chip_no)
{
	IOoutput(Chip_no, 0);	//Clear IO-port
}


//	Set IO chip and update the 'IOdata' & 'IO' array.
//	-------------------------------------------------

void I2c::SetIOchip(int Chip_no)
{
	IOoutput(Chip_no, 0x0FF);	//Set IO-port
}


//	Set one IO channel and update the 'IOdata' & 'IO' array.
//	--------------------------------------------------------

void I2c::SetIOchannel(int Channel_no)
{
	int Chip_no, Channel, Data;

	Chip_no = (Channel_no - 1) / 8;		//Calculate chip no
	Channel = (Channel_no - 1) % 8;		//Calculate channel of IOchip
	Data = IOdata[Chip_no] | (0x01 << Channel);	//Data for correct IOchannel to set
	IOoutput(Chip_no, Data);			//Set IOchannel
}

//	Clear one IO channel and update the 'IOdata' & 'IO' array.
//	----------------------------------------------------------

void I2c::ClearIOchannel(int Channel_no)
{
	int Chip_no, Channel, Data;

	Chip_no = (Channel_no - 1) / 8;		//Calculate chip no
	Channel = (Channel_no - 1) % 8;		//Calculate channel of IOchip
	Data = IOdata[Chip_no] & (~(0x01 << Channel));	//Data for IOchannel to clear
	IOoutput(Chip_no, Data);			//Clear IOchannel
}


//	*********************
//	* INPUT PROCEDURES  *
//	*********************

//	Read IO chip and update the 'IOdata' & 'IO' array.
//	--------------------------------------------------

void I2c::ReadIOchip(int Chip_no)
{
	GenerateStartCondition();

	//Serial output of Chipcode for readmode
	OutputByteToI2C(IOchipCode[Chip_no] | 0x01);

	GenerateAcknowledge();

	//Serial input of ChipData
	IOdata[Chip_no] = ~InputByteFromI2C();

	GenerateAcknowledge();

	GenerateStopCondition();

	UpdateIOArray(Chip_no);
}


//	Read all IO channels and update the 'IOdata' & 'IO' array.
//	----------------------------------------------------------

void I2c::ReadAllIO()
{
	int Chip_no;

	for (Chip_no = 0; Chip_no <= MaxIOchip; Chip_no++)
		ReadIOchip(Chip_no);	//Read all IO ports
}


//	Read one IO channels and update the 'IOdata' & 'IO' array.
//	----------------------------------------------------------

void I2c::ReadIOchannel(int Channel_no)
{
	int Chip_no;

	Chip_no = (Channel_no - 1) / 8;	//Calculate chip no
	ReadIOchip(Chip_no);			//Read IO port
}

//	*******************************
//	* IO CONFIGURATION PROCEDURES *
//	*******************************

//	Config all IO-ports as inputs (1 = Input mode / 0 = Output Mode).
//	-----------------------------------------------------------------

void I2c::ConfigAllIOasInput()
{
	int Chip_no;

	for (Chip_no = 0; Chip_no <= MaxIOchip; Chip_no++)
	{
		IOconfig[Chip_no] = 0;		//Config all IO-ports as outputs
		ClearIOchip(Chip_no);		//Clear all IO-ports
		IOconfig[Chip_no] = 0xFF;	//Config all IO-ports as inputs
		ReadIOchip(Chip_no);		//Update 'IOdata' & 'IO' array
	}
}


//	Config all IO-ports as outputs (1 = Input mode / 0 = Output Mode).
//	-----------------------------------------------------------------

void I2c::ConfigAllIOasOutput()
{
	int Chip_no;

	for (Chip_no = 0; Chip_no <= MaxIOchip; Chip_no++)
		IOconfig[Chip_no] = 0x00;	//Config all IO-ports as outputs
	ClearAllIO();					//Clear all IO-ports
}

//	Config one IO-port as input (1 = Input mode / 0 = Output mode).
//	---------------------------------------------------------------

void I2c::ConfigIOchipAsInput(int Chip_no)
{
	IOconfig[Chip_no] = 0;		//Config IO-port as output
	ClearIOchip(Chip_no);		//Clear  IO-port
	IOconfig[Chip_no] = 0xFF;	//Config IO-port as inputs
	ReadIOchip(Chip_no);		//Update 'IOdata' & 'IO' array
}


//	Config one IO-port as output (1 = Input mode / 0 = Output mode).
//	----------------------------------------------------------------

void I2c::ConfigIOchipAsOutput(int Chip_no)
{
	IOconfig[Chip_no] = 0x00;	//IO-port as outputs
	ClearIOchip(Chip_no);		//Clear IO-port
}

//	Config one IO-channel as input (1 = Input mode / 0 = Output mode).
//	------------------------------------------------------------------

void I2c::ConfigIOchannelAsInput(int Channel_no)
{
	int Chip_no, Channel;

	Chip_no = (Channel_no - 1) / 8;		//Calculate chip no
	Channel = (Channel_no - 1) % 8;		//Calculate channel from IOchip
	IOconfig[Chip_no] = IOconfig[Chip_no] & (~(0x01 << Channel)); //Set IOchannel as output
	ClearIOchannel(Channel_no);			//Clear IO-channel
	IOconfig[Chip_no] = IOconfig[Chip_no] | (0x01 << Channel); //Set IOchannel as input
	ReadIOchannel(Channel_no);			//Update 'IOdata' & 'IO' array
}

//	Config one IO-channel as output (1 = Input mode / 0 = Output mode).
//	-------------------------------------------------------------------

void I2c::ConfigIOchannelAsOutput(int Channel_no)
{
	int Chip_no, Channel;

	Chip_no = (Channel_no - 1) / 8;		//Calculate chip no
	Channel = (Channel_no - 1) % 8;		//Calculate channel from IOchip
	IOconfig[Chip_no] = IOconfig[Chip_no] & (~(0x01 << Channel)); //Set IOchannel as output
	ClearIOchannel(Channel_no);			//clear IO-channel
}

//	**********************************
//	* 6 BIT DAC CONVERTER PROCEDURES *
//	**********************************

//	Set selected DAC channel by given value. The 'DAC' array will be updated.
//	-------------------------------------------------------------------------

void I2c::OutputDACchannel(int Channel_no, int Data)
{
	
	Data = (Data > 63) ? 63: Data;	//If overflow  set at maximum (63)
	DAC[Channel_no] = Data;			//Update 'DAC' array

	GenerateStartCondition();

	//Serial output of Chipcode
	OutputByteToI2C(DACchipCode[(Channel_no - 1) / 8]);

	GenerateAcknowledge();

	//Serial output of selected channel
	OutputByteToI2C(0xF0 | ((Channel_no - 1) % 8));

	GenerateAcknowledge();

	//Serial output of Data
	OutputByteToI2C(Data);

	GenerateAcknowledge();

	GenerateStopCondition();
}

//	Update the eight DAC channels from the selected chip by the 'DAC' array.
//	------------------------------------------------------------------------

void I2c::UpdateDACchip(int Chip_no)
{
	int Channel, k;
	
	GenerateStartCondition();

	//	Serial output of Chipcode
	OutputByteToI2C(DACchipCode[Chip_no]);

	GenerateAcknowledge();

	//Serial output of channel 0 from selected chip
	OutputByteToI2C(0x00);

	GenerateAcknowledge();

	//Update the 8 channels of the selected chip
	Channel = Chip_no * 8;				//Calculate DAC channel
	for (k = 1; k <= 8; k++)
	{
		//Serial output of DAC Data
		DAC[Channel + k] = (DAC[Channel+k] > 63) ? 63 : DAC[Channel+k];	//Overflow
		OutputByteToI2C(DAC[Channel + k]);

		GenerateAcknowledge();
	}
	GenerateStopCondition();
}


//	Update DAC channels by the 'DAC' array.
//	--------------------------------------

void I2c::UpdateDACchannel(int Channel_no)
{
	OutputDACchannel(Channel_no, DAC[Channel_no]); //Update DAC channel
}

//	Set selected DAC channel at minimum (0). The 'DAC' array will be updated.
//	-------------------------------------------------------------------------

void I2c::ClearDACchannel(int Channel_no)
{
	OutputDACchannel(Channel_no, 0); //Set selected DAC channel at 0
}


//	Set selected DAC channel at maximum (63). The 'DAC' array will be updated.
//	--------------------------------------------------------------------------

void I2c::SetDACchannel(int Channel_no)
{
	OutputDACchannel(Channel_no,63);  //Set selected DAC channel at 63
}

//	Set the eight DAC channels from the selected chip at minimum (0) and update the 'DAC' array.
//	--------------------------------------------------------------------------------------------

void I2c::ClearDACchip(int Chip_no)
{
	int i, Channel;

	Channel = Chip_no * 8;
	for (i = 1; i <= 8; i++)
		DAC[Channel + i] = 0;	//Clear 8 channels of selected chip
	UpdateDACchip(Chip_no);		//Update DAC chip
}

//	Set the eight DAC channels from the selected chip at maximum (63) and update the 'DAC' array.
//	---------------------------------------------------------------------------------------------

void I2c::SetDACchip(int Chip_no)
{
	int i, Channel;

	Channel = Chip_no * 8;
	for (i = 1; i <= 8; i++)
		DAC[Channel + i] = 63;	//Set 8 channels selected chip max
	UpdateDACchip(Chip_no);		//Update DAC chip
}

//	Update all DAC channels by the 'DAC' array.
//	-------------------------------------------

void I2c::UpdateAllDAC()
{
	int Chip_no;

	for (Chip_no = 0; Chip_no <= MaxIOcard; Chip_no++)
		UpdateDACchip(Chip_no);	//Update all DAC channels
}

//	Set all DAC channels at minimum (0) and update the 'DAC' array.
//	---------------------------------------------------------------

void I2c::ClearAllDAC()
{
	int Channel_no;

	for (Channel_no = 1; Channel_no <= MaxDACchannel; Channel_no++)
		DAC[Channel_no] = 0x00;	//Clear the 'DAC' array
	UpdateAllDAC();				//Update all DAC channels
}


//	Set all DAC channels at maximum (63) and update the 'DAC' array.
//	----------------------------------------------------------------

void I2c::SetAllDAC()
{
	int Channel_no;

	for (Channel_no = 1; Channel_no <= MaxDACchannel; Channel_no++)
		DAC[Channel_no] = 63;	//Set 'DAC' array at max.(63)
	UpdateAllDAC();			//Update all DAC channels
}


//	*********************************
//	* 8 BIT DA CONVERTER PROCEDURES *
//	*********************************

//	Set selected DA channel by given value. The 'DA' array will be updated.
//	-----------------------------------------------------------------------

void I2c::OutputDAchannel(int Channel_no, int Data)
{
	DA[Channel_no] = Data;	//Store DA data into DA array

	GenerateStartCondition();

	//Serial output of Chipcode
	OutputByteToI2C(ADDAchipCode[Channel_no-1]);

	GenerateAcknowledge();

	//Serial output of ControlByte
	OutputByteToI2C(0x40);

	GenerateAcknowledge();
	
	//Serial output of DA Data
	OutputByteToI2C(Data);

	GenerateAcknowledge();

	GenerateStopCondition();
}


//	Update DA channels by the 'DA' array .
//	--------------------------------------

void I2c::UpdateDAchannel(int Channel_no)
{
	OutputDAchannel(Channel_no, DA[Channel_no]);	//Update DA channel
}

//	Set selected DA channel at minimum (0). The 'DA' array will be updated.
//	-----------------------------------------------------------------------

void I2c::ClearDAchannel(int Channel_no)
{
	OutputDAchannel(Channel_no, 0);	//Set selected DA channel at 0
}

//	Set selected DA channel at maximum (255). The 'DA' array will be updated.
//	-------------------------------------------------------------------------

void I2c::SetDAchannel(int Channel_no)
{
	OutputDAchannel(Channel_no, 255);	//Set selected DA channel at 255
}

//	Update all DA channels by the 'DA' array .
//	------------------------------------------

void I2c::UpdateAllDA()
{
	int Channel_no;

	for (Channel_no = 1; Channel_no <= MaxDAchannel; Channel_no++)
		OutputDAchannel(Channel_no, DA[Channel_no]);	//Update all DA channels
}

//	Set all DA channels at minimum (0). The 'DA' array will be cleared.
//	-------------------------------------------------------------------

void I2c::ClearAllDA()
{
	int channel_no;

	for (channel_no = 1; channel_no <= MaxDAchannel; channel_no++)
		OutputDAchannel(channel_no, 0);	//Set all DA channels at 0
}

//	Set all DA channels at maximum (255). The 'DA' array will be updated.
//	---------------------------------------------------------------------

void I2c::SetAllDA()
{
	int Channel_no;

	for (Channel_no = 1; Channel_no <= MaxDAchannel; Channel_no++ )
		OutputDAchannel(Channel_no, 255);			//Set all DA channels at 255
}


//	*********************************************
//	* 8 bit AD CONVERTER FUNCTIONS & PROCEDURES *
//	*********************************************

//	Read one AD channel. The result will be stored into the 'AD' array.
//	-------------------------------------------------------------------

void I2c::ReadADchannel(int Channel_no)
{
	int ChipCode;

	ChipCode = ADDAchipCode[(Channel_no - 1) / 4];	//Calculate chipcode

	GenerateStartCondition();

	//Serial output of Chipcode
	OutputByteToI2C(ChipCode);

	GenerateAcknowledge();
	
	//Serial output of selected channel
	OutputByteToI2C(0x40 | ((Channel_no - 1) % 4));

	GenerateAcknowledge();

	GenerateStopCondition();

	GenerateStartCondition();

	//Serial output of Chipcode for readmode
	OutputByteToI2C(ChipCode | 0x01);

	GenerateAcknowledge();

	//Serial input of previous converted byte
	InputByteFromI2C();

	GenerateAcknowledgeFromMaster();

	//Serial input of current converted byte
	
	AD[Channel_no] = InputByteFromI2C();

	GenerateAcknowledge();

	GenerateStopCondition();
}

//	Read four AD channels from one chip. The result will be stored into the 'AD' array.
//	-----------------------------------------------------------------------------------

void I2c::ReadADchip(int Chip_no)
{
	int Channel, k;

	Channel = Chip_no * 4 + 1;	//Calculate first channel

	GenerateStartCondition();

	//Serial output of Chipcode
	OutputByteToI2C(ADDAchipCode[Chip_no]);

	GenerateAcknowledge();

	//Serial output of controlbyte for autoincrement

	OutputByteToI2C(0x44);

	GenerateAcknowledge();

	GenerateStopCondition();

	GenerateStartCondition();

	//Serial output of Chipcode for readmode
	OutputByteToI2C(ADDAchipCode[Chip_no] | 0x01);

	GenerateAcknowledge();

	InputByteFromI2C();

	//Read 4 AD channels
	for (k = 0; k <= 3; k++)
	{
		GenerateAcknowledgeFromMaster();
		
		//Serial input of current converted byte
		AD[Channel + k] = InputByteFromI2C();
	};

	GenerateAcknowledge();

	GenerateStopCondition();
}

//	Read all AD channels. The result will be stored into the 'AD' array.
//	--------------------------------------------------------------------

void I2c::ReadAllAD()
{
	int Chip_no;
					//Scanning of all AD chips
	for (Chip_no = 0; Chip_no <= MaxIOcard; Chip_no++)
		ReadADchip(Chip_no);	//Read 4 AD channels from chip
}


//	****************************
//	* COMPLETE CARD PROCEDURES *
//	****************************

//	Read all IO's & AD's and update 'IOdata', 'IO' & 'AD' arrays.
//	-------------------------------------------------------------

void I2c::ReadAll()
{
	ReadAllIO();	//Read all IO ports
	ReadAllAD();	//Read all DA channels
}

//	Read all IO's & AD's from one card and update 'IOdata', 'IO' & 'AD' arrays.
//	---------------------------------------------------------------------------

void I2c::ReadCard(int Card_no)
{
	int Chip_no;

	Chip_no = Card_no * 2;		//Calculate IO chip no
	ReadIOchip(Chip_no);		//Read first  8 IO ports
	ReadIOchip(Chip_no + 1);	//Read second 8 IO ports
	ReadADchip(Card_no);		//Read 4 DA channels
}

//	Update all IO's, DAC's & DA's and update 'IOdata', 'IO', 'DAC' & 'DA' arrays.
//	-----------------------------------------------------------------------------

void I2c::UpdateAll()
{
	UpdateAllIO();	//Update all IO ports
	UpdateAllDAC();	//Update all DAC channels
	UpdateAllDA();	//Update all DA channels
}

//	Update all IO DAC & DA's from one card and update 'IOdata' 'IO' 'DAC' 'DA' arrays.
//	----------------------------------------------------------------------------------

void I2c::UpdateCard(int Card_no)
{
	int Chip_no;

	Chip_no = Card_no * 2;			//Calculate chip no
	UpdateIOchip(Chip_no);			//Update first  8 IO ports
	UpdateIOchip(Chip_no + 1);		//Update second 8 IO ports
	UpdateDACchip(Card_no);			//Update 8 DAC channels
	UpdateDAchannel(Card_no + 1);	//Update DA channel
}


//	**********************
//	* I2C-BUS CONDITIONS *
//	**********************

//	SDA IN  =  SELECT    (pin 13) = bit 4 of STATUSPORT
//	SDA OUT = -AUTOFEED  (pin 14) = bit 1 of CONTROLPORT
//	SCL OUT = -SELECT IN (pin 17) = bit 3 of CONTROLPORT bits 0,4...7 = 0 / bit 2 = 1  of CONTROLPORT

//	Set I2C-bus at non actif.
//	-------------------------

void I2c::I2CBusNotBusy()
{
  MyOutput(ControlPort, 0x04);
}

//	Select I2C communication printerport.
//	-------------------------------------

void I2c::SelectI2CprinterPort(int Printer_no)
{
	switch (Printer_no)
	{
		case 0: //lpt on monochroom display adapter
			StatusPort  = 0x03BD;
			ControlPort = 0x03BE;
			break;
		case 1:	//lpt1 on mainboard
			StatusPort  = 0x0379;
			ControlPort = 0x037A;
			break;
		case 2:	//lpt2 on mainboard
			StatusPort  = 0x0279;
			ControlPort = 0x027A;
			break;
	};
}

void I2c::InitChipCode()
{
	//Initialiseer chipcode
	IOchipCode[0] = 0x70;
	IOchipCode[1] = 0x72;
	IOchipCode[2] = 0x74;
	IOchipCode[3] = 0x76;
	IOchipCode[4] = 0x78;
	IOchipCode[5] = 0x7A;
	IOchipCode[6] = 0x7C;
	IOchipCode[7] = 0x7E;

	//Initialiseer DACchipCode
	DACchipCode[0] = 0x40;
	DACchipCode[1] = 0x42;
	DACchipCode[2] = 0x44;
	DACchipCode[3] = 0x46;
	
	//Initialiseer ADDAchipCode
	ADDAchipCode[0] = 0x90;
	ADDAchipCode[0] = 0x92;
	ADDAchipCode[0] = 0x94;
	ADDAchipCode[0] = 0x96;
}

void I2c::SetDelay(int Delay)
{
	CTIMER Dummy;

	I2CbusDelay = (int)((float)Delay*(Dummy.OutputFreq() / 1000000.0));
}

int  I2c::GetDelay(void)
{
  CTIMER Dummy;
	
  //Convert to microseconds
	return (int)((float)I2CbusDelay / (Dummy.OutputFreq() / 1000000.0)+0.5);
}

void I2c::MyDelay(void)
{
	CTIMER DelayTime;

	DelayTime.Start();
	DelayTime.Stop();
	while (DelayTime.OutputTicks() < I2CbusDelay)
	{
		DelayTime.Stop();
	}
}


//*************************************************
// I2C Output functions
//*************************************************

void I2c::GenerateStartCondition(void)
{
	MyOutput(ControlPort, 0x06);
	MyOutput(ControlPort, 0x0E);   
}

void I2c::GenerateAcknowledge(void)
{
	MyOutput(ControlPort, 0x0C);
	MyOutput(ControlPort, 0x04);
	MyOutput(ControlPort, 0x0C);
}


void I2c::GenerateStopCondition(void)
{
	MyOutput(ControlPort, 0x0E);
	MyOutput(ControlPort, 0x06);
	MyOutput(ControlPort, 0x04);
}

void I2c::GenerateAcknowledgeFromMaster(void)
{
	MyOutput(ControlPort, 0x0E);
	MyOutput(ControlPort, 0x06);
	MyOutput(ControlPort, 0x0E);
	MyOutput(ControlPort, 0x0C);
}

void I2c::OutputByteToI2C(int DataByte)
{
  int BitLoop;
  int cport;
 
  for (BitLoop = 0; BitLoop < 8; BitLoop++) 
  {
		cport = (DataByte & MSB8) ? 0x0C: 0x0E;
		MyOutput(ControlPort, cport);
		cport = cport & 0x07;
		MyOutput(ControlPort, cport);
		DataByte <<= 1;
		cport = cport | 0x08;
		MyOutput(ControlPort, cport);
	};
}

int I2c::InputByteFromI2C(void)
{
  int BitLoop;
  int DataByte = 0;
  
  for (BitLoop = 0; BitLoop < 8; BitLoop++) 
  {
		DataByte <<= 1;
		MyOutput(ControlPort, 0x04);
		if (MyInput(StatusPort) & 0x10) 
    {
      DataByte |= 0x0001;
    }
		MyOutput(ControlPort, 0x0C);
  }
  return DataByte;
}

void I2c::UpdateIOArray(int Chip_no)
{
  int Start_Channel;
  int Channel;

	/*Update 'IO' array*/
	Start_Channel = Chip_no * 8 + 1;	/*Calculate start channel*/
	for (Channel = 0; Channel <= 7; Channel++)	/*Test status 8 ch. of the IOchip*/
  {
		IO[Start_Channel+Channel] = ((IOdata[Chip_no] & (0x01 << Channel)) != 0);
  }
}


//************************************************
//  Lowest level interface routines, change for NT
//************************************************

int I2c::MyInput(unsigned short PortNumber)
{
	return (unsigned char)_inpw(PortNumber);
}

void I2c::MyOutput(unsigned short PortNumber, int Value)
{
	_outp(PortNumber,(unsigned short) Value);
	MyDelay();
}
