#ifndef __PullerGetDeviceInfoTask__
#define __PullerGetDeviceInfoTask__

#include "PullerTaskBase.h"
#include "ModbusDriver.h"


class PullerGetDeviceInfoTask :
    public PullerTaskBase 
{
public:
    PullerGetDeviceInfoTask(int id,const QString& uartName,ModbusDriver::detectionCallback cb);
    virtual ~PullerGetDeviceInfoTask();

    bool proceed(ModBusUART_Impl* modbus);
 
private:
    QString m_uartName;
    ModbusDriver::detectionCallback m_cb;
};

typedef std::shared_ptr<PullerGetDeviceInfoTask> PullerGetDeviceInfoTaskShared;

#endif // __PullerGetDeviceInfoTask__
