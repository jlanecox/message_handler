
#include <iostream>

#include "data_handler.h"


DataHandler::~DataHandler()
{
	for (auto& handler : _handlers) {
		delete handler.second;
	}
	_handlers.clear();
}

#if USE_PTR
void DataHandler::handle(const DataClass* _data)
{
	Handlers::iterator it = _handlers.find(typeid(*_data));
	if (it != _handlers.end()) {
		it->second->exec(_data);
	}
	else {
		std::cout << "No handler found for type: " << typeid(*_data).name() << std::endl;
	}
}
#else
void DataHandler::handle(const DataClass& _data)
{
	Handlers::iterator it = _handlers.find(typeid(_data));
	if (it != _handlers.end()) {
		it->second->exec(_data);
	}
	else {
		std::cout << "No handler found for type: " << typeid(_data).name() << std::endl;
	}
}
#endif