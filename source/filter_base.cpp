#include "filter_base.h"
#include <cstring>

TypeComparisonInfo TypeComparisonOptions::getOptions(SCHEDULE_TYPE type)
{
    if (m_comparisonOptions.find(type) == m_comparisonOptions.end()) { printf("TypeComparisonOptions::getComparisonOptions(%d): No comparison options for type.\n", type); return TypeComparisonInfo({} , {}); }

    auto comparisons = m_comparisonOptions.at(type);

    // // thank you ZMATEUSZ!
    // int len = 0;
    // // did this if you're gonna add more
    // for(int i = 0; i < comparisons.size(); i++)
    // {
    //     // +1 to account for NULL.
    //     len += strlen(comparisonStrings.at(comparisons[i])) + 1;
    // }

    // char* output = new char[len];
    // char* head = output;

    // for(int i = 0; i < comparisons.size(); i++)
    // {
    //     int stringLen = strlen(comparisonStrings.at(comparisons[i])) + 1;
    //     // assuming that the string is in fact null terminated (i mean we used strlen and it didnt shit itself until this point so it has to be.)
    //     std::memcpy(head, comparisonStrings.at(comparisons[i]), stringLen);
    //     head += stringLen;
    // }

    std::vector<std::string> comparisonNames = {};

    for (Comparison comparison : comparisons)
    {
        comparisonNames.push_back(std::string(comparisonStrings.at(comparison)));
    }

    return TypeComparisonInfo(m_comparisonOptions.at(type), comparisonNames);
}

int TypeComparisonOptions::getOptionSelection(SCHEDULE_TYPE type)
{
    if (m_selectedOptions.find(type) == m_selectedOptions.end()) { printf("TypeComparisonOptions::getOptionSelection(%d): No selection for type.\n", type); return 0; }
    return m_selectedOptions.at(type);
}

const char* TypeComparisonOptions::getComparisonString(Comparison comparison) const
{
    return comparisonStrings.at(comparison);
}

void TypeComparisonOptions::setOptionSelection(SCHEDULE_TYPE type, int selection)
{
    if (m_comparisonOptions.find(type) == m_comparisonOptions.end()) { printf("TypeComparisonOptions::setOptionSelection(%d, %d): No comparison options for type.\n", type, selection); return; }
    if (m_selectedOptions.find(type) == m_selectedOptions.end()) { printf("TypeComparisonOptions::setOptionSelection(%d, %d): No selection for type.\n", type, selection); return; }

    size_t optionCount = getOptions(type).comparisons.size();

    if (selection >= optionCount) 
    { 
        printf("TypeComparisonOptions::setOptionSelection(%d, %d): Selection higher than amount of options (%td). Clamping it to %td.\n", type, selection, optionCount, optionCount - 1);
        selection = optionCount - 1;
    }

    m_selectedOptions.at(type) = selection;
}

bool FilterBase::checkPasses(const ElementBase* element)
{
    return true;
}