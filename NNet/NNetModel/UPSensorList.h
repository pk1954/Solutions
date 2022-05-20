// UPSensorList.h 
//
// NNetModel

#pragma once

#include <vector>
#include "NamedType.h"
#include "Sensor.h"

using std::ranges::for_each;
using std::unique_ptr;
using std::wstring;
using std::vector;

class Sensor;
class UPNobList;

using SensorId = NamedType<int, struct SensorIdParam>;
using UPSensor = unique_ptr<Sensor>;

class UPSensorList
{
public:
    ~UPSensorList();

    size_t   Size() const { return m_list.size(); }

    Sensor const * GetSensor(SensorId const) const;
    Sensor       * GetSensor(SensorId const);

    SensorId       GetSensorIdSelected() const { return m_sensorIdSelected; }
    Sensor const * GetSensorSelected  () const { return GetSensor(m_sensorIdSelected); }
    Sensor       * GetSensorSelected  ()       { return GetSensor(m_sensorIdSelected); }

    bool IsAnySensorSelected()      const { return m_sensorIdSelected.IsNotNull(); }
    bool IsValid(SensorId const id) const { return id.GetValue() < m_list.size(); }

    SensorId SetActive(SensorId const);
    UPSensor NewSensor(MicroMeterCircle const &, UPNobList const &);
    SensorId PushSensor(UPSensor);
    UPSensor PopSensor();
    UPSensor RemoveSensor(SensorId const);
    void     InsertSensor(UPSensor, SensorId const);
    SensorId FindSensor(MicroMeterPnt const &);

    void Apply2All(auto const &f)  
    { 
        for_each(m_list, [&f](auto & up) { f(up.get()); });
    }

    void Apply2AllC(auto const &f) const
    { 
        for_each(m_list, [&f](auto const & up) { f(up.get()); });
    }

private:

    vector<UPSensor> m_list;
    SensorId         m_sensorIdSelected { SensorId::NULL_VAL() };

    vector<UPSensor>::      iterator getSensor(SensorId const);
    vector<UPSensor>::const_iterator getSensor(SensorId const) const;

    UPSensor removeSensor(vector<UPSensor>::iterator);
};