#ifndef DATA_HPP
#define DATA_HPP





/*
 * data class for testing algorithm.
 * gives algorithm as argument
 */
template<typename Struct>
class Data: public Struct
{
public:
	typedef Struct data_type;

	Data();
	~Data();

	Data &update();
	unsigned int getN() const;
	Data &next();

};



template<typename T>
Data<T>::Data() {}

template<typename T>
Data<T>::~Data() {}

template<typename T>
Data<T> &Data<T>::update() {}

template<typename T>
unsigned int Data<T>::getN() const
{
	return 0;
}

template<typename T>
Data<T> &Data<T>::next() {}





#endif

