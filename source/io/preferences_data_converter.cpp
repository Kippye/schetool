#include "preferences_data_converter.h"

using namespace blf;
using namespace blf::file;

const std::string& PreferencesDataConverter::getExtension() const {
    return m_extension;
}

void PreferencesDataConverter::setupObjectTable() {
    addObjectDefinition<BLF_Base>();
    addObjectDefinition<BLF_Preferences>();
}

bool PreferencesDataConverter::isValidPreferencesFile(const char* path) const {
    try {
        // Try to load the file
        FileReadStream stream(path);
        File file = File::fromData(stream);
        // Try to deserialize the file
        file.deserializeBody(m_definitions.getObjectTableConst());
        return true;
    }
    // The file is not valid BLF or some other I/O error occurred, catch error and return false
    // OR Some error occurred, probably due to a mismatch in the object tables, return false
    catch (std::exception& e)
    {
        return false;
    }
}

int PreferencesDataConverter::writePreferences(const char* path, const Preferences& preferences) {
    FileWriteStream stream(path);

    DataTable data;

    data.insert(getObjectDefinition<BLF_Preferences>().serialize(preferences));

    File file(data, m_definitions.getObjectTable(), {blf::CompressionType::None, blf::EncryptionType::None});

    file.serialize(stream);

    return 0;
}

std::optional<Preferences> PreferencesDataConverter::readPreferences(const char* path) {
    FileReadStream stream(path);

    auto file = File::fromData(stream);

    auto fileBody = file.deserializeBody(m_definitions.getObjectTableConst());

    BLF_Preferences preferences = *fileBody.data.groupby(m_definitions.get<BLF_Preferences>()).begin();
    Preferences returnPreferences = preferences.getPreferences();

    return returnPreferences;
}