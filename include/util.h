#pragma once

#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <sstream>
#include <ctime>

class containers
{
	public:
		// thank you Sir Elan Hickler from StackOverflow
		template <typename t> static void move(std::vector<t>& v, size_t oldIndex, size_t newIndex)
		{
			if (oldIndex > newIndex)
				std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
			else        
				std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);
		}

		template <typename t> static void move(std::deque<t, std::allocator<t>>& d, size_t oldIndex, size_t newIndex)
		{
			if (oldIndex > newIndex)
				std::rotate(d.rend() - oldIndex - 1, d.rend() - oldIndex, d.rend() - newIndex);
			else        
				std::rotate(d.begin() + oldIndex, d.begin() + oldIndex + 1, d.begin() + newIndex + 1);
		}
		static std::vector<std::string> split (const std::string &s, char delim) 
		{
			std::vector<std::string> result;
			std::stringstream ss (s);
			std::string item;

			while (getline (ss, item, delim)) {
				result.push_back (item);
			}

			return result;
		}
};

class mytime
{
	public:
        static unsigned int get_year_day_count(unsigned int year)
        {
            if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
            {
                return 366;
            }
            return 365;
        }

		static unsigned int get_month_day_count(unsigned int year, unsigned int month)
		{
			switch(month)
			{
				case(0):
				{
					return 31;
				}
				case(1):
				{
					if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
					{
						return 29;
					}
					else
					{
						return 28;
					}
				}
				case 3: case 5: case 8: case 10:
				{
					return 30;
				}
				default:
				{
					return 31;
				}
			}
		}
		static unsigned int get_month_day_count(const tm& time)
		{
			return get_month_day_count(time.tm_year + 1900, time.tm_mon);
		}

        static unsigned int get_year_day(unsigned int year, unsigned int month, unsigned int monthDay)
        {
            unsigned int yearDay = 0;

            for (size_t m = 0; m < month; m++)
            {
                yearDay += get_month_day_count(year, m);
            }

            yearDay += monthDay;

            return yearDay;
        }
        static unsigned int get_year_day(const tm& time)
        {
            return get_year_day(time.tm_year + 1900, time.tm_mon, time.tm_mday);
        }
};