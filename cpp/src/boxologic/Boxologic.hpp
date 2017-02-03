#pragma once

#include <boxologic/Pallet.hpp>
#include <boxologic/Box.hpp>

#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <memory>

#include <bws/packer/Wrapper.hpp>
#include <bws/packer/InstanceArray.hpp>

/**
 * @brief A set of programs that calculate the best fit for boxes on a pallet migrated from language C. </p>
 *
 * <ul>
 *	<li> Forked from: https://github.com/exad/boxologic </li>
 * </ul>
 * 
 * @author Bill Knechtel, <br>
 *		   Migrated and Refactored by Jeongho Nam <http://samchon.org>
 */
namespace boxologic
{
	/**
	 * @brief Cumulated lengths of a layer.	
	 * 
	 * @details Scrapped represents an edge of a layer under construction.
	 * 
	 * @author Bill Knechtel, <br>
	 *		   Migrated and Refactored by Jeongho Nam <http://samchon.org>
	 */
	struct Scrappad
	{
	public:
		/**
		 * @brief Cumulated length on the X-axis in 3D.
		 */
		double cumx;

		/**
		 * @brief Cumulated length on the Z-axis in 3D.
		 */
		double cumz;
	};

	/**
	 * @brief A facade class of boxologic.
	 * @details The boloxogic class deducts the best solution of packing boxes to a pallet.
	 *
	 * <ul>
	 *	<li> Reference: https://github.com/exad/boxologic </li>
	 * </ul>
	 *
	 * @author Bill Knechtel, <br>
	 *		   Migrated and Refactored by Jeongho Nam <http://samchon.org>
	 */
	class Boxologic
	{
	private:
		/* ===========================================================
			PARAMETRIC DATA
		=========================================================== */
		/**
		 * A Wrapper to pack instances.
		 */
		std::shared_ptr<bws::packer::Wrapper> wrapper;

		/**
		 * Instances trying to pack into the wrapper.
		 */
		std::shared_ptr<bws::packer::InstanceArray> instanceArray;

		/**
		 * Instances failed to pack by overloading.
		 */
		std::shared_ptr<bws::packer::InstanceArray> leftInstances;

		/* ===========================================================
			BACKGROUND DATA
				- STRUCTURES
				- VARIABLES FOR ITERATION
				- FLAGS FOR TERMINATING ITERATION
				- THE BEST
		==============================================================
			STRUCTURES
		----------------------------------------------------------- */
		/**
		 * @brief A pallet containing {@link Box boxes}.
		 *
		 * @see Wrapper
		 */
		Pallet pallet;

		/** 
		 * @brief Boxes, trying to pack into the #pallet.
		 *
		 * @see Instance
		 */
		std::vector<struct Box> box_array;

		/**
		 * Sum of all boxes' volume.
		 */
		double total_box_volume;

		/**
		 * @brief All different lengths of {@link box_array all box} dimensions along with evaluation values.
		 *
		 * @details 
		 * <p> In other word, the <i>layer_map</i> stores those entries; each {@link Boxbox}'s length on each 
		 * axis as a <i>key</i> (width, height or length) and evaluation value as a <i>value</i>. The evaluation 
		 * value means sum of minimum gaps between the key and other {@link Box boxes}' width, height and length 
		 * </p>
		 *
		 * <code>
		 FOR i to box_array.size()
			 WHILE key IN width, length and height in box_array[i]
			 BEGIN
		 		value := 0
		 		FOR j to box_array.size()
		 			value += min
		 			(
		 				abs(key - box_array[j].width), 
		 				abs(key - box_array[j].height), 
		 				abs(key - box_array[j].length)
		 			)
		 			layer_map.insert({key, value});
			END
		 * </code>
		 * 
		 * <ul>
		 *	<li> key: A dimension value </li>
		 *	<li> value: Evaluation weight value for the corresponding key. </li>
		 * </ul>
		 */
		std::map<double, double> layer_map;
		
		/**
		 * @brief List of Scrapped instances, edges of layers under construction.
		 *
		 * @see Scrapped
		 * @see #scrap_min_z
		 */
		std::list<struct Scrappad> scrap_list;

		/**
		 * @brief The topology Scrapped, the edge of the current layer under construction.
		 *
		 * @see Scrapped
		 * @see #scrap_list
		 */
		std::list<struct Scrappad>::iterator scrap_min_z;

		/* -----------------------------------------------------------
			VARIABLES FOR ITERATIONS
				- BOX
				- LAYER
				- REMAINS & PACKED
				- FINDER & ANALYZER
		--------------------------------------------------------------
		//	BOX
		/////////////////////////////////////////////////////////// */
		/**
		 * @brief Index of the current {@link Box box}.
		 */
		size_t cboxi;

		/**
		 * @brief Candidate {@link Box.layout_width layout_width} of the {@link cboxi current box}.
		 */
		double cbox_layout_width;

		/**
		 * @brief Candidate {@link Box.layout_height layout_height} of the {@link cboxi current box}.
		 */
		double cbox_layout_height;

		/**
		 * @brief Candidate {@link Box.layout_length layout_length} of the {@link cboxi current box}.
		 */
		double cbox_layout_length;
		
		//////////////////////////////////////////////////////////////
		//	LAYER
		//////////////////////////////////////////////////////////////
		/**
		 * @brief Current layer's key on iteration.
		 */
		double layer_thickness;

		/**
		 * @brief Previous layer's key had iterated.
		 */
		double pre_layer;
		
