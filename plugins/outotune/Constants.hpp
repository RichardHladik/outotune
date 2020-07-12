#define DISTRHO_PLUGIN_NUM_PARAMETERS 5
#define DISTRHO_PLUGIN_START_BUFFERED_PARAMETERS 0
#define DISTRHO_PLUGIN_NUM_BUFFERED_PARAMETERS 3

enum class MidiMode : int {
	absolute = 0,
	relative = 1,
	_count = 2,
};

enum {
	pPitch
};

// assumes EnumType has int as the underlying type and all the enum values are
// consecutive numbers from 0 to EnumType::_count
template<typename EnumType, typename T> EnumType castToEnum(T t, EnumType def, bool &ok) {
	EnumType e = static_cast<EnumType>(t);
	ok = true;
	if ((int)e < 0 || (int)e >= EnumType::_count) {
		e = def;
		ok = false;
	}
	return e;
}

template<typename EnumType, typename T> EnumType castToEnum(T t, EnumType def) {
	return castToEnum<EnumType, T>(t, def);
}
