#ifndef CLEVER_IOSTREAM_FUNCTION_HPP
#define CLEVER_IOSTREAM_FUNCTION_HPP

#include <array>
#include <vector>
#include <utility>


namespace clever
{
	template<class Ostream, typename T>
	inline Ostream &print(Ostream &os, T const &t)
	{
		return os << t;
	}

	template<class Ostream, typename T>
	inline Ostream &print(Ostream &os, T* t)
	{
		return os << '&' << *t;
	}


	template<class Ostream, typename T>
	Ostream &print(
		Ostream &os, std::vector<T> const &vec,
		std::string const &term = ", "
	)
	{
		os << "( ";
		if(!vec.empty()) {
			print(os, vec.front());
			for(auto b = vec.begin()+1, e = vec.end(); b != e; ++b) {
				print(os << term, *b);
			}
		}
		os << " )";
		return os;
	}


	template<class Ostream, typename T, typename U>
	inline Ostream &print(Ostream &os, std::pair<T, U> const &pair)
	{
		return print(
			print(os << "< ", pair.first) <<
			", ", pair.second
		) << " >";
	}

	template<class Ostream, typename T, typename U>
	inline Ostream &print(
		Ostream &os, std::pair<T, U> const &pair,
		std::string const &term
	)
	{
		return print(
			print(os << "< ", pair.first) <<
			term, pair.second
		) << " >";
	}


	template<class Ostream, typename T, size_t N>
	Ostream &print(
		Ostream &os, std::array<T, N> const &arr,
		std::string const &term = ", "
	)
	{
		static_assert(N > 0);

		os << "< " << arr[0];
		for(size_t i = 1; i < N; ++i) {
			print(os << term, arr[i]);
		}
		os << " >";
		return os;
	}


	template<class Ostream, class Matrix>
	Ostream &print_matrix(
		Ostream &os, Matrix const &m, int w, int h,
		std::string const &term = "\t"
	)
	{
		for(int i = 0; i < h; ++i) {
			for(int j = 0; j < w; ++j) {
				print(os, m[i][j]) << '\t';
			}
			os << '\n';
		}
		return os;
	}

}



template<class Ostream, typename T>
inline Ostream &operator<<(Ostream &os, std::vector<T> const &vec)
{
	return clever::print(os, vec);
}

template<class Ostream, typename T, typename U>
inline Ostream &operator<<(Ostream &os, std::pair<T, U> const &pair)
{
	return clever::print(os, pair);
}

template<class Ostream, typename T, size_t N>
inline Ostream &operator<<(Ostream &os, std::array<T, N> const &arr)
{
	return clever::print(os, arr);
}





#endif
