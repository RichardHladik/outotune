// guaranteed to be 0 ... _count by the standard
enum class MidiMode : int {
	absolute,
	relative,
	_count,
};

enum class pId : int {
	bufferedStart,
	pitch = bufferedStart,
	nearest,
	corrected,
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
