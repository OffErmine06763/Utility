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


#ifdef LEAK_CHECK
#define LEAK_STMT(x) x
#else
#define LEAK_STMT(x)
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
	
namespace std {
	template <typename K, typename V>
	using hmap = std::unordered_map<K, V>;
	template <typename V>
	using hset = std::unordered_set<V>;

	namespace chrono {
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
template <typename T, typename NonVariant>
struct variant_contains : std::false_type {};
template <typename T, typename... Types>
struct variant_contains<T, std::variant<Types...>> : std::disjunction<std::is_same<T, Types>...> {};
#endif
#ifdef HAS_CPP20
template <typename T, typename Variant>
concept in_variant = variant_contains<T, Variant>::value;
#endif

#ifdef HAS_CPP17
template <typename T, typename... Args>
constexpr bool is_one_of = (std::is_same_v<T, Args> || ...);
template <auto Value, auto... Accepted>
constexpr bool val_is_one_of = ((Value == Accepted) || ...);
#endif


// BINDING
#define BIND(fn) [this]() { this->fn(); }
template <class C, typename R = void, typename I> _requires((!std::is_void_v<I>))
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
// The requires are not necessary, but help with error messages, pointing directly to the function call, rather then in here
template <typename T, typename... VTs> _requires(is_one_of<T, VTs...>)
bool inline constexpr holds(const std::variant<VTs...>& var) { return std::holds_alternative<T>(var); }
template <typename T, typename... Vs> _requires((variant_contains<T, Vs>::value && ...))
bool inline constexpr holds(const Vs&... vars) { return (... && std::holds_alternative<T>(vars)); }
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
template <typename T>
class BST
{
public:
	struct Node {
		T val;
		Node* left = nullptr, *right = nullptr;
		Node(const T& x) : val(x) { LEAK_STMT(counter++); }
		Node(T&& x) : val(std::move(x)) { LEAK_STMT(counter++); }
		~Node() { LEAK_STMT(counter--); }

		LEAK_STMT(static int counter);
	};

public:
	BST() = default;
	BST(const BST<T>& other) {
		other.PreOrder([this](const Node* node) -> bool { Insert(node->val); return false; });
	}
	BST(BST<T>&& other) { m_Root = other.m_Root; other.m_Root = nullptr; }
	~BST() { Destroy(); }

	void Insert(const T&  el) { Insert(new Node(el)); }
	void Insert(      T&& el) { Insert(new Node(std::move(el))); }
	void Insert(Node* node)   { m_Root = Insert(m_Root, node); }

	bool PreOrder(const std::function<bool(Node*)>& cb) { return PreOrder(m_Root, cb); }
	bool PreOrder(const std::function<bool(const Node*)>& cb) const { return PreOrder(m_Root, cb); }
	bool PostOrder(const std::function<bool(Node*)>& cb) { return PostOrder(m_Root, cb); }
	bool PostOrder(const std::function<bool(const Node*)>& cb) const { return PostOrder(m_Root, cb); }
	bool InOrder(const std::function<bool(Node*)>& cb) { return InOrder(m_Root, cb); }
	bool InOrder(const std::function<bool(const Node*)>& cb) const { return InOrder(m_Root, cb); }
	
	bool Search(const T& key) { return Search(m_Root, key) != nullptr; }

	void Delete(const T& key) { m_Root = Delete(m_Root, key); }

	void Destroy() { Destroy(m_Root); m_Root = nullptr; }

	constexpr void swap(BST<T>& other) noexcept {
		auto tmp = other.m_Root;
		other.m_Root = m_Root;
		m_Root = tmp;
	}