		/**
		 * @brief Key of the unevened layer in the current packing layer.
		 */
		double layer_in_layer;

		/**
		 * @brief Little Z, gotten from {@link Scrapped.cumz cumz} in #min_scrap_z
		 */
		double lilz;

		//////////////////////////////////////////////////////////////
		//	REMAINS & PACKED
		//////////////////////////////////////////////////////////////
		/**
		 * @brief Remained (unfilled) {@link Pallet.layout_height layout_height} of the #pallet.
		 */
		double remain_layout_height;

		/**
		 * @brief Remained (unfilled) {@link Pallet.layout_length layout_length} of the #pallet.
		 */
		double remain_layout_length;

		/**
		 * @brief Packed (filled) {@link Pallet.layout_height layout_height} of the @pallet.
		 */
		double packed_layout_height;

		/**
		 * @brief Packed {@link Pallet.volume volume} of the #pallet
		 */
		double packed_volume;

		//////////////////////////////////////////////////////////////
		//	FINDER & ANALYZER
		//////////////////////////////////////////////////////////////
		size_t boxi;
		size_t bboxi;

		double boxx;
		double boxy;
		double boxz;

		double bboxx;
		double bboxy;
		double bboxz;

		double bfx, bfy, bfz;
		double bbfx, bbfy, bbfz;

		/* -----------------------------------------------------------
			FLAGS FOR TERMINATING ITERATION
		----------------------------------------------------------- */
		/**
		* @brief Whether the packing is on progress.
		*
		* @details The #packing is flag variable for terminating iterations in #iterate_orientations().
		*/
		bool packing;

		/**
		 * Whether packing a layer is done.
		 */
		bool layer_done;

		/**
		 * Whether the current packing layer is evened.
		 */
		bool evened;

		/**
		 * Whether the best solution is deducted.
		 */
		bool packing_best;

		/**
		 * Whether the utilization degree of pallet space is 100%.
		 */
		bool hundred_percent;

		/**
		 * The best orientation of the pallet, which can deduct the {@link best_solution_volume}.
		 */
		int best_orientation;

		/**
		 * The best layer, which can deduct the {@link best_solution_volume}.
		 */
		double best_layer;

		/**
		 * The best volume, fit the best utilization degree of the pallet space.
		 */
		double best_solution_volume;

	public:
		/* ===========================================================
			CONSTRUCTORS
				- CONSTRUCTOR
				- ENCODER & DECODER
		==============================================================
			CONSTRUCTOR
		----------------------------------------------------------- */
		/**
		 * Construct from a wrapper and instances.
		 *
		 * @param wrapper
		 * @param instanceArray
		 */
		Boxologic(std::shared_ptr<bws::packer::Wrapper> wrapper, std::shared_ptr<bws::packer::InstanceArray> instanceArray)
		{
			this->wrapper = wrapper;
			this->instanceArray = instanceArray;
			this->leftInstances = std::make_shared<bws::packer::InstanceArray>();
		};
		
		~Boxologic()
		{
		};

	private:
		/* -----------------------------------------------------------
			ENCODER & DECODER
		----------------------------------------------------------- */
		/**
		 * @brief Encode data.
		 *
		 * @details Encodes {@link bws::packer Packer}'s data to be suitable for the 
		 *			{@link boxologic Boxologic}'s parametric data.
		 */
		void encode()
		{
			// READ_BOXLIST_INPUT
			pallet.width = wrapper->getContainableWidth();
			pallet.height = wrapper->getContainableHeight();
			pallet.length = wrapper->getContainableLength();

			box_array.assign(instanceArray->size(), struct Box());
			layer_map.clear();

			for (size_t i = 0; i < instanceArray->size(); i++)
			{
				std::shared_ptr<bws::packer::Instance> instance = instanceArray->at(i);

				box_array[i].width = instance->getWidth();
				box_array[i].height = instance->getHeight();
				box_array[i].length = instance->getLength();

				box_array[i].volume = instance->getVolume();
				box_array[i].is_packed = false;
			}

			// INITIALIZE
			pallet.volume = pallet.width * pallet.height * pallet.length;
			total_box_volume = 0.0;

			for (size_t x = 0; x < box_array.size(); x++)
				total_box_volume = total_box_volume + box_array[x].volume;

			scrap_list.emplace_back();

			best_solution_volume = 0.0;
			packing_best = false;
			hundred_percent = false;
		};

		/**
		 * @brief Decode data.
		 *
		 * @details Decodes the {@link boxologic Boxologic's optimization result data} to be suitable for the 
		 *			{@link bws::packer Packer}'s own.
		 */
		void decode()
		{
			wrapper->clear();
			leftInstances->clear();

			for (size_t i = 0; i < box_array.size(); i++)
			{
				const Box &box = box_array[i];
				std::shared_ptr<bws::packer::Instance> instance = instanceArray->at(i);

				if (box.is_packed == true)
				{
					bws::packer::Wrap *wrap = new bws::packer::Wrap(wrapper.get(), instance, box.cox, box.coy, box.coz);
					wrap->estimateOrientation(box.layout_width, box.layout_height, box.layout_length);

					// IF HAS THICKNESS
					if (wrapper->getThickness() != 0)
						wrap->setPosition
						(
							wrap->getX() + wrapper->getThickness(),
							wrap->getY() + wrapper->getThickness(),
							wrap->getZ() + wrapper->getThickness()
						);

					wrapper->emplace_back(wrap);
				}
				else
				{
					// NOT WRAPED INSTANCES BY LACK OF VOLUME
					leftInstances->push_back(instance);
				}
			}
		};

