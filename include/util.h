#pragma once

#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <sstream>
#include <ctime>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#define GLM_ENABLE_EXPERIMENTAL

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

class debug
{
	public:
		static void print_info(const char* text)
		{

		}
		static void print_warning(const char* text)
		{

		}
		static void print_error(const char* text)
		{

		}
};

class mymath
{
	public:
		static inline float gridSize = 1.0f;

		static float floor_to_grid(float f, float _gridSize = gridSize)
		{
			return floor(f / _gridSize) * _gridSize;
		}
		static glm::vec2 floor_to_grid(glm::vec2 pos, float _gridSize = gridSize)
		{
			pos.x = floor_to_grid(pos.x, _gridSize);
			pos.y = floor_to_grid(pos.y, _gridSize);
			return pos;
		}
		static glm::vec3 floor_to_grid(glm::vec3 pos, float _gridSize = gridSize)
		{
			pos.x = floor_to_grid(pos.x, _gridSize);
			pos.y = floor_to_grid(pos.y, _gridSize);
			return pos;
		}
		static glm::vec4 floor_to_grid(glm::vec4 pos, float _gridSize = gridSize)
		{
			pos.x = floor_to_grid(pos.x, _gridSize);
			pos.y = floor_to_grid(pos.y, _gridSize);
			return pos;
		}
		static float ceil_to_grid(float f, float _gridSize = gridSize)
		{
			return ceil(f / _gridSize) * _gridSize;
		}
		static glm::vec2 ceil_to_grid(glm::vec2 pos, float _gridSize = gridSize)
		{
			pos.x = ceil_to_grid(pos.x, _gridSize);
			pos.y = ceil_to_grid(pos.y, _gridSize);
			return pos;
		}
		static glm::vec3 ceil_to_grid(glm::vec3 pos, float _gridSize = gridSize)
		{
			pos.x = ceil_to_grid(pos.x, _gridSize);
			pos.y = ceil_to_grid(pos.y, _gridSize);
			return pos;
		}
		static glm::vec4 ceil_to_grid(glm::vec4 pos, float _gridSize = gridSize)
		{
			pos.x = ceil_to_grid(pos.x, _gridSize);
			pos.y = ceil_to_grid(pos.y, _gridSize);
			return pos;
		}
		static float round_to_grid(float f, float _gridSize = gridSize)
		{
			return round(f / _gridSize) * _gridSize;
		}
		static glm::vec2 round_to_grid(glm::vec2 pos, float _gridSize = gridSize)
		{
			pos.x = round_to_grid(pos.x, _gridSize);
			pos.y = round_to_grid(pos.y, _gridSize);
			return pos;
		}
		static glm::vec3 round_to_grid(glm::vec3 pos, float _gridSize = gridSize)
		{
			pos.x = round_to_grid(pos.x, _gridSize);
			pos.y = round_to_grid(pos.y, _gridSize);
			return pos;
		}
		static glm::vec4 round_to_grid(glm::vec4 pos, float _gridSize = gridSize)
		{
			pos.x = round_to_grid(pos.x, _gridSize);
			pos.y = round_to_grid(pos.y, _gridSize);
			return pos;
		}
		static double rad(double deg)
		{
			if (deg == 0 || deg == 360)
				return 0;
			
			return deg * (glm::pi<double>() / 180);
		}
		static double deg(double rad)
		{
			if (rad == 0)
				return 0;

			return rad / glm::pi<double>() * 180;
		}
};

class Bounding_Box
{
	public:
		glm::vec2 minimum, maximum;
		glm::vec2 size;

	public:
		Bounding_Box()
		{
			size = glm::vec2(1.0f);
		}

		Bounding_Box(glm::vec2 size)
		{
			update_size(size);
		}

		void update_size(glm::vec2 size)
		{
			this->size = size;
			// handle negative sizes correctly as well
			minimum.x = size.x > 0 ? 0.0f : size.x;
			minimum.y = size.y > 0 ? 0.0f : size.y;
			maximum.x = size.x > 0 ? size.x : 0.0f;
			maximum.y = size.y > 0 ? size.y : 0.0f;
		}

		bool contains_position(const glm::vec4 &boxPos, const glm::vec4 &sourcePos)
		{
			return sourcePos.x >= (boxPos.x) && sourcePos.y >= (boxPos.y)
			&& sourcePos.x <= (maximum.x + boxPos.x) && sourcePos.y <= (maximum.y + boxPos.y);
		}

		bool overlaps(glm::vec4 &boxPos, const glm::vec4 &sourcePos, const Bounding_Box &sourceBox)
		{
			glm::vec4 bl = sourcePos + glm::vec4(sourceBox.minimum + glm::vec2(0.01f), 0.0f, 1.0f);
			glm::vec4 br = sourcePos + glm::vec4(sourceBox.minimum.x + sourceBox.size.x - 0.01f, sourceBox.minimum.y + 0.01f, 0.0f, 1.0f);
			glm::vec4 tl = sourcePos + glm::vec4(sourceBox.minimum.x + 0.01f, sourceBox.minimum.y + sourceBox.size.y - 0.01f, 0.0f, 1.0f);
			glm::vec4 tr = sourcePos + glm::vec4(sourceBox.maximum - glm::vec2(0.01f), 0.0f, 1.0f);

			return
			(
				contains_position(boxPos, bl) || // contains BL
				contains_position(boxPos, br) || // BR
				contains_position(boxPos, tl) || // TL
				contains_position(boxPos, tr) // TR
			);
		}

		bool contains(glm::vec4 &boxPos, glm::vec4 &sourcePos, Bounding_Box &sourceBox)
		{
			glm::vec4 bl = sourcePos + glm::vec4(sourceBox.minimum + glm::vec2(0.01f), 0.0f, 1.0f);
			glm::vec4 br = sourcePos + glm::vec4(sourceBox.minimum.x + sourceBox.size.x - 0.01f, sourceBox.minimum.y + 0.01f, 0.0f, 1.0f);
			glm::vec4 tl = sourcePos + glm::vec4(sourceBox.minimum.x + 0.01f, sourceBox.minimum.y + sourceBox.size.y - 0.01f, 0.0f, 1.0f);
			glm::vec4 tr = sourcePos + glm::vec4(sourceBox.maximum - glm::vec2(0.01f), 0.0f, 1.0f);

			return
			(
				contains_position(boxPos, bl) && // contains BL
				contains_position(boxPos, br) && // contains BR
				contains_position(boxPos, tl) && // contains TL
				contains_position(boxPos, tr) // containts TR
			);
		}
};