	Node* GetRoot() const { return m_Root; }

private:
	Node* Insert(Node* root, Node* node) {
		if (root == nullptr) return node;

		if (node->val > root->val)
			root->right = Insert(root->right, node);
		else
			root->left = Insert(root->left, node);
		size++;
		return root;
	}

	
	bool InOrder  (Node* root, const std::function<bool(Node*)>& cb) {
		if (root == nullptr) return false;

		if (InOrder(root->left , cb)) return true;
		if (cb(root)) return true;
		if (InOrder(root->right, cb)) return true;
		return false;
	}
	bool InOrder  (Node* root, const std::function<bool(const Node*)>& cb) const {
		if (root == nullptr) return false;

		if (InOrder(root->left , cb)) return true;
		if (cb(root)) return true;
		if (InOrder(root->right, cb)) return true;
		return false;
	}
	bool PreOrder (Node* root, const std::function<bool(Node*)>& cb) {
		if (root == nullptr) return false;

		if (cb(root)) return true;
		if (PreOrder(root->left , cb)) return true;
		if (PreOrder(root->right, cb)) return true;
		return false;
	}
	bool PreOrder (Node* root, const std::function<bool(const Node*)>& cb) const {
		if (root == nullptr) return false;

		if (cb(root)) return true;
		if (PreOrder(root->left , cb)) return true;
		if (PreOrder(root->right, cb)) return true;
		return false;
	}
	bool PostOrder(Node* root, const std::function<bool(Node*)>& cb) {
		if (root == nullptr) return false;

		if (PostOrder(root->left , cb)) return true;
		if (PostOrder(root->right, cb)) return true;
		if (cb(root)) return true;
		return false;
	}
	bool PostOrder(Node* root, const std::function<bool(const Node*)>& cb) const {
		if (root == nullptr) return false;

		if (PostOrder(root->left , cb)) return true;
		if (PostOrder(root->right, cb)) return true;
		if (cb(root)) return true;
		return false;
	}

	Node* Search(Node* node, const T& key) {
		if (node == nullptr)  return nullptr;
		if (node->val == key) return node;
		if (key < node->val)  return search(node->left, key);
		else                  return search(node->right, key);
	}

	Node* Delete(Node* node, const T& key) {
		if (node == nullptr) return nullptr;

		if (key < node->val)
			node->left = Delete(node->left, key);
		else if (key > node->val)
			node->right = Delete(node->right, key);
		else {
			// Node with one child or no child
			if (node->left == nullptr) {
				Node* temp = node->right;
				delete node;
				size--;
				return temp;
			} else if (node->right == nullptr) {
				Node* temp = node->left;
				delete node;
				size--;
				return temp;
			}

			// Node with two children: get the smallest of the values greater than the current node (min node in right subtree)
			Node* temp = MinNode(node->right);
			node->val = temp->val;
			node->right = Delete(node->right, temp->val);
		}
		return node;
	}

	Node* MinNode(Node* node) {
		if (node == nullptr) return nullptr;

		Node* current = node;
		while (current->left != nullptr)
			current = current->left;
		return current;
	}

	void Destroy(Node* root) {
		if (root == nullptr) return;

		Destroy(root->left);
		Destroy(root->right);
		delete root;
	}

private:
	size_t size = 0;
	Node* m_Root = nullptr;
};

namespace std {
	template <typename T>
	constexpr inline void swap(BST<T>& _Left, BST<T>& _Right) noexcept {
		_Left.swap(_Right);
	}
}

#ifdef LEAK_CHECK
template <typename T>
int BST<T>::Node::counter = 0;
#endif
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
	std::format_to(std::ostream_iterator<char>(std::cout), "[{}:{}]", c.x, c.y);
	return out;
#else
	return out << '[' << c.x << ':' << c.y << ']';
#endif
}
template <typename T>
std::ostream& operator<<(std::ostream& out, const coord<T, 3>& c) {
#ifdef HAS_CPP20
	std::format_to(std::ostream_iterator<char>(std::cout), "[{}:{}:{}]", c.x, c.y, c.z);
	return out;
#else
	return out << '[' << c.x << ':' << c.y << ':' << c.z << ']';
#endif
}
template <typename T>
std::ostream& operator<<(std::ostream& out, const coord<T, 4>& c) {
#ifdef HAS_CPP20
	std::format_to(std::ostream_iterator<char>(std::cout), "[{}:{}:{}:{}]", c.x, c.y, c.z, c.w);
	return out;
#else
	return out << '[' << c.x << ':' << c.y << ':' << c.z << ':' << c.w << ']';
#endif
}

enum class TreePrintMode { IN, PRE, POST };
extern const i32 g_TreePrintModeIndex; // extern to be common across translation units

std::ostream& TreeInOrder(std::ostream& out);
std::ostream& TreePreOrder(std::ostream& out);
std::ostream& TreePostOrder(std::ostream& out);

template <typename T>
std::ostream& operator<<(std::ostream& out, const BST<T>& tree) {
	auto lambda = [&out](const typename BST<T>::Node* node) -> bool { out << node->val << ' '; return false; };
	TreePrintMode mode = (TreePrintMode)out.iword(g_TreePrintModeIndex);
	if (mode == TreePrintMode::IN)
		tree.InOrder(lambda);
	if (mode == TreePrintMode::PRE)
		tree.PreOrder(lambda);
	if (mode == TreePrintMode::POST)
		tree.PostOrder(lambda);
	return out;
}
// ################################################################## LOGGING ##################################################################