	public:
		/* ===========================================================
			MAIN PROCEDURES
				- OPERATORS
				- GETTERS
				- CHECKERS
				- REPORTERS
		==============================================================
			OPERATORS
		------------------------------------------------------------ */
		/**
		 * <p> Pack instances to the #wrapper. </p>
		 *
		 * <p> The #pack is an adaptor method between {@link bws::packer Packer} and {@link boxologic}. 
		 * It encodes data from {@link bws::packer Packer}, deducts the best packing solution decodes the 
		 * optimization result and returns it. </p>
		 *
		 * <p> The optimization result is returned as a Pair like below: </p>
		 * <ul>
		 *	<li> first: The #wrapper with packed instances. </li>
		 *	<li> second: {@link leftInstances Left instances failed to pack} by overloading. </li>
		 * </ul>
		 *
		 * @return A pair of #wrapper with packed instances and 
		 *		   {@link leftInstances instances failed to pack} by overloading.
		 */
		auto pack() -> std::pair<std::shared_ptr<bws::packer::Wrapper>, std::shared_ptr<bws::packer::InstanceArray>>
		{
			encode();

			iterate_orientations();
			report_results();

			decode();

			return{ wrapper, leftInstances };
		};

	private:
		/**
		 * @brief Execute iterations by calling proper functions.
		 *
		 * @details Iterations are done and parameters of the best solution are found.
		 */
		void iterate_orientations() //TODO: Needs a better name yet
		{
			for (int orientation = 1; orientation <= 6; orientation++)
			{
				pallet.set_orientation(orientation);

				// CONSTRUCT LAYERS
				construct_layers();

				// ITERATION IN LAYERS
				for (auto it = layer_map.begin(); it != layer_map.end(); it++)
				{
					// BEGINS PACKING
					iterate_layer(it->first);

					if (packed_volume > best_solution_volume)
					{
						// NEW VOLUME IS THE BEST
						best_solution_volume = packed_volume;
						best_orientation = orientation;
						best_layer = it->first;
					}

					if (hundred_percent)
						break; // SUCCESS TO UTILIZE ALL
				}

				if (hundred_percent)
					break; // SUCCESS TO UTILIZE ALL

						   // IF THE PALLET IS REGULAR CUBE,
				if (pallet.width == pallet.height && pallet.height == pallet.length)
					orientation = 6; // DON'T ITERATE ALL ORIENTATIONS
			}
		};

		/**
		 * @brief Iterate a layer.
		 *
		 * @param layer_thickness Thickness of the iterating layer.
		 */
		void iterate_layer(double thickness)
		{
			// INIT PACKED
			packing = true;

			packed_volume = 0.0;
			packed_layout_height = 0;
			this->layer_thickness = thickness;

			// SET REMAINS FROM PALLET'S DIMENSIONS
			remain_layout_height = pallet.layout_height;
			remain_layout_length = pallet.layout_length;

			// UNPACK ALL BOXES
			for (size_t i = 0; i < box_array.size(); i++)
				box_array[i].is_packed = false;

			do
			{
				// INIT VARS OF LAYER ITERATION
				layer_in_layer = 0;
				layer_done = false;

				// PACK_LAYER AND POST-PROCESS
				pack_layer();

				packed_layout_height += layer_thickness;
				remain_layout_height = pallet.layout_height - packed_layout_height;

				if (layer_in_layer != 0)
				{
					// STORE ORDINARY PACKING VARS
					double pre_packed_y = packed_layout_height;
					double pre_remain_py = remain_layout_height;

					// STORE CAUCLATED RESULTS
					remain_layout_height = layer_thickness - pre_layer;
					packed_layout_height -= layer_thickness + pre_layer;
					remain_layout_length = lilz;
					layer_thickness = layer_in_layer;

					// ITERATION IS NOT FINISHED YET
					layer_done = false;

					// RE-CALL PACK_LAYER
					pack_layer();

					// REVERT TO THE STORED ORDINARIES
					packed_layout_height = pre_packed_y;
					remain_layout_height = pre_remain_py;
					remain_layout_length = pallet.layout_length;
				}

				// CALL FIND_LAYER
				find_layer(remain_layout_height);
			} while (packing);
		};

