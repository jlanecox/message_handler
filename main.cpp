#include <iostream>
#include <vector>

#include "data_handler.h"


class TestData : public DataClass {
public:
	TestData(int pos) : _pos(pos), _result(60) {};

	int getPosition()	const	{return _pos;}
	int getResult()		const	{return _result;}

private:
	int _pos, _result;
};

class ProxyData
{
public:
	ProxyData() : _name("PH"), _result(15) {};
	
	std::string getName()	const {return _name;}
	int			getResult()	const {return _result;}

private:
	std::string _name;
	int _result;
};

class TestProxyData : public DataClass
{
public:
	TestProxyData(ProxyData* pd) : _pd(pd){}

	ProxyData* getPd() const		{return _pd;}

private:
	ProxyData* _pd;
};


class ExchangeController : public DataHandler
{
public:
	ExchangeController() : _pos(20), _result(100)
	{
		registerDataHandlerFunc(this, &ExchangeController::gotTestResult);
		registerDataHandlerFunc(this, &ExchangeController::gotTestProxyResult);
	}

	void receiveResult(int result)
	{
		_result -= result;
		std::cout << "result: " << _result << std::endl;
		if(_result <= 0)
		{
			std::cout << "Too many errors" << std::endl;
		}
	}
#if USE_PTR
	void gotTestResult(const TestData* testdata) {
		if(abs(_pos - testdata->getPosition()) < 20) {
			std::cout << "Out of bounds!!!" << std::endl;
			receiveResult(testdata->getResult());
		}
		else {
			std::cout << "In bounds, no error :)" << std::endl;
		}
	}
		
	void gotTestProxyResult(const TestProxyData* tpd) {
		ProxyData* pd = tpd->getPd();
		std::cout << ": " << pd->getName().c_str() << " got proxied results: " << pd->getResult() << std::endl;
		receiveResult(pd->getResult());
	}
#else
	void gotTestResult(const TestData& testdata) {
		if (abs(_pos - testdata.getPosition()) < 20) {
			std::cout << "Out of bounds!!!" << std::endl;
			receiveResult(testdata.getResult());
		}
		else {
			std::cout << "In bounds, no error :)" << std::endl;
		}
	}

	void gotTestProxyResult(const TestProxyData& tpd) {
		ProxyData* pd = tpd.getPd();
		std::cout << ": " << pd->getName().c_str() << " got proxied results: " << pd->getResult() << std::endl;
		receiveResult(pd->getResult());
	}
#endif

	int _pos;
	int _result;
};


class GuiManager
{
public:
	GuiManager() : _dataHandler(this) {}

#if USE_PTR
	void handle(const DataClass* _data) {
		if (_data)
#else
		void handle(const DataClass& _data) {
#endif
		{
			_dataHandler.handle(_data);
		}
	}

private:
	class MessageHandler : public DataHandler
	{
	public:
		MessageHandler(GuiManager* gm) : _gm(gm) {
			registerDataHandlerFunc(_gm, &GuiManager::gotTestResult);
		}
		GuiManager* _gm;
	};
	//friend class TankEventHandler; // not needed with C++11
#if USE_PTR
	void gotTestResult(const TestData* testdata) {
		std::cout << "Got test result: " << testdata->getResult() << std::endl;
	}
#else
	void gotTestResult(const TestData& testdata) {
		std::cout << "Got test result: " << testdata.getResult() << std::endl;
	}
#endif
	MessageHandler _dataHandler;
};


int main()
{
	ExchangeController ec;
	GuiManager gm;


	std::cout << "=== ExchangeController ===" << std::endl; 
#if USE_PTR
	ec.handle(new TestData(40));
	ec.handle(new TestData(10));
	ec.handle(new TestProxyData(new ProxyData));
	ec.handle(new TestProxyData(new ProxyData));
	ec.handle(new TestProxyData(new ProxyData));

	std::cout << "\n=== GuiManager ===" << std::endl;
	gm.handle(new TestData(40));
	gm.handle(new TestData(10));
	gm.handle(new TestProxyData(new ProxyData));
#else
	ec.handle( TestData(40));
	ec.handle( TestData(10));
	ec.handle( TestProxyData(new ProxyData));
	ec.handle( TestProxyData(new ProxyData));
	ec.handle( TestProxyData(new ProxyData));

	std::cout << "\n=== GuiManager ===" << std::endl; 
	gm.handle(TestData(40));
	gm.handle(TestData(10));
	gm.handle(TestProxyData(new ProxyData));
#endif
	system("PAUSE");
}


