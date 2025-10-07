#pragma once

#if defined(_MSVC_LANG) && _MSVC_LANG > __cplusplus
#define CPP_VERSION _MSVC_LANG
#else
#define CPP_VERSION __cplusplus
#endif

#if CPP_VERSION >= 202302L
    #define HAS_CPP23
#endif
#if CPP_VERSION >= 202002L
    #define HAS_CPP20
#endif
#if CPP_VERSION >= 201703L
    #define HAS_CPP17
#endif
#if CPP_VERSION >= 201402L
    #define HAS_CPP14
#else
    #error "Bro update your computer"
#endif

#ifdef HAS_CPP20
	#define _requires(...) requires __VA_ARGS__
#else
	#define _requires(...)
#endif

#include <inttypes.h>
#include <numbers>

#include <iostream>
#include <fstream>

#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <stack>
#include <queue>

#include <functional>

#include <memory>
#include <condition_variable>
#include <semaphore>

#include <regex>
#include <chrono>

#ifdef HAS_CPP20
#include <ranges>
#include <format>
#endif
#ifdef HAS_CPP17
#include <filesystem>
#include <optional>
#include <variant>
#endif

// ################################################################## ALIASES ##################################################################
#ifdef HAS_CPP20
namespace stdv = std::views;
namespace stdr = std::ranges;
#endif
#ifdef HAS_CPP17
namespace fs   = std::filesystem;
#endif
namespace stdc = std::chrono;
using namespace std::string_literals;
using namespace std::chrono_literals;

using u8  = uint8_t ;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8  =  int8_t ;
using i16 =  int16_t;
using i32 =  int32_t; 
using i64 =  int64_t;
using f32 =  float  ;
using f64 =  double ;
	
namespace std
{
	template <typename K, typename V>
	using hmap = std::unordered_map<K, V>;
	template <typename V>
	using hset = std::unordered_set<V>;

	namespace chrono
	{
		using clock = high_resolution_clock;
	}
}

template <typename T>
using uptr = std::unique_ptr<T>;
template <typename T>
using sptr = std::shared_ptr<T>;

using callable = std::function<void(void)>;
template <typename T>
using consumer = std::function<void(T)>;
template <typename T>
using provider = std::function<T(void)>;
// ################################################################## ALIASES ##################################################################


// ################################################################## COORD ##################################################################
template <typename T, size_t D, bool = std::is_trivially_destructible<T>::value>
struct coord;

template <typename T>
struct coord<T, 2, true>
{
	union { T x, col; };
	union { T y, row; };
};
template <typename T>
struct coord<T, 2, false>
{
	T x, y;
};
template <typename T>
struct coord<T, 3, true>
{
	union { T x, r; };
	union { T y, g; };
	union { T z, b; };
};
template <typename T>
struct coord<T, 3, false>
{
	T x, y, z;
};
template <typename T>
struct coord<T, 4, true>
{
	union { T x, r; };
	union { T y, g; };
	union { T z, b; };
	union { T w, a; };
};
template <typename T>
struct coord<T, 4, false>
{
	T x, y, z, w;
};


template <typename T>
coord<T, 2> operator+(const coord<T, 2>& a, const coord<T, 2>& b) {
	return { a.x + b.x, a.y + b.y };
}
template <typename T>
coord<T, 3> operator+(const coord<T, 3>& a, const coord<T, 3>& b) {
	return { a.x + b.x, a.y + b.y, a.z + b.z };
}
template <typename T>
coord<T, 4> operator+(const coord<T, 4>& a, const coord<T, 4>& b) {
	return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}
// ################################################################## COORD ##################################################################



// CASTING
template <typename T, typename X>
inline constexpr T to(const X& x) { return static_cast<T>(x); }
template <typename T, typename X, typename _X>
inline constexpr T to(const stdc::duration<_X, X>& x) { return stdc::duration_cast<T>(x); }


// CONCEPTS
#ifdef HAS_CPP17
template <typename T, typename Variant>
struct variant_contains : std::false_type {};
template <typename T, typename... Types>
struct variant_contains<T, std::variant<Types...>> : std::disjunction<std::is_same<T, Types>...> {};
#endif
#ifdef HAS_CPP20
template <typename T, typename Variant>
concept in_variant = variant_contains<T, Variant>::value;
template <typename T, typename... Variant>
concept in_variants = (variant_contains<T, Variant>::value && ...);

template <typename T, typename U>
concept not_same_as = !std::same_as<T, U>;
template <typename T>
concept not_void = !std::is_void_v<T>;
#endif