		/**
		 * @brief Construct layers.
		 *
		 * @details Creates all possible layer heights by giving a weight value to each of them.
		 */
		void construct_layers()
		{
			layer_map.clear();

			for (size_t i = 0; i < box_array.size(); i++)
			{
				const struct Box &box = box_array[i];

				for (size_t j = 1; j <= 3; j++)
				{
					double ex_dim; // STANDARD LENGTH ON THE DIMENSION
					double dimen2; // THE SECOND, LENGTH ON A RESIDUAL DIMENSION
					double dimen3; // THE THIRD, LENGTH ON A RESIDUAL DIMENSION

					double layer_eval = 0; // SUM OF LAYERS (height)

					// FETCH STANDARD DIMENSIONS FROM EACH AXIS
					switch (j)
					{
					case 1:
						ex_dim = box.width;
						dimen2 = box.height;
						dimen3 = box.length;
						break;
					case 2:
						ex_dim = box.height;
						dimen2 = box.width;
						dimen3 = box.length;
						break;
					case 3:
						ex_dim = box.length;
						dimen2 = box.width;
						dimen3 = box.height;
						break;
					}

					// A DIMENSIONAL LENGTH IS GREATER THAN THE PALLET ?
					if (ex_dim > pallet.layout_height || 
						(
							(dimen2 > pallet.layout_width || dimen3 > pallet.layout_length) &&
							(dimen3 > pallet.layout_width || dimen2 > pallet.layout_length)
						))
					{
						// A DIMENSIONAL LENGTH IS GREATER THAN THE PALLET
						continue;
					}

					// WHEN A DUPLICATED LAYER EXISTS, SKIPS
					if (layer_map.count(ex_dim) != 0)
						continue;

					// ABOUT ALL BOXES, FIND THE MINIMUM LENGTH OF GAP ~,
					// STACK ON THE CURRENT LAYER (ADD ON TO THE LAYER_EVAL)
					for (size_t k = 0; k < box_array.size(); k++)
					{
						// SAME INSTANCE WITH THE SAME INDEX
						if (i == k)
							continue;

						const struct Box &my_box = box_array[k];
						double dim_diff = 
							std::min
							({
								abs(ex_dim - my_box.width),
								abs(ex_dim - my_box.height),
								abs(ex_dim - my_box.length)
							});

						layer_eval += dim_diff;
					}

					// RECORD THE SUM
					layer_map[ex_dim] = layer_eval;
				}
			}
		};

