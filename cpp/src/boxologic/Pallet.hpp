#pragma once
#include <boxologic/Instance.hpp>

namespace boxologic
{
	/**
	 * @brief A pallet containing boxes.
	 *
	 * @see bws::packer::Wrapper
	 *
	 * @author Bill Knechtel, <br>
	 *		   Migrated and Refactored by Jeongho Nam <http://samchon.org>
	 *
	 */
	struct Pallet
		: public st_Instance
	{
	public:
		/**
		 * Set placement orientation.
		 */
		void set_orientation(int orientation)
		{
			switch (orientation)
			{
			case 1:
				layout_width = width;
				layout_height = height;
				layout_length = length;
				break;
			case 2:
				layout_width = length;
				layout_height = height;
				layout_length = width;
				break;
			case 3:
				layout_width = length;
				layout_height = width;
				layout_length = height;
				break;
			case 4:
				layout_width = height;
				layout_height = width;
				layout_length = length;
				break;
			case 5:
				layout_width = width;
				layout_height = length;
				layout_length = height;
				break;
			case 6:
				layout_width = height;
				layout_height = length;
				layout_length = width;
				break;
			}
		};
	};
};