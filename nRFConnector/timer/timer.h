/*
 * timer.h
 *
 *  Created on: 07.03.2017
 *      Author: Damian
 */

#ifndef TIMER_H_
#define TIMER_H_

/**
 * Timer2 is set in FastPWM mode
 */


class CTimer1
{
public:
	static CTimer1 * getInstance();
	void setChannelA(uint8_t compareValue);
	void setChannelB(uint8_t compareValue);
	void setChannel(uint8_t channel, uint8_t compareValue);
private:
	//Cnstructors
	CTimer1();	//Prevent consttuction but allow in getInstance
	CTimer1(const CTimer1&);	//Prevent construction by copying
	CTimer1& operator=(const CTimer1&);	//Prevent assigment
	~CTimer1();					//Prevent unwanted destruction
	static CTimer1 * timerInstance;
	static bool isCreated;
};



extern "C"
{
	void TIMER2_COMP_vect(void) __attribute__ ((signal,used, externally_visible)) ;
}


class CTimer2
{
public:
	enum T2Prescallers
	{
		PS_0 = 0,
		PS_1 = (1 << CS20),
		PS_8 = (1 << CS21),
		PS_32 = (1 << CS20) | (1 << CS21),
		PS_64 = (1 << CS22),
		PS_128 = (1 << CS20) | (1 << CS22),
		PS_256 = (1 << CS22) | (1 << CS21),
		PS_1024 = (1 << CS22) | (1 << CS21) | (1 << CS20)
	};

	enum T2Mode
	{
		NormalMode = 0,
		PWMMode = (1 << WGM20),
		CTCMode = (1 << WGM21),
		FastPWMMode = (1 << WGM21) | (1 << WGM20)
	};

	struct TPROGTIMER
	{
		void (*pFunct)(void);
		bool b_Active;
		uint16_t u16_Interval;
		uint16_t u16_Counter;
	};

	static volatile TPROGTIMER TProgTimer[8];

	//Public interface
	static CTimer2 * getInstance(T2Prescallers prescaller = T2Prescallers::PS_1024, uint8_t Tick = 0);
	void SetPeriod (uint8_t Tick);
	void SetPrescaller (T2Prescallers Prescaller);
	uint16_t GetInterval (uint8_t HandlerNumber);
	void Assign(uint8_t HWNDnumber, uint16_t Interval, void (*pfun)(void), bool state = false);
	int8_t AssignFreeChannel(uint16_t Interval, void (*pfun)(void), bool state = false);
	void UnAssign(uint8_t HandlerNumber);
	void Disable(uint8_t HandlerNumber) ;
	void Enable(uint8_t HandlerNumber) ;
	void Hold (uint8_t HandlerNumber);
	void UnHold (uint8_t HandlerNumber);
	bool Enabled (uint8_t HandlerNumber);
	void SetInterval (uint8_t HandlerNumber, uint64_t Interval);
	bool isTimerActive(uint8_t HandlerNumber);

	friend void TIMER2_COMP_vect(void);

private:
	//Cnstructors
	CTimer2(T2Prescallers prescaller, uint8_t Tick);	//Prevent consttuction but allow in getInstance
	CTimer2(const CTimer2&);	//Prevent construction by copying
	CTimer2 & operator=(const CTimer2&);	//Prevent assigment
	~CTimer2();					//Prevent unwanted destruction
	static CTimer2 * timerInstance;
	static bool isCreated;

	//Members

};

/*
//This is a SINGLETON pattern class with reference
class CTimer2
{
public:
	enum T2Prescallers
	{
		PS_0 = 0,
		PS_1 = (1 << CS20),
		PS_8 = (1 << CS21),
		PS_32 = (1 << CS20) | (1 << CS21),
		PS_64 = (1 << CS22),
		PS_128 = (1 << CS20) | (1 << CS22),
		PS_256 = (1 << CS22) | (1 << CS21),
		PS_1024 = (1 << CS22) | (1 << CS21) | (1 << CS20)
	};

	enum T2Mode
	{
		NormalMode = 0,
		PWMMode = (1 << WGM20),
		CTCMode = (1 << WGM21),
		FastPWMMode = (1 << WGM21) | (1 << WGM20)
	};

	struct TPROGTIMER
	{
		void (*pFunct)(void);
		bool b_Active;
		uint16_t u16_Interval;
		uint16_t u16_Counter;
	};



	static volatile TPROGTIMER TProgTimer[8];
	//static volatile uint8_t timer;

	//Public interface
	static CTimer2 & getInstance(T2Prescallers prescaller, uint8_t Tick);
	static void SetPeriod (uint8_t Tick);
	static void SetPrescaller (T2Prescallers Prescaller);
	static uint16_t GetInterval (uint8_t HandlerNumber);
	static void Assign(uint8_t HWNDnumber, uint16_t Interval, void (*pfun)(void));
	static int8_t AssignFreeChannel(uint16_t Interval, void (*pfun)(void));
	static void UnAssign(uint8_t HandlerNumber);
	static void Disable(uint8_t HandlerNumber) ;
	static void Enable(uint8_t HandlerNumber) ;
	static void Hold (uint8_t HandlerNumber);
	static void UnHold (uint8_t HandlerNumber);
	static bool Enabled (uint8_t HandlerNumber);
	static void SetInterval (uint8_t HandlerNumber, uint64_t Interval);

	friend void TIMER2_COMP_vect(void);

	CTimer2 & operator=(const CTimer2&);	//Prevent assigment

private:
	//Cnstructors
	CTimer2(T2Prescallers prescaller, uint8_t Tick);	//Prevent consttuction but allow in getInstance
	CTimer2(const CTimer2&);	//Prevent construction by copying

	~CTimer2();					//Prevent unwanted destruction

	//Members

};

*/


#endif /* TIMER_H_ */