		/**
		 * @brief Packs the boxes found and arranges all variables and records properly.
		 * 
		 * @details Update the linked list and the Boxlist[] array as a box is packed.
		 */
		void pack_layer()
		{
			if (layer_thickness == 0)
			{
				packing = false;
				return;
			}
			else if (scrap_list.empty() == true)
				return;

			double lenx, lenz, lpz;

			scrap_list.begin()->cumx = pallet.layout_width;
			scrap_list.begin()->cumz = 0;

			while (true)
			{
				// INIT SCRAP_MIN_Z
				find_smallest_z();

				// FETCH LEFT AND RIGHT OF SCRAP_MIN_Z
				auto &prev = fetch_scrap_min_z_left();
				auto &next = fetch_scrap_min_z_right();

				if (scrap_min_z == scrap_list.end())
				{
					break;
				}
				if (prev == scrap_list.end() && next == scrap_list.end())
				{
					/////////////////////////////////////////////////////////
					// NO LEFT AND RIGHT
					/////////////////////////////////////////////////////////
					//*** SITUATION-1: NO BOXES ON THE RIGHT AND LEFT SIDES ***
					lenx = scrap_min_z->cumx;
					lpz = remain_layout_length - scrap_min_z->cumz;

					// CALL FIND_BOX AND CHECK_FOUND
					find_box(lenx, layer_thickness, remain_layout_height, lpz, lpz);
					check_found();

					// BREAK ?
					if (layer_done) break;
					if (evened) continue;

					// UPDATE CURRENT BOX
					struct Box &box = box_array[cboxi];
					box.cox = 0;
					box.coy = packed_layout_height;
					box.coz = scrap_min_z->cumz;
			
					if (cbox_layout_width == scrap_min_z->cumx)
					{
						// CUMULATE
						scrap_min_z->cumz += cbox_layout_length;
					}
					else
					{
						// CREATE A NEW NODE AND IT'S THE NEW MIN_Z
						// ORDINARY MIN_Z WILL BE SHIFTED TO THE RIGHT
						struct Scrappad scrap =
						{
							cbox_layout_width,
							scrap_min_z->cumz + cbox_layout_length
						};

						// SHIFTS ORDINARY MIN_Z TO RIGHT
						// AND THE NEW NODE'S ITERATOR IS THE NEW MIN_Z FROM NOW ON
						scrap_min_z = scrap_list.insert(scrap_min_z, scrap);
					}
				}
				else if (prev == scrap_list.end())
				{
					/////////////////////////////////////////////////////////
					// NO LEFT, BUT RIGHT
					/////////////////////////////////////////////////////////
					//*** SITUATION-2: NO BOXES ON THE LEFT SIDE ***
					lenx = scrap_min_z->cumx;
					lenz = next->cumz - scrap_min_z->cumz;
					lpz = remain_layout_length - scrap_min_z->cumz;

					// CALL FIND_BOX AND CHECK_FOUND
					find_box(lenx, layer_thickness, remain_layout_height, lenz, lpz);
					check_found();

					// BREAK ?
					if (layer_done) break;
					if (evened) continue;

					// RE-FETCH LEFT AND RIGHT
					next = fetch_scrap_min_z_right();

					// UPDATE CURRENT BOX
					struct Box &box = box_array[cboxi];
					box.coy = packed_layout_height;
					box.coz = scrap_min_z->cumz;

					if (cbox_layout_width == scrap_min_z->cumx)
					{
						box.cox = 0;

						if (scrap_min_z->cumz + cbox_layout_length == next->cumz)
						{
							// RIGHT IS THE NEW MIN_Z
							// ORDINARY MIN_Z WILL BE ERASED
							scrap_min_z = scrap_list.erase(scrap_min_z);
						}
						else
						{
							// CUMULATE
							scrap_min_z->cumz += cbox_layout_length;
						}
					}
					else
					{
						box.cox = scrap_min_z->cumx - cbox_layout_width;

						if (scrap_min_z->cumz + cbox_layout_length == next->cumz)
						{
							// DE-CUMULATE
							scrap_min_z->cumx -= cbox_layout_width;
						}
						else
						{
							

							// CREATE A NEW NODE BETWEEN MIN_Z AND RIGHT
							struct Scrappad scrap =
							{
								scrap_min_z->cumx,
								scrap_min_z->cumz + cbox_layout_length
							};
							scrap_list.insert(next, scrap);

							// UPDATE MIN_Z
							scrap_min_z->cumx -= cbox_layout_width;
						}
					}
				}
				else if (next == scrap_list.end())
				{
					////////////////////////////////////////////////////////
					// NO RIGHT BUT LEFT
					/////////////////////////////////////////////////////////
					//*** SITUATION-3: NO BOXES ON THE RIGHT SIDE ***
					lenx = scrap_min_z->cumx - prev->cumx;
					lenz = prev->cumz - scrap_min_z->cumz;
					lpz = remain_layout_length - scrap_min_z->cumz;

					// CALL FIND_BOX AND CHECK_FOUND
					find_box(lenx, layer_thickness, remain_layout_height, lenz, lpz);
					check_found();

					// BREAK ?
					if (layer_done) break;
					if (evened) continue;

					// RE-FETCH LEFT AND RIGHT
					prev = fetch_scrap_min_z_left();

					// UPDATE CURRENT BOX
					struct Box &box = box_array[cboxi];
					box.coy = packed_layout_height;
					box.coz = scrap_min_z->cumz;
					box.cox = prev->cumx;

					if (cbox_layout_width == scrap_min_z->cumx - prev->cumx)
					{
						if (scrap_min_z->cumz + cbox_layout_length == prev->cumz)
						{
							// LEFT FETCHES MIN_Z'S CUM_X
							prev->cumx = scrap_min_z->cumx;

							// ERASE FROM MIN_Z TO END
							scrap_min_z = scrap_list.erase(scrap_min_z, scrap_list.end());
						}
						else
						{
							// CUMULATE
							scrap_min_z->cumz += cbox_layout_length;
						}
					}
					else
					{
						if (scrap_min_z->cumz + cbox_layout_length == prev->cumz)
						{
							// CUMULATE
							prev->cumx += cbox_layout_width;
						}
						else
						{
							// CREATE A NEW NODE BETWEEN LEFT AND MIN_Z
							struct Scrappad scrap =
							{
								prev->cumx + cbox_layout_width,
								scrap_min_z->cumz + cbox_layout_length
							};
							scrap_list.insert(scrap_min_z, scrap);
						}
					}
				}
				else if (prev->cumz == next->cumz)
				{
					////////////////////////////////////////////////////////
					// LEFT AND RIGHT ARE ALL EXIST -> SAME CUMZ
					/////////////////////////////////////////////////////////
					//*** SITUATION-4: THERE ARE BOXES ON BOTH OF THE SIDES ***
					//*** SUBSITUATION-4A: SIDES ARE EQUAL TO EACH OTHER ***
					lenx = scrap_min_z->cumx - prev->cumx;
					lenz = prev->cumz - scrap_min_z->cumz;
					lpz = remain_layout_length - scrap_min_z->cumz;

					// CALL FIND_BOX AND CHECK_FOUND
					find_box(lenx, layer_thickness, remain_layout_height, lenz, lpz);
					check_found();

					// BREAK ?
					if (layer_done) break;
					if (evened) continue;

					// RE-FETCH LEFT AND RIGHT
					prev = fetch_scrap_min_z_left();
					next = fetch_scrap_min_z_right();

					// UPDATE CURRENT BOX
					struct Box &box = box_array[cboxi];
					box.coy = packed_layout_height;
					box.coz = scrap_min_z->cumz;

					if (cbox_layout_width == scrap_min_z->cumx - prev->cumx)
					{
						box.cox = prev->cumx;

						if (scrap_min_z->cumz + cbox_layout_length == next->cumz)
						{
							// LEFT FETCHES RIGHT'S CUM_X
							prev->cumx = next->cumx;

							// ERASE MIN_Z AND RIGHT
							auto next_of_next = next;
							next_of_next++; // RIGHT'S RIGHT OR END MAYBE

							scrap_min_z = scrap_list.erase(scrap_min_z, next_of_next);
						}
						else
						{
							// CUMULATE
							scrap_min_z->cumz += cbox_layout_length;
						}
					}
					else if (prev->cumx < pallet.layout_width - scrap_min_z->cumx)
					{
						if (scrap_min_z->cumz + cbox_layout_length == prev->cumz)
						{
							// DE-CUMULATE
							scrap_min_z->cumx -= cbox_layout_width;
							box.cox = scrap_min_z->cumx;
						}
						else
						{
							box.cox = prev->cumx;

							// CREATE A NODE BETWEEN LEFT AND MIN_Z
							struct Scrappad scrap =
							{
								prev->cumx + cbox_layout_width,
								scrap_min_z->cumz + cbox_layout_length
							};
							scrap_list.insert(scrap_min_z, scrap);
						}
					}
					else
					{
						if (scrap_min_z->cumz + cbox_layout_length == prev->cumz)
						{
							// CUMULATE
							prev->cumx += cbox_layout_width;
							box.cox = prev->cumx;
						}
						else
						{
							box.cox = scrap_min_z->cumx - cbox_layout_width;

							// CREATE A NODE BETWEEN MIN_Z AND RIGHT
							struct Scrappad scrap =
							{
								scrap_min_z->cumx,
								scrap_min_z->cumz + cbox_layout_length
							};
							scrap_list.insert(next, scrap);

							// UPDATE MIN_Z
							scrap_min_z->cumx -= cbox_layout_width;
						}
					}
				}
				else
				{
					////////////////////////////////////////////////////////
					// LEFT AND RIGHT ARE ALL EXIST
					////////////////////////////////////////////////////////
					//*** SUBSITUATION-4B: SIDES ARE NOT EQUAL TO EACH OTHER ***
					lenx = scrap_min_z->cumx - prev->cumx;
					lenz = prev->cumz - scrap_min_z->cumz;
					lpz = remain_layout_length - scrap_min_z->cumz;

					// CALL FIND_BOX AND CHECK_FOUND
					find_box(lenx, layer_thickness, remain_layout_height, lenz, lpz);
					check_found();

					// BREAK ?
					if (layer_done) break;
					if (evened) continue;

					// RE-FETCH LEFT AND RIGHT
					prev = fetch_scrap_min_z_left();
					next = fetch_scrap_min_z_right();

					// UPDATE CURRENT BOX
					struct Box &box = box_array[cboxi];
					box.coy = packed_layout_height;
					box.coz = scrap_min_z->cumz;
					box.cox = prev->cumx;

					if (cbox_layout_width == scrap_min_z->cumx - prev->cumx)
					{
						if (scrap_min_z->cumz + cbox_layout_length == prev->cumz)
						{
							// LEFT FETCHES MIN_Z'S
							prev->cumx = scrap_min_z->cumx;

							// ERASE MIN_Z
							scrap_min_z = scrap_list.erase(scrap_min_z);
						}
						else
						{
							// CUMULATE
							scrap_min_z->cumz += cbox_layout_length;
						}
					}
					else
					{
						if (scrap_min_z->cumz + cbox_layout_length == prev->cumz)
						{
							// CUMULATE
							prev->cumx += cbox_layout_width;
						}
						else if (scrap_min_z->cumz + cbox_layout_length == next->cumz)
						{
							// DE-CUMULATE
							scrap_min_z->cumx -= cbox_layout_width;
							box.cox = scrap_min_z->cumx;
						}
						else
						{
							// CREATE NODE BETWEEN LEFT AND MIN_Z
							struct Scrappad scrap =
							{
								prev->cumx + cbox_layout_width,
								scrap_min_z->cumz + cbox_layout_length
							};
							scrap_list.insert(scrap_min_z, scrap);
						}
					}
				}
	
				volume_check();
			}
		};

