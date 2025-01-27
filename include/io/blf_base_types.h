#pragma once
#include "blf/include/blf.hpp"
#include <memory>

struct BLF_Base;

class ObjectDefinitions
{
    private:
        std::map<std::string, const std::shared_ptr<blf::ObjectDefinition>> m_localObjectDefinitions = {};
        blf::LocalObjectTable m_objectTable;
    public:
        blf::LocalObjectTable& getObjectTable()
        {
            return m_objectTable;
        }
        const blf::LocalObjectTable& getObjectTableConst() const
        {
            return m_objectTable;
        }

        template <typename T>
        void add(blf::LocalObjectDefinition<T>& definition)
        {
            static_assert(std::is_base_of_v<BLF_Base, T> == true);
            if (m_localObjectDefinitions.contains(definition.getName()))
            {
                printf("ObjectDefinitions::add(definition): Tried to add duplicate object definition with name %s\n", definition.getName().getBuffer());
                return;
            }
            m_localObjectDefinitions.insert({definition.getName(), std::make_shared<blf::LocalObjectDefinition<T>>(definition)});
        }
        template <typename T>
        const blf::LocalObjectDefinition<T>& get()
        {
            static_assert(std::is_base_of_v<BLF_Base, T> == true);
            return *std::dynamic_pointer_cast<blf::LocalObjectDefinition<T>>(m_localObjectDefinitions.at(T::getName()));
        }
};

struct BLF_Base
{
    static constexpr std::string getName()
    {
        return "BLF_Base";
    }
    static void addDefinition(ObjectDefinitions& objectTable)
    {
        objectTable.add(objectTable.getObjectTable().define<BLF_Base>(getName()));
    }
};

template <typename T>
concept DerivedBlfBase = std::is_base_of<BLF_Base, T>::value;