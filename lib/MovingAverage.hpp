#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H
/**
 * Author	: Nico Verduin (nico.verduin@ziggo.nl)
 * Date		: 31-8-2016
 *
*/

template <typename T_ty> struct TypeInfo { static const char * name; };
template <typename T_ty> const char * TypeInfo<T_ty>::name = "unknown";


template<class T>
class MovingAverage {
private:
	T 			elements[100];     			// Type pointer elements in our array
	uint16_t 	_currentIndex;			// current index
	int32_t 	_sum_32;				// sum for rest
	bool    	_loopedThrough;			// used to determine if we went through the whole array
	uint16_t 	_numberOfElements;		// number of elements in our array

public:
	MovingAverage(uint16_t);			// constructor
	~MovingAverage();					// destructor
	T CalculateMovingAverage(T);		// Calculate new moving Average
};
/**
 * @name MovingAverage Constructor
 * @param numberOfElements number of elements in the array
 * Creates dynamically an array of our type values and initializes the array
 * and some local variables. There is no destructor as this is quite useless
 * due to lack of garbage collection in Arduino
 */
template<class T>
MovingAverage<T>::MovingAverage(uint16_t numberOfelements) {

	//
	// allocate an array of size T
	//
	_numberOfElements = numberOfelements;

	for (uint8_t i = 0; i < _numberOfElements; i++) {
		elements[i] = 0.0;
	}
	_loopedThrough 	= false;				// this becomes true when the array is minimally filled once
	_currentIndex 	= 0;					// start with index 0

	_sum_32 = 0;
	//_sum = 0.0;
}
/**
 * @name ~MovingAverage Constructor
 * frees dynamic allocated memory
 */
template<class T>
MovingAverage<T>::~MovingAverage() {
	
}
/**
 * @name CalculateMovingAverage
 * @param newValue	value to be added to array
 * @returns T returns the new moving average.
 * During the first filling of the elements array the moving average is determined on a less
 * amount of entries. This function keeps track of this situation and calculates the correct
 * average on the number of items listed.
 */

template<class T>
T MovingAverage<T>::CalculateMovingAverage(T newValue) {
	//
	// check if index is at our last entry in the array
	//
	if (_currentIndex >= _numberOfElements) {
		_currentIndex 	= 0;						// reset index
		_loopedThrough 	= true;						// we looped through at least once
	}

	_sum_32 = _sum_32 - elements[_currentIndex] + newValue;

	elements[_currentIndex] = newValue;

	_currentIndex++;

	if (_loopedThrough) {
		return (T) (_sum_32 / _numberOfElements);
	} else {
		return (T) (_sum_32 / _currentIndex);
	}
}

#endif