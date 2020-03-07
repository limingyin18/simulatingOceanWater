#pragma once

inline constexpr float pi = 3.1415926f;

inline constexpr int pos_modulo(int n, int d) { return (n % d + d) % d; }

template <class T, typename Fun>
auto integrate(int integration_nodes, T x_min, T x_max, Fun &fun)
{
    T dx = (x_max - x_min) / integration_nodes;
    T x  = x_min + 0.5 * dx;

    decltype(fun(0)) result = dx * fun(x); // the first integration node
    for (int i = 1; i < integration_nodes; i++) { // proceed with other nodes, notice `int i= 1`
    x = x + dx;
    result += dx * fun(x);
    }

    return result;
}

// https://github.com/lecopivo/Interpolation

template <int I, class... Ts>
constexpr decltype(auto) get(Ts &&... ts)
{
  return std::get<I>(std::forward_as_tuple(ts...));
}

template <int I>
auto InterpolateNthArgument = [](auto fun, auto interpolation) {
  return [=](auto... x) mutable {
    auto foo = [fun, x...](int i) mutable {
      get<I>(x...) = i;
      return fun(x...);
    };

    auto xI = get<I>(x...);
    return interpolation(foo)(xI);
  };
};

template <int I, class Fun, class... Interpolation>
auto InterpolationDimWise_impl(Fun fun, Interpolation... interpolation)
{

  if constexpr (I == sizeof...(interpolation))
  {
    return fun;
  }
  else
  {
    auto interpol = get<I>(interpolation...);
    return InterpolationDimWise_impl<I + 1>(
        InterpolateNthArgument<I>(fun, interpol), interpolation...);
  }
}

template <int I, int... J, class Fun, class... Interpolation>
auto InterpolationDimWise_impl2(Fun fun, Interpolation... interpolation)
{

  if constexpr (I == sizeof...(J))
  {
    return fun;
  }
  else
  {
    constexpr int K = get<I>(J...);
    auto interpol = get<K>(interpolation...);
    return InterpolationDimWise_impl2<I + 1, J...>(
        InterpolateNthArgument<K>(fun, interpol), interpolation...);
  }
}

template <class... Interpolation>
auto InterpolationDimWise(Interpolation... interpolation)
{
  return [=](auto fun) {
    return InterpolationDimWise_impl<0>(fun, interpolation...);
  };
}

template <int... J, class... Interpolation>
auto InterpolationDimWise2(Interpolation... interpolation)
{
  static_assert(
      sizeof...(J) == sizeof...(Interpolation),
      "The number of interpolations must match the number order indices");

  return [=](auto fun) {
    return InterpolationDimWise_impl2<0, J...>(fun, interpolation...);
  };
}

const auto ConstantInterpolation = [](auto fun) {
  using ValueType = std::remove_reference_t<decltype(fun(0))>;
  return [=](auto x) mutable -> ValueType {
    int ix = (int)round(x);
    return fun(ix);
  };
};

const auto LinearInterpolation = [](auto fun) {
  // The first two lines are here because Eigen types do
  using ValueType = std::remove_reference_t<decltype(fun(0))>;
  return [=](auto x) mutable -> ValueType {
    int ix = (int)floor(x);
    auto wx = x - ix;

    return (wx != 0 ? wx * fun(ix + 1) : 0) +
           (wx != 1 ? (1 - wx) * fun(ix) : 0);
  };
};

const auto CubicInterpolation = [](auto fun) {
  using ReturnType = std::remove_reference_t<decltype(fun(0))>;
  return [=](auto x) mutable -> ReturnType {
    // for explanation see https://en.wikipedia.org/wiki/Cubic_Hermite_spline
    int ix = (int)floor(x);
    auto wx = x - ix;

    if (wx == 0)
      return 1.0 * fun(ix);

    auto pm1 = fun(ix - 1);
    auto p0 = fun(ix);
    auto p1 = fun(ix + 1);
    auto p2 = fun(ix + 2);

    auto m0 = 0.5 * (p1 - pm1);
    auto m1 = 0.5 * (p2 - p0);

    auto t1 = wx;
    auto t2 = t1 * wx;
    auto t3 = t2 * wx;

    return (2.0 * t3 - 3.0 * t2 + 1.0) * p0 + (t3 - 2.0 * t2 + t1) * m0 +
           (-2.0 * t3 + 3.0 * t2) * p1 + (t3 - t2) * m1;
  };
};

template<class T>
std::enable_if_t<std::is_arithmetic_v<T>>  fft(std::complex<T>*x, size_t N, size_t step)
{
    assert(N % 2 == 0);

    // reverse order, rader algorithm
	for(size_t i = 0, j = 0; i < N - 1; ++i)        
	{
		if(i < j)
		{
            swap(x[i*step], x[j*step]);
		}

        // next j
        size_t k = N/2;
		while( k <= j)                 // bit = 1   
		{           
			j = j - k;                 // set bit to 0
			k = k / 2;                 // next bit
		}
		j = j + k;                     //set bit 0 to 1
	}

    // butterfly algorithm
    size_t stages = static_cast<size_t>(log2(N));
    for(size_t s = 1; s <= stages; s++)                    // stage
    {                                        
        size_t d = static_cast<size_t>(pow(2, s-1));                            // distance between points in a butterfly
        complex<T> w = {static_cast<T>(1), static_cast<T>(0)}; // weight
        for(size_t k = 0; k < d; k++)                      // butterfly kind
        {
            for(size_t i = k; i < N; i += d*2)             // butterfly 
            {
                size_t j = i + d;                          // i, j index of butterfly inputs
                complex<T> temp = x[j*step] * w;
                x[j*step] = x[i*step] - temp;
                x[i*step] = x[i*step] + temp;
            }
            w = w *complex<T>{std::cos(pi/d), -std::sin(pi/d)};
        }
    }
}

template<class T>
std::enable_if_t<std::is_arithmetic_v<T>> ifft(std::complex<T>*x, size_t N, size_t step)
{
    // conjugate the complex numbers
    for(size_t i = 0; i < N; ++i)
        x[i*step] = std::conj(x[i*step]);
 
    // forward fft
    fft( x , N, step);
 
    // conjugate the complex numbers again and scale the numbers
    for(size_t i = 0; i < N; ++i)
        x[i*step] = std::conj(x[i*step])/static_cast<T>(N);
}

template<class T>
std::enable_if_t<std::is_arithmetic_v<T>> ifft2(std::complex<T>*x, size_t N, size_t M)
{
    	// row fft
	for (int m = 0; m < M; ++m)
		ifft(&x[m*N+0], N, 1);

	// col fft
	for (int n = 0; n < N; ++n)
		ifft(&x[0*N+n], M, N);
}