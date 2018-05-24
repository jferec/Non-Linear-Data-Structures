#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <queue>

namespace aisdi
{




template <typename KeyType, typename ValueType>
class TreeMap
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

    class TreeNode
    {
    public:

        TreeNode* leftchild;
        TreeNode* rightchild;
        TreeNode* parent;
        value_type datapair;

        TreeNode(key_type key, mapped_type value):
            leftchild(nullptr), rightchild(nullptr), parent(nullptr), datapair(std::make_pair(key, value)){}
        TreeNode():
             leftchild(nullptr), rightchild(nullptr), parent(nullptr){}
        TreeNode(value_type it) : TreeNode(it.first,it.second) {}

        ~TreeNode()
        {leftchild = nullptr;
        }

    };
    TreeNode *root;
    size_type node_counter;

public:


  ~TreeMap()
  {
      removeTree();
  }


  TreeMap():root(nullptr), node_counter(0){};



  TreeMap(std::initializer_list<value_type> list):TreeMap()
  {
    for(auto it = list.begin(); it != list.end(); it++)
        insert(new TreeNode(*it));
  }

  TreeMap(const TreeMap& other):TreeMap()
  {
     for(auto it = other.begin(); it!= other.end(); it++)
     {
         insert(new TreeNode(*it));
     }


  }

  TreeMap(TreeMap&& other)
  {
        root=other.root;
        node_counter=other.node_counter;
        other.root = nullptr;
        other.node_counter=0;

  }

  TreeMap& operator=(const TreeMap& other)
  {
        if(this != &other)
            {
              removeTree();
              for (auto it = other.begin(); it != other.end(); ++it)
                insert( new TreeNode(*it) );
            }
    return *this;
  }

  TreeMap& operator=(TreeMap&& other)
  {
        if(this != &other)
            {
              removeTree();

              root = other.root;
              node_counter = other.node_counter;

              other.root = nullptr;
              other.node_counter = 0;
            }
        return *this;
  }

  bool isEmpty() const
  {
        return (node_counter == 0);
  }

  mapped_type& operator[](const key_type& key)
  {


        TreeNode* current = findNode(key);
        if( current == nullptr)
        {
            current = new TreeNode(key,mapped_type());
            insert( current);
        }
        return current->datapair.second;
  }


  const mapped_type& valueOf(const key_type& key) const
  {
        ConstIterator temp = find(key);
        return (*temp).second;
  }

  mapped_type& valueOf(const key_type& key)
  {
        Iterator temp = find(key);
        return (*temp).second;
  }

  const_iterator find(const key_type& key) const
  {
        return const_iterator(this,findNode(key));
  }

  iterator find(const key_type& key)
  {
        return iterator(this,findNode(key));
  }

  void remove(const key_type& key)
  {
        remove(find(key));
  }

  void remove(const const_iterator& it)
  {
    if(it == end())
        throw std::out_of_range("error: out of range");

    if(it.curr_node->leftchild == nullptr)
        transplant(it.curr_node,it.curr_node->rightchild);

    else if(it.curr_node->rightchild == nullptr)
        transplant(it.curr_node,it.curr_node->leftchild);

    else
        {
            TreeNode* temp = findMin(it.curr_node->rightchild);

            if (temp->parent != it.curr_node)
            {
                transplant(temp, temp->rightchild);
                temp->rightchild = it.curr_node->rightchild;
                temp->rightchild->parent = temp;

            }
            transplant(it.curr_node, temp);
            temp->leftchild = it.curr_node->leftchild;
            temp->leftchild->parent = temp;
        }
        delete it.curr_node;
        node_counter--;
  }

  size_type getSize() const
  {
    return node_counter;
  }



  bool operator==(const TreeMap& other) const
  {
            if (other.getSize() != getSize())
                return false;

            for (auto iter1 = begin(), iter2=other.begin(); iter1 != end(); iter1++,iter2++)
            {
                if (*iter1 != *iter2)
                    return false;//różne elementy
            }
            return true;
  }

  bool operator!=(const TreeMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
        return Iterator(this, findMin(root));
  }

  iterator end()
  {
        return Iterator(this, nullptr);
  }

  const_iterator cbegin() const
  {
        return ConstIterator(this, findMin(root));
  }