		/**
		 * @brief Find the most proper layer height.
		 *
		 * @details Find the most proper layer height by looking at the unpacked boxes and 
		 *			the remaining empty space available.
		 */
		void find_layer(double thickness)
		{
			// MINIMUM SUM OF LAYERS (height)
			double min_eval = INT_MAX;
	
			layer_thickness = 0;

			for (size_t i = 0; i < box_array.size(); i++)
			{
				const struct Box &box = box_array[i];
				if (box.is_packed) 
					continue;

				for (size_t j = 1; j  <= 3; j++)
				{
					double ex_dim; // STANDARD LENGTH ON THE DIMENSION
					double dim2; // THE SECOND, LENGTH ON A RESIDUAL DIMENSION
					double dim3; // THE THIRD, LENGTH ON A RESIDUAL DIMENSION

					double my_eval = 0;

					// FETCH STANDARD DIMENSIONS FROM EACH AXIS
					switch (j)
					{
					case 1:
						ex_dim = box.width;
						dim2 = box.height;
						dim3 = box.length;
						break;
					case 2:
						ex_dim = box.height;
						dim2 = box.width;
						dim3 = box.length;
						break;
					case 3:
						ex_dim = box.length;
						dim2 = box.width;
						dim3 = box.height;
						break;
					}

					// ABOUT ALL BOXES, FIND THE MINIMUM LENGTH OF GAP ~,
					// STACK ON THE CURRENT LAYER (ADD ON TO THE LAYER_EVAL)
					if (
							ex_dim <= thickness && 
							(
								(dim2 <= pallet.layout_width && dim3 <= pallet.layout_length) || 
								(dim3 <= pallet.layout_width && dim2 <= pallet.layout_length)
							)
						)
					{
						for (size_t k = 0; k < box_array.size(); k++)
						{
							const struct Box &my_box = box_array[k];

							// SAME INSTANCE WITH THE SAME INDEX OR ALREADY PACKED
							if (i == k || my_box.is_packed == true)
								continue;

							double dim_diff =
								std::min
								({
									abs(ex_dim - my_box.width),
									abs(ex_dim - my_box.height),
									abs(ex_dim - my_box.length)
								});
							my_eval += dim_diff;
						}
						if (my_eval < min_eval)
						{
							min_eval = my_eval;
							layer_thickness = ex_dim;
						}
					}
				}
			}
			if (layer_thickness == 0 || layer_thickness > remain_layout_height) 
				packing = false;
		};

