#ifndef COMMBASE_H
#define COMMBASE_H
#include <string>

typedef void (*pNetRunCallback)();
typedef unsigned int sender_handle;
typedef unsigned int receiver_handle;
#define ErrorHandle 0xFFFFFFFF
#define __CommBase(c) private:	friend CommBase<c>;
#define defaultPartitiontName	"__BUILT-IN PARTITION__"
template<typename T> 
class CommBase 
{
public:
	static CommBase & GetInstance() 
	{  
		static T instance;    
		return instance;  
	}  
	static CommBase* GetInstancePtr() 
	{   
		return &GetInstance();  
	}  
	virtual sender_handle publish    (int dataSize,const void* pStData, const char* tblname, const char* dbname) =0;
	virtual receiver_handle subscribe  (int dataSize,void* pStData, const char* tblname, const char* dbname) =0;
	virtual bool sendData(int index, sender_handle hand)=0;
	virtual int recvData(receiver_handle hand)=0;
	virtual inline const char* getLastError()=0;
protected:
	CommBase(){};
	~CommBase(){};
private:
	CommBase(const CommBase &){};  
	CommBase& operator = (const CommBase &);  
};

////
#endif // COMMBASE_H
