#pragma once
#include "blf/include/blf.hpp"
#include "blf_static.hpp"
#include <memory>

struct BLF_Base;

class ObjectDefinitions {
    private:
        std::map<std::string, const std::shared_ptr<blf::st::StaticDefinition>> m_objectDefinitions = {};
        blf::StaticObjectTable m_objectTable;

    public:
        blf::StaticObjectTable& getObjectTable() {
            return m_objectTable;
        }
        const blf::StaticObjectTable& getObjectTableConst() const {
            return m_objectTable;
        }

        template <typename T>
        void add(blf::StaticDefinition<T>& definition) {
            static_assert(std::is_base_of_v<BLF_Base, T> == true);
            if (m_objectDefinitions.contains(definition.getName())) {
                printf("ObjectDefinitions::add(definition): Tried to add duplicate object definition with name %s\n",
                       definition.getName().c_str());
                return;
            }
            m_objectDefinitions.insert({definition.getName(), std::make_shared<blf::StaticDefinition<T>>(definition)});
        }
        template <typename T>
        const blf::StaticDefinition<T>& get() {
            static_assert(std::is_base_of_v<BLF_Base, T> == true);
            return *std::dynamic_pointer_cast<blf::StaticDefinition<T>>(m_objectDefinitions.at(T::getName()));
        }
};

struct BLF_Base {
        static constexpr std::string getName() {
            return "BLF_Base";
        }
        static void addDefinition(ObjectDefinitions& objectTable) {
            objectTable.add(objectTable.getObjectTable().define<BLF_Base>(getName()));
        }
};

template <typename T>
concept DerivedBlfBase = std::is_base_of<BLF_Base, T>::value;