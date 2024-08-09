#pragma once

#include <iostream>
#include <chrono>

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

using namespace std::chrono;

// Expects the time zone to be the same as in getTimeZoneOffsetMinutesUTC().
// Expects the time provided to have the base UTC offset applied.
inline minutes getDaylightSavingsOffsetMinutes(const local_time<seconds>& localStdTime, int timezoneIndex = -1)
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

    auto localDays = floor<days>(localStdTime);
    year_month_day date = year_month_day(localDays);
    hh_mm_ss localClockTime = hh_mm_ss(floor<seconds>(localStdTime - localDays));
    year_month_weekday ymw = year_month_weekday(localDays);
    // Silly check to see if the weekday is the last of its kind in the month
    auto nextWeekDays = localDays + days{7};
    bool isLastWeekdayIndex = year_month_weekday(nextWeekDays).index() == 1;
    
    // Convert to valid type
    WORD year = (int)date.year();

    TIME_ZONE_INFORMATION timeZoneInfo;
    GetTimeZoneInformationForYear((USHORT)year, dynTimezoneInfo, &timeZoneInfo); 

    // The time zone never uses DST, just return the standard bias (should be 0)
    if (timeZoneInfo.DaylightDate.wMonth == 0)
    {
        return minutes{(int)timeZoneInfo.StandardBias};
    }

    // Convert to comparable types
    WORD month = (unsigned int)date.month();
    WORD weekday = ymw.weekday().c_encoding();
    WORD weekdayIndex = ymw.index();
    WORD hours = (unsigned int)localClockTime.hours().count();
    WORD mins = (unsigned int)localClockTime.minutes().count();

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
        && mins > timeZoneInfo.DaylightDate.wMinute))))))))
            &&
        (month < timeZoneInfo.StandardDate.wMonth || (month == timeZoneInfo.StandardDate.wMonth 
        && (weekdayIndex < timeZoneInfo.StandardDate.wDay || (weekdayIndex == timeZoneInfo.StandardDate.wDay
        && (weekday < timeZoneInfo.StandardDate.wDayOfWeek || (weekday == timeZoneInfo.StandardDate.wDayOfWeek
        && (hours < timeZoneInfo.StandardDate.wHour || (hours == timeZoneInfo.StandardDate.wHour
        && mins < timeZoneInfo.StandardDate.wMinute))))))));
    }
    // Daylight start date comes after standard - the period between standard and daylight, isDST == FALSE
    else
    {
        isDST = !((month > timeZoneInfo.StandardDate.wMonth || (month == timeZoneInfo.StandardDate.wMonth 
        && (weekdayIndex > timeZoneInfo.StandardDate.wDay || (weekdayIndex == timeZoneInfo.StandardDate.wDay
        && (weekday > timeZoneInfo.StandardDate.wDayOfWeek || (weekday == timeZoneInfo.StandardDate.wDayOfWeek
        && (hours > timeZoneInfo.StandardDate.wHour || (hours == timeZoneInfo.StandardDate.wHour
        && mins > timeZoneInfo.StandardDate.wMinute))))))))
            &&
        (month < timeZoneInfo.DaylightDate.wMonth || (month == timeZoneInfo.DaylightDate.wMonth 
        && (weekdayIndex < timeZoneInfo.DaylightDate.wDay || (weekdayIndex == timeZoneInfo.DaylightDate.wDay
        && (weekday < timeZoneInfo.DaylightDate.wDayOfWeek || (weekday == timeZoneInfo.DaylightDate.wDayOfWeek
        && (hours < timeZoneInfo.DaylightDate.wHour || (hours == timeZoneInfo.DaylightDate.wHour
        && mins < timeZoneInfo.DaylightDate.wMinute)))))))));
    }

    // std::cout << "INPUT TIME" << std::endl;
    // std::cout << year << "; ";
    // std::cout << month << "; ";
    // std::cout << weekdayIndex << "; ";
    // std::cout << weekday << std::endl;
    // std::cout << hours << ":";
    // std::cout << mins << std::endl;

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
    
    return isDST ? minutes{-timeZoneInfo.DaylightBias} : minutes{-timeZoneInfo.StandardBias}; 
}

// Get the base offset from UTC
inline minutes getTimeZoneOffsetFromUTC(int year, int timezoneIndex = -1)
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
    return minutes{-timeZoneInfo.Bias};
}

// Apply the current UTC and DST offsets for the (current) time zone.
// Returns the total offset applied in minutes.
inline minutes applyLocalTimeOffsetFromUTC(local_time<seconds>& utcTime, int timezoneIndex = -1)
{
    auto localDays = floor<days>(utcTime);
    year_month_day date = year_month_day(localDays);
    minutes utcOffsetMins = getTimeZoneOffsetFromUTC((int)date.year(), timezoneIndex);
    // std::cout << "UTC offset: " << utcOffsetMins << std::endl;
    // Add UTC offset BEFORE calling function to get DST offset.
    utcTime += utcOffsetMins;

    minutes daylightSavingsOffsetMins = getDaylightSavingsOffsetMinutes(utcTime, timezoneIndex);
    // std::cout << "DST offset: " << daylightSavingsOffsetMins << std::endl;
    // Apply DST offset
    utcTime += daylightSavingsOffsetMins;

    return utcOffsetMins + daylightSavingsOffsetMins;
}