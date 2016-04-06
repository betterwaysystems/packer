#pragma once
#include <bws/packer/API.hpp>

#include <bws/packer/WrapperArray.hpp>

#include <unordered_map>

namespace bws
{
namespace packer
{
	class Packer;
	class WrapperGroup;
	class InstanceArray;

	/**
	 * @brief Sequence list of Wrapper objects.
	 *
	 * @author Jeongho Nam <http://samchon.org>
	 */
	class GAWrapperArray
		: public WrapperArray
	{
	private:
		typedef WrapperArray super;

	protected:
		/**
		 * @brief Instance objects to be wrapped.
		 */
		std::shared_ptr<InstanceArray> instanceArray;
		
		std::unordered_map<std::string, std::shared_ptr<WrapperGroup>> result;

		/**
		 * @brief Price of this sequence list.
		 *
		 * @details It stores price deducted by the packing process to block repeated operation.
		 */
		double price;

		/**
		 * @brief Validity of this sequence list.
		 *
		 * @details It stores validity of the sequence list to block repeated operation.
		 */
		bool valid;

	public:
		/* -----------------------------------------------------------
			CONSTRUCTORS
		----------------------------------------------------------- */
		/**
		 * @brief Construct from instances.
		 *
		 * @param instanceArray Instances to be wrapped.
		 */
		GAWrapperArray(std::shared_ptr<InstanceArray>);
		
		/**
		 * @brief Copy Constructor.
		 */
		GAWrapperArray(const GAWrapperArray &);
		virtual ~GAWrapperArray() = default;

	protected:
		/**
		 * @brief Construct result, by packing process.
		 */
		void constructResult();

	public:
		/* -----------------------------------------------------------
			GETTERS
		----------------------------------------------------------- */
		/**
		 * @brief Get optimization result.
		 *
		 * @return Const reference of result map.
		 */
		auto getResult() const -> const std::unordered_map<std::string, std::shared_ptr<WrapperGroup>>&;

		/**
		 * @brief Whether this sequence list is inferior?
		 */
		auto operator<(const GAWrapperArray &) const -> bool;
	};
};
};