#ifdef HAS_CPP17
template <typename T, typename... Args>
constexpr bool is_one_of = (std::is_same_v<T, Args> || ...);
template <auto Value, auto... Accepted>
constexpr bool v_is_one_of = ((Value == Accepted) || ...);
#endif


// BINDING
#define BIND(fn) [this]() { this->fn(); }
template <class C, typename R = void, typename I> _requires(not_void<I>)
constexpr std::function<R(I)> bind(C* _this, R(C::* fn)(I)) {
	return [_this, fn](I data) -> R { return (_this->*fn)(data); };
}
template <class C, typename R = void>
constexpr std::function<R(void)> bind(C* _this, R(C::* fn)()) {
	return [_this, fn]() -> R { return (_this->*fn)(); };
}


// VISITOR
#ifdef HAS_CPP17
template <class... Ts>
struct visitor : Ts... { using Ts::operator()...; };
#endif


// VARIANT
#ifdef HAS_CPP17
template <typename V, typename... T>
bool inline constexpr holds(const std::variant<T...>& var) { return std::holds_alternative<V>(var); }
template <typename V, typename... T> _requires(in_variants<V, T...>)
bool inline constexpr holds(const T&... vars) { return (... && std::holds_alternative<V>(vars)); }
#endif


// CHRONO
std::ostream& print_time(const stdc::nanoseconds& time, std::ostream& out = std::cout);


// FILE
#ifdef HAS_CPP17
std::string ReadFile(const fs::path& file);
void ShowInExplorer(const fs::path& path);
#endif


// ################################################################## EXPECTED ##################################################################
#ifdef HAS_CPP17
template <typename E, typename U>
struct expected
{
	std::variant<E, U> content;

	expected(const E& e) { content = e; }
	expected(const U& u) { content = u; }
	expected(const expected<E, U>& other) { content = other.content; }
	void operator=(const expected<E, U>& other) { content = other.content; }
	~expected() {};

	std::optional<E> getE() {
		return std::holds_alternative<E>(content) ? std::optional<E>{ std::get<E>(content) } : std::nullopt;
	}
	std::optional<U> getU() {
		return std::holds_alternative<U>(content) ? std::optional<U>{ std::get<U>(content) } : std::nullopt;
	}
	E& _getE() { return std::get<E>(content); }
	U& _getU() { return std::get<U>(content); }
	E&& _consumeE() { return std::move(std::get<E>(content)); }
	U&& _consumeU() { return std::move(std::get<U>(content)); }
	template <typename T> _requires(in_variant<T, std::variant<E, U>>)
	std::optional<T> get() {
		return std::holds_alternative<T>(content) ? std::get<T>(content) : std::nullopt;
	}

	bool success() const { return std::holds_alternative<E>(content); }
	operator bool() const { return success(); }
};
#endif
// ################################################################## EXPECTED ##################################################################


// ################################################################## TREE ##################################################################

// ################################################################## TREE ##################################################################


// ################################################################## LOGGING ##################################################################
#ifdef _DEBUG
#define LOG(x) std::cout << x
#define LOGE(x) std::cerr << x
#else
#define LOG(x)
#define LOGE(x)
#endif

template <typename T> /*requires std::is_arithmetic_v<T>*/
std::ostream& operator<<(std::ostream& out, const std::vector<T>& data)
{
	out << '[';
	for (size_t i = 0; i + 1 < data.size(); i++)
		out << data[i] << ", ";
	if (!data.empty())
		out << data.back();
	out << ']';
	return out;
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const coord<T, 2>& c) {
#ifdef HAS_CPP20
	return out << std::format("[{}:{}]", c.x, c.y);
#else
	return out << '[' << c.x << ':' << c.y << ']';
#endif
}
template <typename T>
std::ostream& operator<<(std::ostream& out, const coord<T, 3>& c) {
#ifdef HAS_CPP20
	return out << std::format("[{}:{}:{}]", c.x, c.y, c.z);
#else
	return out << '[' << c.x << ':' << c.y << ':' << c.z << ']';
#endif
}
template <typename T>
std::ostream& operator<<(std::ostream& out, const coord<T, 4>& c) {
#ifdef HAS_CPP20
	return out << std::format("[{}:{}:{}:{}]", c.x, c.y, c.z, c.w);
#else
	return out << '[' << c.x << ':' << c.y << ':' << c.z << ':' << c.w << ']';
#endif
}
// ################################################################## LOGGING ##################################################################