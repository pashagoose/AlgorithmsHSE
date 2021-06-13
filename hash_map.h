#include<functional>
#include<list>
#include<stdexcept>
#include<utility>
#include<vector>

using std::initializer_list;
using std::list;
using std::pair;
using std::vector;
  
template<class KeyType, class ValueType, class Hash = std::hash<KeyType>> 
class HashMap {
public:

	class iterator;
	class const_iterator;

	HashMap(const Hash& hash_func = Hash()) : 
					hasher(hash_func) 
					{}
   
	size_t size() const noexcept {
		return sz;
	}

	bool empty() const noexcept {
		return (sz == 0);
	}

	iterator begin() {
		return iterator(content.begin());
	}

	iterator end() {
		return iterator(content.end());
	}

	const_iterator begin() const {
		return const_iterator(content.cbegin());
	}

	const_iterator end() const {
		return const_iterator(content.cend());
	}

	Hash hash_function() const {
		return hasher;
	}

	iterator find(const KeyType& key) {
		if (empty()) return end();
		size_t table_index = hasher(key) % table.size();
		for (auto it : table[table_index]) {
			if (it->first == key) {
				return iterator(it);
			}
		}
		return end();
	}

	const_iterator find(const KeyType& key) const {
		if (empty()) return end();
		size_t table_index = hasher(key) % table.size();
		for (const auto& it : table[table_index]) {
			if (it->first == key) {
				return const_iterator(it);
			}
		}
		return end();
	}
 
	pair<bool, iterator> insert(const pair<const KeyType, ValueType>& elem) {
		auto it = find(elem.first);
		if (it != end()) {
			return {false, it};
		}
		++sz;
		content.push_back(elem);
		if (sz >= table.size()) {
			Rescale();
		} else {
			table[hasher(elem.first) % table.size()].push_back(--content.end());
		}
		return {true, iterator(--content.end())};
	}

	template<class InputIt>
	void InsertRange(InputIt first, InputIt last) {
		while (first != last) {
			insert(*first);
			++first;
		}
	}

	HashMap& operator=(const HashMap& other) {
		list<pair<const KeyType, ValueType>> copy_other(other.content);
		hasher = other.hasher;
		content.clear();
		table.clear();
		sz = 0;
		InsertRange(copy_other.begin(), copy_other.end());
		return *this;
	}

	void erase(const KeyType& key) {
		if (empty()) return;
		size_t table_index = hasher(key) % table.size();
		auto vect_iterator = table[table_index].begin();
		for (; vect_iterator != table[table_index].end(); ++vect_iterator) {
			if ((*vect_iterator)->first == key) {
				content.erase(*vect_iterator);
				table[table_index].erase(vect_iterator);
				--sz;
				break;
			}
		}
		if (sz * SIZE_MULTIPLIER * SIZE_MULTIPLIER < table.size()) {
			Rescale();
		}
	}

	template<class InputIt>
	HashMap(InputIt first, InputIt last, const Hash& hash_func = Hash()) : 
					hasher(hash_func) 
					{
		while (first != last) {
			insert(*first);
			++first;
		}
	}

	HashMap(initializer_list<pair<KeyType, ValueType>> build_from, 
										const Hash& hash_func = Hash()) : 
							HashMap(build_from.begin(), build_from.end(), hash_func) 
							{}

	ValueType& operator[](const KeyType& key) {
		auto res = insert({key, ValueType()});
		return res.second->second;
	}

	const ValueType& at(const KeyType& key) const {
		auto it = find(key);
		if (it == end()) throw std::out_of_range("Key doesn't exist");
		else return it->second;
	}

	void clear() {
		while (!content.empty()) {
			erase(content.begin()->first);
		}
	}

	class iterator {
	private:
		typename list<pair<const KeyType, ValueType>>::iterator it;

	public:
		iterator() = default;

		iterator(const iterator& other_it) : 
						it(other_it.it) 
						{}

		iterator(typename list<pair<const KeyType, ValueType>>::iterator other_it) : 
						it(other_it) 
						{}

		iterator& operator=(const iterator& other) {
			it = other.it;
			return *this;
		}

		iterator& operator++() {
			++it;
			return *this;
		}

		iterator operator++(int) {
			auto new_it = *this;
			++it;
			return new_it;
		}

		pair<const KeyType, ValueType>& operator*() const {
			return *it;
		}

		pair<const KeyType, ValueType>* operator->() const {
			return it.operator->();
		}

		bool operator==(const iterator& other) const {
			return it == other.it;
		}

		bool operator!=(const iterator& other) const {
			return it != other.it;
		}
	};

	class const_iterator {
	private:
		typename list<pair<const KeyType, ValueType>>::const_iterator it;

	public:
		const_iterator() = default;

		const_iterator(const const_iterator& other) : 
							it(other.it) 
							{}

		const_iterator(typename list<pair<const KeyType, ValueType>>::const_iterator other_it) : 
							it(other_it) 
							{}

		const_iterator& operator=(const const_iterator& other) {
			it = other.it;
			return *this;
		}

		const_iterator& operator++() {
			++it;
			return *this;
		}

		const_iterator operator++(int) {
			auto new_it = *this;
			++it;
			return new_it;
		}

		const pair<const KeyType, ValueType>& operator*() const {
			return *it;
		}

		const pair<const KeyType, ValueType>* operator->() const {
			return it.operator->();
		}

		bool operator==(const const_iterator& other) const {
			return it == other.it;
		}

		bool operator!=(const const_iterator& other) const {
			return it != other.it;
		}
	};

private:
	constexpr static inline size_t SIZE_MULTIPLIER = 2;
	Hash hasher;
	list<pair<const KeyType, ValueType>> content;
	vector<vector<typename list<pair<const KeyType, ValueType>>::iterator>> table;
	size_t sz = 0;

	void Rescale() {
		table.assign(SIZE_MULTIPLIER * sz, 
			vector<typename list<pair<const KeyType, ValueType>>::iterator>());
		for (auto it = content.begin(); it != content.end(); ++it) {
			table[hasher(it->first) % table.size()].push_back(it);
		}
	}
};  
