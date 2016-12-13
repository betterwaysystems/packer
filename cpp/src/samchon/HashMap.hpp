#pragma once
#include <unordered_map>

namespace samchon
{
	/**
	 * @brief Customized std::unordered_map.
	 *
	 * @details
	 * <p> HashMap is a std::unordered_map some methods are modified. </p>
	 *
	 * <ul>
	 *	<li>Addicted methods</li>
	 *	<ul>
	 *		<li> has(find(key) != end()) method is addicted </li>
	 *		<li> at method is addicted </li>
	 *		<li> set method is addicted </li>
	 *		<li> pop method is addicted </li>
	 *	</ul>
	 *	<li>Modified methods</li>
	 *	<ul>
	 *		<li> at was deprecated (get is different with at) </li>
	 *	</ul>
	 * </ul>
	 *
	 * \par [Inherited]
	 * <p> Unordered maps are associative containers that store elements formed by the combination of a key 
	 * value and a mapped value, and which allows for fast retrieval of individual elements based on their 
	 * keys. </p>
	 *
	 * <p> In an unordered_map, the key value is generally used to uniquely identify the element, while the 
	 * mapped value is an object with the content associated to this key. Types of key and mapped value may 
	 * differ. </p>
	 *
	 * <p> Internally, the elements in the unordered_map are not sorted in any particular order with respect 
	 * to either their key or mapped values, but organized into buckets depending on their hash values to 
	 * allow for fast access to individual elements directly by their key values (with a constant average 
	 * time complexity on average). </p>
	 *
	 * <p> unordered_map containers are faster than map containers to access individual elements by their 
	 * key, although they are generally less efficient for range iteration through a subset of their 
	 * elements. </p>
	 *
	 * <p> Unordered maps implement the direct access operator (operator[]) which allows for direct access of 
	 * the mapped value using its key value as argument. </p>
	 *
	 * <p> In the reference for the unordered_map member functions, these same names (Key, T, Hash, Pred and 
	 * Alloc) are assumed for the template parameters. </p>
	 *
	 * <p> Iterators to elements of unordered_map containers access to both the key and the mapped value. 
	 * For this, the class defines what is called its value_type, which is a pair class with its first value 
	 * corresponding to the const version of the key type (template parameter Key) and its second value 
	 * corresponding to the mapped value (template parameter T): </p>
	 *
	 * @code{.cpp}
	 * typedef pair<const Key, T> value_type;
	 * @endcode
	 *
	 * <p> Iterators of a unordered_map container point to elements of this value_type. Thus, for an iterator 
	 * called it that points to an element of a map, its key and mapped value can be accessed respectively 
	 * with: </p>
	 *
	 * @code{.cpp}
	 * unordered_map<Key,T>::iterator it;
	 * (*it).first;             // the key value (of type Key)
	 * (*it).second;            // the mapped value (of type T)
	 * (*it);                   // the "element value" (of type pair<const Key,T>)
	 * @endcode
	 *
	 * <p> Naturally, any other direct access operator, such as -> or [] can be used, for example: </p>
	 *
	 * @code{.cpp}
	 * it->first;               // same as (*it).first   (the key value)
	 * it->second;              // same as (*it).second  (the mapped value) 
	 * @endcode
	 *
	 *	\li Reference: http://www.cplusplus.com/reference/unordered_map/unordered_map/
	 *
	 * @tparam Key Type of the key values. Each element in an unordered_map is uniquely identified by its key 
	 *			   value. Aliased as member type unordered_map::key_type.
	 *
	 * @tparam T Type of the mapped value. Each element in an unordered_map is used to store some data as its 
	 *			 mapped value. Aliased as member type unordered_map::mapped_type. Note that this is not the 
	 *			 same as unordered_map::value_type.
	 *
	 * @tparam Hash A unary function object type that takes an object of type key type as argument and 
	 *				returns a unique value of type size_t based on it. This can either be a class implementing 
	 *				a function call operator or a pointer to a function (see constructor for an example). 
	 *				This defaults to hash<Key>, which returns a hash value with a probability of collision 
	 *				approaching 1.0/std::numeric_limits<size_t>::max(). The unordered_map object uses the 
	 *				hash values returned by this function to organize its elements internally, speeding up 
	 *				the process of locating individual elements. Aliased as member type unordered_map::hasher.
	 *
	 * @tparam Alloc Type of the allocator object used to define the storage allocation model. By default, 
	 *				 the allocator class template is used, which defines the simplest memory allocation model 
	 *				 and is value-independent. Aliased as member type unordered_map::allocator_type.
	 *
	 * @author Jeongho Nam <http://samchon.org>
	 */
	template <typename Key, typename T, 
			  typename Hash = std::hash<Key>, typename Pred = std::equal_to<Key>, 
			  typename Alloc = std::allocator<std::pair<const Key, T> > >
	class HashMap
		: public std::unordered_map<Key, T, Hash, Pred, Alloc>
	{
	private:
		typedef std::unordered_map<Key, T, Hash, Pred, Alloc> super;

		//PROHIBIT ACCESS BY AT
		auto at(const Key &) -> T& = delete;
		auto at(const Key &) const -> const T& = delete;

	public:
		using super::super;

		/**
		 * @brief Whether have the item or not
		 *
		 * @details
		 * Indicates whether a map has an item having the specified identifier.
		 *
		 * @param key Key value of the element whose mapped value is accessed.
		 * @return Whether the map has an item having the specified identifier
		 */
		auto has(const Key &key) const -> bool
		{
			return count(key) != 0;
		};

		/**
		 * @brief Get element
		 *
		 * @details
		 * Returns a reference to the mapped value of the element identified with key
		 *
		 * @warning
		 * get is different with std::map's at. 
		 * get does not create object but throws excention if the matched key doesn't exist.
		 *
		 * @param key Key value of the element whose mapped value is accessed.
		 * @throw exception out of range
		 * @return A reference object of the mapped value (T)
		 */
		auto get(const Key &key) -> T&
		{
			return find(key)->second;
		};
		auto get(const Key &key) const -> const T&
		{
			return find(key)->second;
		};

		/**
		 * @brief Set element
		 *
		 * @details
		 * <p> Set an item as the specified identifier. </p>
		 * 
		 * <p>If the identifier is already in map, change value of the identifier. <br/>
		 * If not, then insert the object with the identifier. </p>
		 * 
		 * @param key Key value of the element whose mapped value is accessed.
		 * @param val Value, the item.
		 */
		void set(const Key &key, const T &val)
		{
			iterator it = find(key);
			if (it != end())
				erase(it);

			insert({ key, val });
		};
		void set(const Key &key, const T &&val)
		{
			iterator it = find(key);
			if (it != end())
				erase(it);

			insert({ key, val });
		};

		/**
		 * @brief Pop item
		 * @details Removes an item having specified key and returns the removed element.
		 *
		 * @return An item released by pop
		 */
		auto pop(const Key &key) -> T
		{
			iterator it = find(key);
			T val = it->second;

			erase(it);

			return val;
		};
	};
};