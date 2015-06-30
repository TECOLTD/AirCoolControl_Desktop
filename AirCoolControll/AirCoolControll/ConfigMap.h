#ifndef __CONFIGMAP__
#define __CONFIGMAP__

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <list>
#include <Interval.h>
#include <qstring.h>
#include <memory>
#include "DeviceInfo.h"
#include "VersionStorage.h"
#include <assert.h>

class ConfigMap
{
public:
    ConfigMap(const std::string& vendor, const std::string& product, const std::string& versionMin, const std::string& versionMax);
    ~ConfigMap();

    enum RegisterType {
        REGISTER_PULL_FIRST = 0,
        INPUT_REGISTER = 0,
        OUTPUT_REGISTER,
        COIL,
        REGISTER_PULL_COUNT
    };
      
    static RegisterType &NEXT(ConfigMap::RegisterType &c) {
        assert(c < REGISTER_PULL_COUNT);
        c = static_cast<RegisterType>(c + 1);
        return c;
    }

    class ErrorDetector
    {
    public:
        ErrorDetector(){}
        
        struct Error
        {
            enum DetectionType
            {
                EQ,
                GT,
                LT,
                GTE,
                LTE,
                AND,
                OR,
                UNSUPPORT
            };
            const static std::unordered_map<std::string, DetectionType> s_error_map;

            DetectionType m_type = UNSUPPORT;
            std::string   m_description;
            int           m_value;

            Error(std::string name, std::string desc, int v)
            {
                std::unordered_map<std::string, DetectionType>::const_iterator f = s_error_map.find(name);
                if (f == s_error_map.end())
                {
                    m_type = UNSUPPORT;
                }
                else
                {
                    m_type = f->second;
                    m_description = desc;
                    m_value = v;
                }
            }

            bool check(qint16 value) const 
            {
                bool rc = false;
                switch (m_type) 
                {
                case EQ:
                    rc = value == m_value;
                    break;
                case GT:
                    rc = value > m_value;
                    break;
                case LT:
                    rc = value < m_value;
                    break;
                case GTE:
                    rc = value >= m_value;
                    break;
                case LTE:
                    rc = value <= m_value;
                    break;
                case AND:
                    rc = value & m_value;
                    break;
                case OR:
                    rc = value | m_value;
                    break;
                }

                return rc;
            }
        };

        void addError(std::string name, std::string desc, int v)
        {
            Error e(name, desc, v);
            if (e.m_type != Error::UNSUPPORT)
            {
                m_detection_list.push_back(e);
            }
        }

        bool isValid(qint16 value) const
        {
            bool rc = true;
            for (const auto& i : m_detection_list)
            {
                if (i.check(value))
                {
                    rc = false;
                    break;
                }
            }
            return rc;
        }
    private:
        std::vector<Error> m_detection_list;
    };

    typedef struct 
    {
        int                     m_registerNumber;
        std::string             m_description;
        bool                    m_isBool;
        ConfigMap::RegisterType m_type;
        int                     m_bitNumber;
        std::string             m_decodeMethod;
        int                     m_minValue;
        int                     m_maxValue;
        ErrorDetector           m_errorDetector;
    } Parameter;

    typedef std::vector<std::pair<std::string, std::string>> ParameterList;
    typedef std::vector<std::pair<std::string, Parameter>> ParameterMap;

    void addVariable(int n,const std::string& name, const Parameter& p);
    ConfigMap::RegisterType getVariableType(const std::string& name) const;
    int  getRegisterNumber(const std::string& name) const;
    bool haveVariableWithName(const std::string& name) const;
    unsigned int  getValue(const std::string& name, const std::vector<quint16>& array) const;
    bool isVariableBool(const std::string& name, int& bitNumber);
    Interval& getInterval(int n);
    bool  isSupport(const DeviceInfoShared info) const;
    ParameterList getParametersList(ConfigMap::RegisterType e);
    void  setUI_Config(const std::string& type, const std::string& configFile);
    ParameterMap::const_iterator findParameter(const std::string& name) const;

private:
    static qint16 decodeWithMethod(qint16 value, const std::string& method);
    
private:
    std::string     m_vendor;
    std::string     m_product;
    VersionStorage  m_versionMin;
    VersionStorage  m_versionMax;
    std::string     m_UI_type;
    std::string     m_UI_config;

    ParameterMap    m_map;
    Interval        m_registersIntervals[REGISTER_PULL_COUNT];

    ConfigMap::ParameterList m_parameters[REGISTER_PULL_COUNT];
};

typedef std::shared_ptr<ConfigMap> ConfigMapShared;
typedef std::list<ConfigMapShared> ConfigList;

#endif // __CONFIGMAP__