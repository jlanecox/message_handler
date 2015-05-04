#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <unordered_map>
#include <typeindex>

#define USE_PTR 0

class DataClass {
protected:
	virtual ~DataClass() {};
};

class HandlerFunctionBase {
public:
	virtual ~HandlerFunctionBase() {};
#if USE_PTR
	void exec(const DataClass* _data) {call( _data );}
#else
	void exec(const DataClass& _data) { call(_data); }
#endif

private:
#if USE_PTR
	virtual void call(const DataClass* _data) = 0;
#else
	virtual void call(const DataClass& _data) = 0;
#endif
};


template <class T, class DataT>
class MemberFunctionHandler : public HandlerFunctionBase {
public:
#if USE_PTR
	typedef void (T::*MemberFunc)(DataT*);

	void call(const DataClass* _data) {
			(_instance->*_function)(static_cast<DataT*>(_data));
	}
#else
	typedef void (T::*MemberFunc)(DataT&);

	void call(const DataClass& _data) {
		(_instance->*_function)(static_cast<DataT&>(_data));
	}
#endif

	MemberFunctionHandler(T* instance, MemberFunc memFn) : _instance(instance), _function(memFn) {};


private:
	T* _instance;
	MemberFunc _function;
};


class DataHandler {
public:
	~DataHandler();
	DataHandler() {};
	DataHandler(const DataHandler& _dh) = delete; //non-copyable

#if USE_PTR
	void handle(const DataClass* _data);
	
	template <class T, class DataT>
	void registerDataHandlerFunc(T*, void (T::*memFn)(DataT*));
#else
	void handle(const DataClass& _data);

	template <class T, class DataT>
	void registerDataHandlerFunc(T*, void (T::*memFn)(DataT&));
#endif

private:
	typedef std::unordered_map<std::type_index, HandlerFunctionBase*> Handlers;
	Handlers _handlers;
};


#if USE_PTR
template <class T, class DataT>
void DataHandler::registerDataHandlerFunc(T* obj, void (T::*memFn)(DataT*)) {
	_handlers[typeid(DataT)]= new MemberFunctionHandler<T, DataT>(obj, memFn);
}
#else
template <class T, class DataT>
void DataHandler::registerDataHandlerFunc(T* obj, void (T::*memFn)(DataT&)) {
	_handlers[typeid(DataT)] = new MemberFunctionHandler<T, DataT>(obj, memFn);
}
#endif

#endif