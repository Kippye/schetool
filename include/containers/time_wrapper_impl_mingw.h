#pragma once

#include <iostream>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <sysinfoapi.h>
#include <timezoneapi.h>
#include <datetimeapi.h>

// Expects the time zone to be the same as in getTimeZoneOffsetMinutesUTC().
// Expects the time provided to have the base UTC offset applied.
inline int getDaylightSavingsOffsetMinutes(int year, unsigned int month, unsigned int weekday, unsigned int weekdayIndex, bool isLastWeekdayIndex, unsigned int hours, unsigned int minutes, int timezoneIndex = -1)
{
    DYNAMIC_TIME_ZONE_INFORMATION* dynTimezoneInfo = NULL;
    // DYNAMIC_TIME_ZONE_INFORMATION tzi{};

    // // Use a custom time zone (Readd -D_WIN32_WINNT=0x0602 comp. flag)
    // if (timezoneIndex > -1)
    // {
    //     EnumDynamicTimeZoneInformation(timezoneIndex, &tzi);
    //     dynTimezoneInfo = &tzi;
    //     std::wcout << L"getDaylightSavingsOffsetMinutes(): Using timezone: " << tzi.StandardName << std::endl;
    // }

    TIME_ZONE_INFORMATION timeZoneInfo;
    GetTimeZoneInformationForYear((USHORT)year, dynTimezoneInfo, &timeZoneInfo); 

    // The time zone never uses DST, just return the standard bias (should be 0)
    if (timeZoneInfo.DaylightDate.wMonth == 0)
    {
        return timeZoneInfo.StandardBias;
    }

    // Set last weekday index to 5 like the tz info struct does
    weekdayIndex = isLastWeekdayIndex == true ? 5 : weekdayIndex;
    
    bool isDST = false;  

    // Daylight start date comes before standard
    if (timeZoneInfo.DaylightDate.wMonth < timeZoneInfo.StandardDate.wMonth)
    {
        isDST = (month > timeZoneInfo.DaylightDate.wMonth || (month == timeZoneInfo.DaylightDate.wMonth 
        && (weekdayIndex > timeZoneInfo.DaylightDate.wDay || (weekdayIndex == timeZoneInfo.DaylightDate.wDay
        && (weekday > timeZoneInfo.DaylightDate.wDayOfWeek || (weekday == timeZoneInfo.DaylightDate.wDayOfWeek
        && (hours > timeZoneInfo.DaylightDate.wHour || (hours == timeZoneInfo.DaylightDate.wHour
        && minutes > timeZoneInfo.DaylightDate.wMinute))))))))
            &&
        (month < timeZoneInfo.StandardDate.wMonth || (month == timeZoneInfo.StandardDate.wMonth 
        && (weekdayIndex < timeZoneInfo.StandardDate.wDay || (weekdayIndex == timeZoneInfo.StandardDate.wDay
        && (weekday < timeZoneInfo.StandardDate.wDayOfWeek || (weekday == timeZoneInfo.StandardDate.wDayOfWeek
        && (hours < timeZoneInfo.StandardDate.wHour || (hours == timeZoneInfo.StandardDate.wHour
        && minutes < timeZoneInfo.StandardDate.wMinute))))))));
    }
    // Daylight start date comes after standard - the period between standard and daylight, isDST == FALSE
    else
    {
        isDST = !((month > timeZoneInfo.StandardDate.wMonth || (month == timeZoneInfo.StandardDate.wMonth 
        && (weekdayIndex > timeZoneInfo.StandardDate.wDay || (weekdayIndex == timeZoneInfo.StandardDate.wDay
        && (weekday > timeZoneInfo.StandardDate.wDayOfWeek || (weekday == timeZoneInfo.StandardDate.wDayOfWeek
        && (hours > timeZoneInfo.StandardDate.wHour || (hours == timeZoneInfo.StandardDate.wHour
        && minutes > timeZoneInfo.StandardDate.wMinute))))))))
            &&
        (month < timeZoneInfo.DaylightDate.wMonth || (month == timeZoneInfo.DaylightDate.wMonth 
        && (weekdayIndex < timeZoneInfo.DaylightDate.wDay || (weekdayIndex == timeZoneInfo.DaylightDate.wDay
        && (weekday < timeZoneInfo.DaylightDate.wDayOfWeek || (weekday == timeZoneInfo.DaylightDate.wDayOfWeek
        && (hours < timeZoneInfo.DaylightDate.wHour || (hours == timeZoneInfo.DaylightDate.wHour
        && minutes < timeZoneInfo.DaylightDate.wMinute)))))))));
    }

    // std::cout << "INPUT TIME" << std::endl;
    // std::cout << year << "; ";
    // std::cout << month << "; ";
    // std::cout << weekdayIndex << "; ";
    // std::cout << weekday << std::endl;
    // std::cout << hours << ":";
    // std::cout << minutes << std::endl;

    // std::cout << "DAYLIGHT DATE" << std::endl;
    // std::cout << timeZoneInfo.DaylightDate.wYear << "; ";
    // std::cout << timeZoneInfo.DaylightDate.wMonth << "; ";
    // std::cout << timeZoneInfo.DaylightDate.wDay << "; ";
    // std::cout << timeZoneInfo.DaylightDate.wDayOfWeek << std::endl;
    // std::cout << timeZoneInfo.DaylightDate.wHour << ":";
    // std::cout << timeZoneInfo.DaylightDate.wMinute << std::endl;
    // std::cout << timeZoneInfo.DaylightBias << std::endl;
    // std::cout << "STANDARD DATE" << std::endl;
    // std::cout << timeZoneInfo.StandardDate.wYear << "; ";
    // std::cout << timeZoneInfo.StandardDate.wMonth << "; ";
    // std::cout << timeZoneInfo.StandardDate.wDay << "; ";
    // std::cout << timeZoneInfo.StandardDate.wDayOfWeek << std::endl;
    // std::cout << timeZoneInfo.StandardDate.wHour << ":";
    // std::cout << timeZoneInfo.StandardDate.wMinute << std::endl;
    // std::cout << timeZoneInfo.StandardBias << std::endl;
    
    return isDST ? -timeZoneInfo.DaylightBias : -timeZoneInfo.StandardBias; 
}

// Get the base offset from UTC
inline int getTimeZoneOffsetMinutesUTC(int year, int timezoneIndex = -1)
{
    DYNAMIC_TIME_ZONE_INFORMATION* dynTimezoneInfo = NULL;
    // DYNAMIC_TIME_ZONE_INFORMATION tzi{};

    // // Use a custom time zone (Readd -D_WIN32_WINNT=0x0602 comp. flag)
    // if (timezoneIndex > -1)
    // {
    //     EnumDynamicTimeZoneInformation(timezoneIndex, &tzi);
    //     dynTimezoneInfo = &tzi;
    //     std::wcout << L"getTimeZoneOffsetMinutesUTC(): Using timezone: " << tzi.StandardName << std::endl;
    // }

    TIME_ZONE_INFORMATION timeZoneInfo;
    // GetTimeZoneInformation(&timeZoneInfo);
    GetTimeZoneInformationForYear((USHORT)year, dynTimezoneInfo, &timeZoneInfo);
    return -timeZoneInfo.Bias;
}