		/**
		 * @brief Determine the gap with the samllest z value in the current layer.
		 *
		 * @details Find the most proper boxes by looking at all six possible orientations,
		 *			empty space given, adjacent boxes, and pallet limits.
		 *
		 * @param hmx Maximum available x-dimension of the current gap to be filled.
		 * @param hy Current layer thickness value.
		 * @param hmy Current layer thickness value.
		 * @param hz Z-dimension of the current gap to be filled.
		 * @param hmz Maximum available z-dimension to the current gap to be filled.
		 */
		void find_box(double hmx, double hy, double hmy, double hz, double hmz)
		{
			boxi = -1;
			bboxi = -1;

			bfx = INT_MAX;
			bfy = INT_MAX;
			bfz = INT_MAX;
			bbfx = INT_MAX;
			bbfy = INT_MAX;
			bbfz = INT_MAX;

			for (size_t i = 0; i < box_array.size(); i++)
			{
				const struct Box &box = box_array[i];
				if (box.is_packed)
					continue;

				analyze_box(i, hmx, hy, hmy, hz, hmz, box.width, box.height, box.length);

				// WHEN REGULAR CUBE
				if (box.width == box.length && box.length == box.height)
					continue;

				analyze_box(i, hmx, hy, hmy, hz, hmz, box.width, box.length, box.height);
				analyze_box(i, hmx, hy, hmy, hz, hmz, box.height, box.width, box.length);
				analyze_box(i, hmx, hy, hmy, hz, hmz, box.height, box.length, box.width);
				analyze_box(i, hmx, hy, hmy, hz, hmz, box.length, box.width, box.height);
				analyze_box(i, hmx, hy, hmy, hz, hmz, box.length, box.height, box.width);
			}
		};

		/**
		 * @brief Analyzes each unpacked Box to find the best fitting one to the empty space.
		 * 
		 * @details Used by #find_box() to analyze box dimensions.
		 *
		 * @param index Index of a Box in the #box_array.
		 * 
		 * @param hmx Maximum available x-dimension of the current gap to be filled.
		 * @param hy Current layer thickness value.
		 * @param hmy Current layer thickness value.
		 * @param hz Z-dimension of the current gap to be filled.
		 * @param hmz Maximum available z-dimension to the current gap to be filled.
		 *
		 * @param dim1 X-dimension of the orientation of the box being examined.
		 * @param dim2 Y-dimension of the orientation of the box being examined.
		 * @param dim3 Z-dimension of the orientation of the box being examined.
		 */
		void analyze_box(size_t index, double hmx, double hy, double hmy, double hz, double hmz, double dim1, double dim2, double dim3)
		{
			// OUT OF BOUNDARY RANGE
			if (dim1 > hmx || dim2 > hmy || dim3 > hmz)
				return;

			if (dim2 <= hy && 
					(
						hy - dim2 < bfy ||
						(hy - dim2 == bfy && hmx - dim1 < bfx) ||
						(hy - dim2 == bfy && hmx - dim1 == bfx && abs(hz - dim3) < bfz)
					)
				)
			{
				boxx = dim1;
				boxy = dim2;
				boxz = dim3;
				bfx = hmx - dim1;
				bfy = hy - dim2;
				bfz = abs(hz - dim3);
				boxi = index;
			}
			else if 
				(dim2 > hy &&
					(
						dim2 - hy < bbfy ||
						(dim2 - hy == bbfy && hmx - dim1 < bbfx) ||
						(dim2 - hy == bbfy && hmx - dim1 == bbfx && abs(hz - dim3) < bbfz)
					)
				)
			{
				bboxx = dim1;
				bboxy = dim2;
				bboxz = dim3;
				bbfx = hmx - dim1;
				bbfy = dim2 - hy;
				bbfz = abs(hz - dim3);
				bboxi = index;
			}
		};

		/* -----------------------------------------------------------
			CHECKERS
		----------------------------------------------------------- */
		/**
		 * After finding each box, the candidate boxes and the condition of the layer
		 * are examined.
		 */
		void check_found() //TODO: Find better name for this
		{
			evened = false;

			if (boxi != (size_t)-1)
			{
				cboxi = boxi;
				cbox_layout_width = boxx;
				cbox_layout_height = boxy;
				cbox_layout_length = boxz;
			}
			else
			{
				auto &prev = fetch_scrap_min_z_left();
				auto &next = fetch_scrap_min_z_right();

				if (bboxi != (size_t)-1 && // IN RANGE
					(
						layer_in_layer != 0 || 
						(
							// NO LEFT AND RIGHT EXISTS
							prev == scrap_list.end() && next == scrap_list.end()
						)
					))
				{
					////////////////////////////////////////////
					// ~ OR SCRAP_MIN_Z HAS NO NEIGHBOR
					////////////////////////////////////////////
					if (layer_in_layer == 0)
					{
						pre_layer = layer_thickness;
						lilz = scrap_min_z->cumz;
					}

					cboxi = bboxi;
					cbox_layout_width = bboxx;
					cbox_layout_height = bboxy;
					cbox_layout_length = bboxz;

					layer_in_layer += bboxy - layer_thickness;
					layer_thickness = bboxy;
				}
				else
				{
					if (prev == scrap_list.end() && next == scrap_list.end())
					{
						///////////////////////////////////////////
						// SCRAP_MIN_Z HAS NO NEIGHBOR
						///////////////////////////////////////////
						// IN RANGE & NO NEIGHBOR
						// LAYER HAS DONE.
						layer_done = true;
					}
					else
					{
						evened = true;

						if (prev == scrap_list.end())
						{
							///////////////////////////////////////////
							// NO LEFT, BUT RIGHT
							///////////////////////////////////////////
							// ERASE SCRAP_MIN_Z
							// RIGHT IS THE NEW SCRAP_MIN_Z
							scrap_min_z = scrap_list.erase(scrap_min_z);
						}
						else if (next == scrap_list.end())
						{
							///////////////////////////////////////////
							// NO RIGHT, BUT LEFT
							///////////////////////////////////////////
							// ERASE CURRENT SCRAP_MIN_Z
							// THE LEFT ITEM FETCHES MIN'S CUM_X
							prev->cumx = scrap_min_z->cumx;

							// ERASE FROM MIN_Z TO END
							scrap_min_z = scrap_list.erase(scrap_min_z, scrap_list.end());
						}
						else
						{
							///////////////////////////////////////////
							// LEFT & RIGHT ARE ALL EXIST
							///////////////////////////////////////////
							if (prev->cumz == next->cumz)
							{
								// ----------------------------------------
								// LEFT AND RIGHT'S CUM_Z ARE EQUAL
								// ----------------------------------------
								// LEFT FETCHES THE RIGHT'S CUM_X
								prev->cumx = next->cumx;

								// ERASE MIN AND ITS RIGHT
								auto next_of_next = next;
								next_of_next++;

								scrap_min_z = scrap_list.erase(scrap_min_z, next_of_next);
							}
							else
							{
								// ----------------------------------------
								// LEFT AND RIGHT'S CUM_Z ARE NOT EQUAL
								// ----------------------------------------
								if (prev->cumz == next->cumz)
									prev->cumx = scrap_min_z->cumx;

								// ERASE SCRAP_MIN_Z
								scrap_min_z = scrap_list.erase(scrap_min_z);
							}
						}
					}
				}
			}
		};
		
