// I2c.h: interface for the I2c class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_I2C_H__6544F240_EECD_11D3_9E38_444553540000__INCLUDED_)
#define AFX_I2C_H__6544F240_EECD_11D3_9E38_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#endif // !defined(AFX_I2C_H__6544F240_EECD_11D3_9E38_444553540000__INCLUDED_)
/* I2C.H */

#if !defined(MaxIOcard)
#define MaxIOcard 0
#endif

#if !defined(MaxIOchip)
#define MaxIOchip 1
#endif

#if !defined(MaxIOchannel)
#define MaxIOchannel 16
#endif

#if !defined(MaxDACchannel)
#define MaxDACchannel 8
#endif

#if !defined(MaxADchannel)
#define MaxADchannel 4
#endif

#if !defined(MaxDAchannel
#define MaxDAchannel 1
#endif

class I2c  
{
private:
	char *Bin;

	int IOchipCode[8];
	int DACchipCode[4];
	int ADDAchipCode[4];

	int IO[MaxIOchannel+1];
	unsigned short ControlPort;
	unsigned short StatusPort;
	int I2CbusDelay;
	
	//RADIX CONVERSION FUNCTIONS
	void DecToBin(int DecNumber, char *BinNumber);
	int BinToDec(char *BinNumber);
	void DecToHex(int DecNumber, char *HexNumber);
	int HexToDec(char *HexNumber);

	//I2C BUS CONDITIONS
	void InitChipCode(void);
	void SelectI2CprinterPort(int Printer_no);
	

public:
	I2c();
	virtual ~I2c();

	int IOdata[MaxIOchip+1];
	int IOconfig[MaxIOchip+1];
	int DAC[MaxDACchannel+1];
	int AD[MaxADchannel+1];
	int DA[MaxDAchannel+1];


	void MyDelay(void);

	//My Helper functions
	int  MyInput(unsigned short PortNumber);
	void MyOutput(unsigned short PortNumber, int Value);
	void GenerateStartCondition(void);
	void GenerateAcknowledge(void);
	void GenerateAcknowledgeFromMaster(void);
	void GenerateStopCondition(void);
	void OutputByteToI2C(int DataByte);
	int  InputByteFromI2C(void);
	void UpdateIOArray(int Chip_no);


	//PROCEDURES OM KANALEN TE LEZEN EN TE SCHRIJVEN
	void I2CBusNotBusy(void);
	void SetDAC(int Channel_no,int Waarde);
	int GetDAC(int Channel_no);

	//IO CONFIGURATION PROCEDURES
	void ConfigAllIOasInput(void);
	void ConfigAllIOasOutput(void);
	void ConfigIOchipAsInput(int Chip_no);
	void ConfigIOchipAsOutput(int Chip_no);
	void ConfigIOchannelAsInput(int Channel_no);
	void ConfigIOchannelAsOutput(int Channel_no);

	//UPDATE IOdata & IO ARRAY PROCEDURES
	void UpdateIOdataArray(int Chip_no, int Data);
	void ClearIOdataArray(int Chip_no);
	void SetIOdataArray(int Chip_no);
	void SetIOchArray(int Channel_no);
	void ClearIOchArray(int Channel_no);

	//OUTPUT PROCEDURES
	void IOoutput(int Chip_no, int Data);
	void UpdateAllIO(void);
	void ClearAllIO(void);
	void SetAllIO(void);
	void UpdateIOchip(int Chip_no);
	void ClearIOchip(int Chip_no);
	void SetIOchip(int Chip_no);
	void SetIOchannel(int Channel_no);
	void ClearIOchannel(int Channel_no);

	//INPUT PROCEDURES
	void ReadIOchip(int Chip_no);
	void ReadAllIO(void);
	void ReadIOchannel(int Channel_no);

	//6 BIT DAC CONVERTER PROCEDURES
	void OutputDACchannel(int Channel_no, int Data);
	void UpdateDACchannel(int Channel_no);
	void ClearDACchannel(int Channel_no);
	void SetDACchannel(int Channel_no);
	void UpdateDACchip(int Chip_no);
	void ClearDACchip(int Chip_no);
	void SetDACchip(int Chip_no);
	void UpdateAllDAC(void);
	void ClearAllDAC(void);
	void SetAllDAC(void);

	//8 BIT DA CONVERTER PROCEDURES
	void OutputDAchannel(int Channel_no, int Data);
	void UpdateDAchannel(int Channel_no);
	void ClearDAchannel(int Channel_no);
	void SetDAchannel(int Channel_no);
	void UpdateAllDA(void);
	void ClearAllDA(void);
	void SetAllDA(void);

	//8 BIT AD CONVERTER PROCEDURES
	void ReadADchannel(int Channel_no);
	void ReadADchip(int Chip_no);
	void ReadAllAD(void);

	//GENERAL PROCEDURES
	void ReadAll(void);
	void ReadCard(int Card_no);
	void UpdateAll(void);
	void UpdateCard(int Card_no);

	//Delay handlers
	void SetDelay(int Delay);
	int  GetDelay(void);

};

#define MSB8 (0x80)

class CTIMER 
{
  public:
    inline CTIMER()  { memset(this, 0, sizeof(*this)); 
                       QueryPerformanceFrequency(&m_liFreq);}
    inline void Start()  { QueryPerformanceCounter(&m_sStart); }
    inline void Stop()   { QueryPerformanceCounter(&m_sStop); }
	inline float OutputTime()       { return (float)(( m_sStop.LowPart - m_sStart.LowPart)/(float)m_liFreq.LowPart);}
  inline float OutputTicks()       { 
			return (float)(m_sStop.LowPart - m_sStart.LowPart);}
  inline float OutputFreq()       { 
			return (float)(m_liFreq.LowPart);}

    // data members
    LARGE_INTEGER   m_sStart, m_sStop, m_liFreq;
};
