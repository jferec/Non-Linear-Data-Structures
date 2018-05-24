#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
{

    public:
      using key_type = KeyType;
      using mapped_type = ValueType;
      using value_type = std::pair<const key_type, mapped_type>;
      using size_type = std::size_t;
      using reference = value_type&;
      using const_reference = const value_type&;

      class ConstIterator;
      class Iterator;
      using iterator = Iterator;
      using const_iterator = ConstIterator;
private:
    class HashNode
    {
         public:
        HashNode *next;
        HashNode *prev;
        value_type datapair;

        HashNode(key_type key, mapped_type mapped):
            next(nullptr), prev(nullptr), datapair(std::make_pair(key,mapped)){}

        HashNode(key_type key, mapped_type mapped, HashNode* prev):
            next(nullptr), prev(nullptr), datapair(std::make_pair(key,mapped))
            {
                this->prev = prev;
            }

        ~HashNode()
            {}
    };
    const size_type TABLE_SIZE = 1000;
    HashNode** hashtable;
    size_type counter;

public:

  HashMap(): hashtable(nullptr), counter(0)
  {
        hashtable = new HashNode* [TABLE_SIZE];
            for (size_type i = 0; i < TABLE_SIZE; i++)
                hashtable[i] = nullptr;//inicjalizacja nullem
  }

  HashMap(std::initializer_list<value_type> list):HashMap()
  {
        for(auto it = list.begin(); it!= list.end(); it++)
            operator[]((*it).first) = (*it).second;
  }

  HashMap(const HashMap& other):HashMap()
  {
        for(auto it = other.begin(); it!= other.end(); it++)
            operator[]((*it).first) = (*it).second;
  }

  HashMap(HashMap&& other):HashMap()//do edycji
  {
        for(auto it = other.begin(); it!= other.end(); it++)
            operator[]((*it).first) = (*it).second;

        other.eraseHashMap();
    }

  HashMap& operator=(const HashMap& other)
  {
        if(this!= &other)
            {
                eraseHashMap();

                for(auto it = other.begin(); it!= other.end(); it++)
                    operator[]((*it).first) = (*it).second;
            }
        return *this;
  }

  HashMap& operator=(HashMap&& other)
  {
            if(this!= &other)
            {
                eraseHashMap();

                for(auto it = other.begin(); it!= other.end(); it++)
                    operator[]((*it).first)= (*it).second;

                other.eraseHashMap();
            }
        return *this;
  }

  ~HashMap()
  {
      eraseHashMap();
      delete[] hashtable;
  }

  bool isEmpty() const
  {
        return counter == 0;
  }

  mapped_type& operator[](const key_type& key)
  {
    HashNode* temp1 = findNode(key);

        if(temp1 == nullptr)
        {
            counter++;
            temp1 = new HashNode(key, mapped_type());

            if(hashtable[modHash(key)] != nullptr)
                {
                    HashNode* temp2 = hashtable[modHash(key)];
                    while(temp2->next != nullptr)
                        temp2 = temp2->next;
                    temp2->next = temp1;
                    temp1->prev = temp2;
                }

            else
                {
                   hashtable[modHash(key)] = temp1;
                }
        }
    return temp1->datapair.second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
        HashNode* temp = findNode(key);
        if(temp == nullptr)
            throw std::out_of_range("valueOf out of range error");
        return temp->datapair.second;
  }

  mapped_type& valueOf(const key_type& key)
  {
        HashNode* temp = findNode(key);
        if(temp == nullptr)
            throw std::out_of_range("valueOf out of range error");
        return temp->datapair.second;
  }

  const_iterator find(const key_type& key) const
  {
        return const_iterator(this, findNode(key), modHash(key));
  }

  iterator find(const key_type& key)
  {
        return iterator(this, findNode(key), modHash(key));
  }

  void remove(const key_type& key)
  {
        remove(find(key));
  }

  void remove(const const_iterator& it)
  {
    if(it == end())
        throw std::out_of_range("remove out of range");
    if(it.curr_node->prev == nullptr)
        hashtable[modHash(it.curr_node->datapair.first)] = it.curr_node->next;
    else
        it.curr_node->prev->next = it.curr_node->prev;

    if(it.curr_node->next != nullptr)
        it.curr_node->next->prev = it.curr_node->next;

    it.curr_node->next = nullptr;
    delete it.curr_node;
    counter--;
  }

  size_type getSize() const
  {
    return counter;
  }

  bool operator==(const HashMap& other) const
  {
    if(other.counter != counter)
        return false;

    auto it2 = other.begin();
    for(auto it1 = begin(); it1!=end(); ++it1, ++it2)
    {
       if(*it1 != *it2)
        return false;
    }
    return true;
  }

  bool operator!=(const HashMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    if(firstNode() == TABLE_SIZE)
        return end();
    return iterator (this, hashtable[firstNode()], firstNode());
  }

  iterator end()
  {
    return iterator(this, nullptr, TABLE_SIZE);
  }

  const_iterator cbegin() const
  {
    if(firstNode() == TABLE_SIZE)
        return cend();
    return const_iterator (this, hashtable[firstNode()], firstNode());
  }

  const_iterator cend() const
  {
    return const_iterator (this, nullptr, TABLE_SIZE);
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }


  HashNode* findNode(const key_type& key) const
  {
      HashNode* temp = hashtable [ modHash(key) ];

      while(temp != nullptr)
          {
              if(temp->datapair.first == key)
                {
                    return temp;
                }
            temp = temp->next;
          }
        return temp;
  }

  void eraseHashMap()
  {
      if(counter != 0)
          {
              for(size_type i = 0; i<TABLE_SIZE; i++)
                  {
                      delete hashtable[i];
                      hashtable[i] = nullptr;
                  }
          }
      counter = 0;
  }



  size_type firstNode() const
  {
      size_type index = 0;

        while(hashtable[index] == nullptr && index != TABLE_SIZE)
            index++;

        return index;

  }

  size_type modHash(const key_type& key) const
  {
      return std::hash<key_type>()(key) % TABLE_SIZE;
  }

  void insert(key_type key, mapped_type mapped)
      {
          operator[](key) = mapped;
      }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type*;


    const HashMap* hashmap;
    HashNode* curr_node;
    size_type index;
    friend void HashMap<KeyType, ValueType>::remove(const const_iterator&);


    explicit ConstIterator(){};

  ConstIterator(const HashMap *hashmap = nullptr, HashNode* node = nullptr, size_type index = 0): hashmap(hashmap), curr_node(node), index(index)
  {
    if(curr_node==nullptr && hashmap != nullptr)
        this->index = hashmap->TABLE_SIZE;
  }

  ConstIterator(const ConstIterator& other)
      :ConstIterator(other.hashmap, other.curr_node, other.index)
        {}

  ConstIterator& operator++()
  {
    if(hashmap == nullptr || curr_node == nullptr)
        throw std::out_of_range("operator++ out of range");

    if(curr_node->next != nullptr)
        curr_node = curr_node->next;

    else
    {
        index++;

        while(hashmap->hashtable[index] == nullptr  && index != hashmap->TABLE_SIZE)
            index++;

        if(index != hashmap->TABLE_SIZE)
            curr_node = hashmap->hashtable[index];

        else
            curr_node = nullptr;
    }
        return *this;
  }

  ConstIterator operator++(int)
  {
    auto result = *this;
    operator++();
    return result;
  }

  ConstIterator& operator--()
  {
    if(hashmap == nullptr)
        throw std::out_of_range("operator-- out of range");
    else if(curr_node == nullptr || curr_node == hashmap->hashtable[index])
        {
            index--;
            while(hashmap->hashtable[index] == nullptr && index > 0 )
                index--;

            if(index == 0 && hashmap->hashtable[index] == nullptr)
                throw std::out_of_range("operator-- out of range");

            curr_node = hashmap->hashtable[index];

            while (curr_node->next != nullptr)
                curr_node = curr_node->next;
        }
    else
        {
            curr_node = curr_node->prev;
        }
        return *this;
  }

  ConstIterator operator--(int)
  {
    auto result = *this;
    operator--();
    return result;
  }

  reference operator*() const
  {
    if(curr_node == nullptr)
        throw std::out_of_range("operator* out of range");

    return curr_node->datapair;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return (hashmap == other.hashmap && curr_node == other.curr_node && index == other.index);
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }


};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;
    explicit Iterator()
    {

    }

  Iterator(HashMap *hashmap , HashNode *node, size_type index) : ConstIterator(hashmap, node, index)
  {}


  Iterator(const ConstIterator& other)
    : ConstIterator(other)
  {}

  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif /* AISDI_MAPS_HASHMAP_H */
