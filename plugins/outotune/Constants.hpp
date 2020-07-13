#include "Color.hpp"
#include "world/constantnumbers.h"

enum class MidiMode : int {
	absolute,
	relative,
	_count,
};

enum class pId : int {
	bufferedStart,
	pitch = bufferedStart,
	bufferedEnd,

	midiMode = bufferedEnd,
	passThrough,

	_count,
	countBuffered = bufferedEnd - bufferedStart,
};

// assumes EnumType has int as the underlying type and all the enum values are
// consecutive numbers from 0 to EnumType::_count
template<typename EnumType, typename T> EnumType castToEnum(T t, EnumType def) {
	EnumType e = static_cast<EnumType>(t);
	return ((int)e >= 0 && (int)e < (int)EnumType::_count) ? e : def;
}

const float FREQ_MIN = world::kFloorF0;
const float FREQ_MAX = 1600;

// reference note for relative mode, the note that is interpreted as identity
const float REFERENCE_NOTE = 60;