		/**
		 * After packing of each box, 100% packing condition is checked.
		 */
		void volume_check()
		{
			struct Box &box = box_array[cboxi];

			box.is_packed = true;
			box.layout_width = cbox_layout_width;
			box.layout_height = cbox_layout_height;
			box.layout_length = cbox_layout_length;

			packed_volume += box.volume;

			if (packing_best)
			{
				// BOXOLOGIC DOESN'T MEMORIZE OPTIMIZED ORIENTATION
				// THUS IT NEEDS ADDITIONAL PROCEDURES FOR EXPORTING
				write_box_file();
			}
			else if (packed_volume == pallet.volume || packed_volume == total_box_volume)
			{
				packing = false;
				hundred_percent = true;
			}
		};

		/* -----------------------------------------------------------
			GETTERS
		----------------------------------------------------------- */
		auto fetch_scrap_min_z_left() -> std::list<struct Scrappad>::iterator
		{
			if (scrap_min_z == scrap_list.begin())
				return scrap_list.end();
			else
			{
				auto it = scrap_min_z;
				it--;

				return it;
			}
		};

		auto fetch_scrap_min_z_right() -> std::list<struct Scrappad>::iterator
		{
			auto it = scrap_min_z;
			it++;

			return it;
		};

		/**
		 * @brief Find the first to be packed gap in the layer edge.
		 *
		 * @details Determine the gap with the {@link #scrap_min_z smallest z} value in the current layer.
		 */
		void find_smallest_z()
		{
			scrap_min_z = scrap_list.begin();

			for (auto it = scrap_list.begin(); it != scrap_list.end(); it++)
				if (it->cumz < scrap_min_z->cumz)
					scrap_min_z = it;
		};

		/* -----------------------------------------------------------
			REPORTERS
		----------------------------------------------------------- */
		/**
		 * @brief Determine {@link #box_arrray boxes}.
		 *
		 * Using the parameters found, packs the best solution found and reports.
		 */
		void report_results()
		{
			////////////////////////////////////////////////////
			// BEGINS RE-PACKING FOLLOWING THE BEST VARS
			////////////////////////////////////////////////////
			packing_best = true;

			pallet.set_orientation(best_orientation);
			construct_layers();
			iterate_layer(best_layer);
		};

		/**
		 * @brief Determine a Box.
		 *
		 * @details Transforms the found co-ordinate system to the one entered by the user and 
		 *			write them to the report.
		 */
		void write_box_file()
		{
			struct Box &box = box_array[cboxi];

			double cox;
			double coy;
			double coz;

			double layout_width;
			double layout_height;
			double layout_length;

			switch (best_orientation)
			{
			case 1:
				cox = box.cox;
				coy = box.coy;
				coz = box.coz;
				layout_width = box.layout_width;
				layout_height = box.layout_height;
				layout_length = box.layout_length;
				break;
			case 2:
				cox = box.coz;
				coy = box.coy;
				coz = box.cox;
				layout_width = box.layout_length;
				layout_height = box.layout_height;
				layout_length = box.layout_width;
				break;
			case 3:
				cox = box.coy;
				coy = box.coz;
				coz = box.cox;
				layout_width = box.layout_height;
				layout_height = box.layout_length;
				layout_length = box.layout_width;
				break;
			case 4:
				cox = box.coy;
				coy = box.cox;
				coz = box.coz;
				layout_width = box.layout_height;
				layout_height = box.layout_width;
				layout_length = box.layout_length;
				break;
			case 5:
				cox = box.cox;
				coy = box.coz;
				coz = box.coy;
				layout_width = box.layout_width;
				layout_height = box.layout_length;
				layout_length = box.layout_height;
				break;
			case 6:
				cox = box.coz;
				coy = box.cox;
				coz = box.coy;
				layout_width = box.layout_length;
				layout_height = box.layout_width;
				layout_length = box.layout_height;
				break;
			}

			box.cox = cox;
			box.coy = coy;
			box.coz = coz;
			box.layout_width = layout_width;
			box.layout_height = layout_height;
			box.layout_length = layout_length;
		};
	};
};