  const_iterator cend() const
  {
        return ConstIterator(this, nullptr);//do poprawy
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }



void transplant(TreeNode* outNode, TreeNode* inNode)
{
    if(outNode->rightchild == nullptr)
        root = inNode;
    else if(outNode->parent->leftchild == outNode)
        outNode->parent->leftchild = inNode;
    else
        outNode->parent->rightchild = inNode;

    if(inNode != nullptr)
        inNode->rightchild = outNode->rightchild;

}


TreeNode* findNode(const key_type& key) const
{

    TreeNode* temp = root;
        while(temp != nullptr && temp->datapair.first != key)
        {
            if(key > temp->datapair.first)
                temp = temp -> rightchild;

            else
                temp = temp -> leftchild;
        }

    return temp;

}

TreeNode* findMax(TreeNode* temp) const
    {
        if(root == nullptr)
            return nullptr;

        while(temp->rightchild != nullptr)
            temp = temp->rightchild;
        return temp;
    }


TreeNode* findMin(TreeNode* temp) const
    {
        if(node_counter==0)
            return nullptr;

        while(temp->leftchild != nullptr)
            temp = temp->leftchild;
        return temp;
    }


void insert(TreeNode* newNode)
    {
        node_counter++;
        TreeNode* curr_parent = nullptr;
        TreeNode* curr = root;
        if (root == nullptr)
            {
                root = newNode;
                return;
            }

        curr = root;

        while (curr != nullptr)
            {
                curr_parent = curr;
                if(curr->datapair.first < newNode->datapair.first)
                    curr = curr->rightchild;
                else
                    curr = curr->leftchild;
            }
        if(curr_parent->datapair.first < newNode->datapair.first)
            curr_parent->rightchild = newNode;
        else
            curr_parent->leftchild = newNode;
        newNode->parent = curr_parent;

    }

void removeAllNodes(TreeNode * temp)
{
    if(temp == nullptr)
        return;
    removeAllNodes(temp->leftchild);
    removeAllNodes(temp->rightchild);
    delete temp;
}

void removeTree()
{
    if(root==nullptr)
        return;
    removeAllNodes(root->leftchild);
    removeAllNodes(root->rightchild);
    node_counter=0;
    delete root;
    root = nullptr;
}


};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::ConstIterator
{


public:

  using reference = typename TreeMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename TreeMap::value_type;
  using pointer = const typename TreeMap::value_type*;

private:
  const TreeMap *tree;
    TreeNode *curr_node;
  friend void TreeMap<KeyType, ValueType>::remove(const const_iterator&);


public:

explicit ConstIterator(const TreeMap* tree, TreeNode* node): tree(tree), curr_node(node){}

    ConstIterator(const ConstIterator& other) : ConstIterator(other.tree, other.curr_node){}

  ConstIterator& operator++()
  {
    if(tree==nullptr||curr_node ==nullptr)
        throw std::out_of_range("operator++ out of range");
    if(curr_node->rightchild)
    {
        curr_node=curr_node->rightchild;
        while(curr_node->leftchild!=nullptr)
            curr_node=curr_node->leftchild;
    }
    else
    {
        while(curr_node->parent != nullptr && curr_node->parent->rightchild == curr_node)
            curr_node=curr_node->parent;
        curr_node=curr_node->parent;
    }
    return *this;
  }

  ConstIterator operator++(int)
  {
    ConstIterator result = *this;
    operator++();
    return result;
  }

  ConstIterator& operator--()
  {

    if(tree == nullptr || tree->root == nullptr)
    throw std::out_of_range("operator-- outofrange");

    else if(curr_node == nullptr)
    {
        curr_node = tree->root;
            while(curr_node->rightchild != nullptr)
                curr_node=curr_node->rightchild;
    }


    else if(curr_node->leftchild)
    {
        curr_node=curr_node->leftchild;
        while(curr_node->rightchild!=nullptr)
            curr_node=curr_node->rightchild;
    }
    else
    {
        while(curr_node->parent != nullptr && curr_node->parent->leftchild == curr_node)
            curr_node=curr_node->parent;
        curr_node=curr_node->parent;
    }

    return *this;
  }

  ConstIterator operator--(int)
  {
    ConstIterator result = *this;
    operator--();
    return result;
  }

  reference operator*() const
  {
        if(tree == nullptr ||curr_node == nullptr)
                throw std::out_of_range("operator*() out_of_range");

        return curr_node->datapair;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return tree == other.tree && curr_node == other.curr_node;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }



};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::reference;
  using pointer = typename TreeMap::value_type*;




explicit Iterator(TreeMap *tree, TreeNode *node) : ConstIterator(tree, node){}

Iterator(const ConstIterator& other)
    : ConstIterator(other)  {}


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

#endif /* AISDI_MAPS_MAP_H */
