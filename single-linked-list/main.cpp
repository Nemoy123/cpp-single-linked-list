#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>


using namespace std;

template <typename Type>
class SingleLinkedList {
        
        // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value{};
        Node* next_node = nullptr;
        //~Node() {}
    };
    
    
        // Шаблон класса «Базовый Итератор».
    // Определяет поведение итератора на элементы односвязного списка
    // ValueType — совпадает с Type (для Iterator) либо с const Type (для ConstIterator)
    template <typename ValueType>
    class BasicIterator {
        // Класс списка объявляется дружественным, чтобы из методов списка
        // был доступ к приватной области итератора
        friend class SingleLinkedList;

        // Конвертирующий конструктор итератора из указателя на узел списка
     
         explicit BasicIterator(Node* node) : node_(node) {} 

    public:
        // Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора

        // Категория итератора — forward iterator
        // (итератор, который поддерживает операции инкремента и многократное разыменование)
        using iterator_category = std::forward_iterator_tag;
        // Тип элементов, по которым перемещается итератор
        using value_type = Type;
        // Тип, используемый для хранения смещения между итераторами
        using difference_type = std::ptrdiff_t;
        // Тип указателя на итерируемое значение
        using pointer = ValueType*;
        // Тип ссылки на итерируемое значение
        using reference = ValueType&;

        BasicIterator() = default;

        // Конвертирующий конструктор/конструктор копирования
        // При ValueType, совпадающем с Type, играет роль копирующего конструктора
        // При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
        BasicIterator(const BasicIterator<Type>& other) noexcept {
            node_= other.node_;
        }

        // Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
        // пользовательского конструктора копирования, явно объявим оператор = и
        // попросим компилятор сгенерировать его за нас
        BasicIterator& operator=(const BasicIterator& rhs) = default;

        // Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return this->node_ == rhs.node_;
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен !=
        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            
            return this->node_ != rhs.node_;
        }

        // Оператор сравнения итераторов (в роли второго аргумента итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return this->node_ == rhs.node_;
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен !=
        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
           return this->node_ != rhs.node_;
        }

        // Оператор прединкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает ссылку на самого себя
        // Инкремент итератора, не указывающего на существующий элемент списка, приводит к неопределённому поведению
        BasicIterator& operator++() noexcept {
            node_ = node_->next_node;
            return *this;   
         }
         
         

        // Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает прежнее значение итератора
        // Инкремент итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        BasicIterator operator++(int) noexcept {
               BasicIterator old_node_ =  *this;
               node_ = node_->next_node;
               return old_node_;
        }

        // Операция разыменования. Возвращает ссылку на текущий элемент
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] reference operator*() const noexcept {
            return node_->value;
        }

        // Операция доступа к члену класса. Возвращает указатель на текущий элемент списка
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] pointer operator->() const noexcept {
            return &node_->value;
        }

    private:
        Node* node_ = nullptr;
    };    




    

public:
    
    SingleLinkedList () : size_(0){};

    ~SingleLinkedList (){
        Clear();
    }
    
    SingleLinkedList(std::initializer_list<Type> values) { //Конструктор с листом значений начальных
        SingleLinkedList temporary = ListCopyFunc (values.begin(), values.end());
        (*this).swap (temporary);
    }

    SingleLinkedList(const SingleLinkedList& other) {   // Конструктор копирования
        SingleLinkedList temporary = ListCopyFunc (other.begin(), other.end());
        (*this).swap (temporary);
    }

    
     
        
    
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
    using ConstIterator = BasicIterator<const Type>;
    
    // Возвращает итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
   
    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{head_.next_node};
    }
    
    // Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    
    [[nodiscard]] Iterator end() noexcept {
        return Iterator{nullptr};
    }
    // Константные версии begin/end для обхода списка без возможности модификации его элементов
   
    [[nodiscard]] ConstIterator begin() const noexcept {
        
        return  ConstIterator (head_.next_node);
    }
    
    [[nodiscard]] ConstIterator end() const noexcept {
        
        auto* nod = head_.next_node;
        while (nod != nullptr){
            nod = nod->next_node;
        }
        return ConstIterator {nod};
       
        }
    // Методы для удобного получения константных итераторов у неконстантного контейнера
   
    [[nodiscard]] ConstIterator cbegin() const noexcept {
         
        return ConstIterator (head_.next_node);
    }
    
    
    [[nodiscard]] ConstIterator cend() const noexcept {
        auto* nod = head_.next_node;
        while (nod != nullptr){
            nod = nod->next_node;
        }
        return ConstIterator {nod};
        
    }

     // Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator before_begin() noexcept {
        //Node* nod_before = new Node (0, &head_);
        //return Iterator{nod_before->next_node};
        return Iterator{ &head_ };
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
         return ConstIterator{ const_cast<Node*>(&head_) };
    }
    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        auto nod_before = new Node (0, &head_);
        return ConstIterator{nod_before->next_node};
    }

    // * Вставляет элемент value после элемента, на который указывает pos.
    //  * Возвращает итератор на вставленный элемент
    //  * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        
            Node* new_node = new Node (value, pos.node_->next_node); 
            pos.node_->next_node = new_node;
            ++size_;
        return Iterator{new_node};
    }

    Iterator ErasePosition (ConstIterator pos) noexcept {  // удаляет указанный элемент
        //Найти предшествующий узел
        auto before_position = before_begin();
        for (auto before_pos = before_begin(); before_pos != end(); ++before_pos) {
            if ((before_pos.node_->next_node) == pos.node_) {
                before_position = before_pos;
                break;
            }
        }

        //запоминаем указатель узла после удаляемого
        Node* pos_after = pos.node_->next_node;
        //удаляем
        Node* del = pos.node_;
        delete del;
        --size_;
        // меняем указатель предшествующего
        before_position.node_->next_node = pos_after;

        return Iterator{pos_after};
    }

    Iterator EraseAfter (ConstIterator pos) noexcept { // удаляет элемент след после указаного
        // запомнить адрес след элемента
            Node* node_del = pos.node_->next_node;
        // запомнить адрес после след
             Node* node_after_del  = node_del->next_node;
        //удаляем
        delete node_del;
        --size_;    
        // привязываем указатель
        pos.node_->next_node = node_after_del;
        return Iterator {node_after_del};
    }



    
    [[nodiscard]] size_t GetSize() const noexcept; // Возвращает количество элементов в списке за время O(1)
    [[nodiscard]] bool IsEmpty() const noexcept; // Сообщает, пустой ли список за время O(1)
    void swap(SingleLinkedList& other) noexcept;
    void operator=(const SingleLinkedList<Type>& other);
    void PushFront(const Type& value);
    void Clear() noexcept;
    void PopFront() noexcept;
    
    
    
    template <typename Iter>
    SingleLinkedList<Type> ListCopyFunc (Iter begin, Iter end) const {
        SingleLinkedList tmp;
      
       vector <Iter> rever;
       for (auto& it = begin; it !=end; ++it){ 
        rever.push_back (it);
       }
       std::reverse(rever.begin(), rever.end());
        for (auto& item : rever) {
            tmp.PushFront(*item);
        }
        
        return tmp;
    }
    
    
        
private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_;
    size_t size_;
};



template <typename Type>
void SingleLinkedList<Type>::PopFront() noexcept {
    Node* first = head_.next_node;
    Node* second = first->next_node;
    head_.next_node = second;
    delete first;
    --size_;
    
}


template <typename Type>
void SingleLinkedList<Type>::PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
}

template <typename Type>
[[nodiscard]] size_t SingleLinkedList<Type>::GetSize() const noexcept {
        return size_;
}


template <typename Type>
[[nodiscard]] bool SingleLinkedList<Type>::IsEmpty() const noexcept {
       
        return (size_ == 0);
}

template <typename Type>
void SingleLinkedList<Type>::operator=(const SingleLinkedList<Type>& other) {
        if (&(*this) != &other ){
           
            if (other.IsEmpty()) {(*this).Clear();}
            
             SingleLinkedList tmp (other);
              (*this).swap (tmp);
        
        } 
}


template <typename Type>
void SingleLinkedList<Type>::swap(SingleLinkedList& other) noexcept {
        std::swap( (*this).size_, other.size_ );
        std::swap( head_.next_node, other.head_.next_node);
        
}

template <typename Type>
void SingleLinkedList<Type>::Clear() noexcept {
        for (;;) {
            auto* delteted_iter = head_.next_node; // запоминаем  указатель удаляемого элелемента
            if (delteted_iter == nullptr) {break;}
            auto* new_iterat = (*delteted_iter).next_node; // запоминаем  указатель на 
                                                       // след после удаляемого элемент
            head_.next_node = new_iterat; // меняем привязку на новый элемент
            
            delete delteted_iter; // удаляем ноду
            delteted_iter = nullptr;
            new_iterat  = nullptr;
           --size_;
        }   
}  


template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}
    
    
template <typename Type>
bool operator==(const SingleLinkedList<Type>& left, const SingleLinkedList<Type>& right) {
        if (&left == &right) {return true;}
        if (left.GetSize() != right.GetSize()) {return false;}
        return std::equal (left.begin(), left.end(), right.begin());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& left, const SingleLinkedList<Type>& right) {
        if (left.GetSize() == right.GetSize()) {return false;}
        return !equal (left.begin(), left.end(), right.begin());
}

template <typename Type>
bool operator< (const SingleLinkedList<Type>& left, const SingleLinkedList<Type>& right) {
        return lexicographical_compare (left.begin(), left.end(), right.begin(), right.end());
}

template <typename Type>
bool operator<= (const SingleLinkedList<Type>& left, const SingleLinkedList<Type>& right) {
        return (left < right) || (left == right);
}

template <typename Type>
bool operator> (const SingleLinkedList<Type>& left, const SingleLinkedList<Type>& right) {
        return ( right < left);
}
 
template <typename Type>
bool operator>= (const SingleLinkedList<Type>& left, const SingleLinkedList<Type>& right) {
        return ( right < left) || (left == right);
}

void Test4() {
    struct DeletionSpy {
        ~DeletionSpy() {
            if (deletion_counter_ptr) {
                ++(*deletion_counter_ptr);
            }
        }
        int* deletion_counter_ptr = nullptr;
    };

    // Проверка PopFront
    {
        SingleLinkedList<int> numbers{3, 14, 15, 92, 6};
        numbers.PopFront();
        assert((numbers == SingleLinkedList<int>{14, 15, 92, 6}));

        SingleLinkedList<DeletionSpy> list;
        list.PushFront(DeletionSpy{});
        int deletion_counter = 0;
        list.begin()->deletion_counter_ptr = &deletion_counter;
        assert(deletion_counter == 0);
        list.PopFront();
        assert(deletion_counter == 1);
    }

    // Доступ к позиции, предшествующей begin
    {
        SingleLinkedList<int> empty_list;
        const auto& const_empty_list = empty_list;
        assert(empty_list.before_begin() == empty_list.cbefore_begin());
        assert(++empty_list.before_begin() == empty_list.begin());
        assert(++empty_list.cbefore_begin() == const_empty_list.begin());

        SingleLinkedList<int> numbers{1, 2, 3, 4};
        const auto& const_numbers = numbers;
        assert(numbers.before_begin() == numbers.cbefore_begin());
        assert(++numbers.before_begin() == numbers.begin());
        assert(++numbers.cbefore_begin() == const_numbers.begin());
    }

    // Вставка элемента после указанной позиции
    {  // Вставка в пустой список
        {
            SingleLinkedList<int> lst;
            const auto inserted_item_pos = lst.InsertAfter(lst.before_begin(), 123);
            assert((lst == SingleLinkedList<int>{123}));
            assert(inserted_item_pos == lst.begin());
            assert(*inserted_item_pos == 123);
        }

        // Вставка в непустой список
        {
            SingleLinkedList<int> lst{1, 2, 3};
            auto inserted_item_pos = lst.InsertAfter(lst.before_begin(), 123);

            assert(inserted_item_pos == lst.begin());
            assert(inserted_item_pos != lst.end());
            assert(*inserted_item_pos == 123);
            assert((lst == SingleLinkedList<int>{123, 1, 2, 3}));

            inserted_item_pos = lst.InsertAfter(lst.begin(), 555);
            assert(++SingleLinkedList<int>::Iterator(lst.begin()) == inserted_item_pos);
            assert(*inserted_item_pos == 555);
            assert((lst == SingleLinkedList<int>{123, 555, 1, 2, 3}));
        };
    }

    // Вспомогательный класс, бросающий исключение после создания N-копии
    struct ThrowOnCopy {
        ThrowOnCopy() = default;
        explicit ThrowOnCopy(int& copy_counter) noexcept
            : countdown_ptr(&copy_counter) {
        }
        ThrowOnCopy(const ThrowOnCopy& other)
            : countdown_ptr(other.countdown_ptr)  //
        {
            if (countdown_ptr) {
                if (*countdown_ptr == 0) {
                    throw std::bad_alloc();
                } else {
                    --(*countdown_ptr);
                }
            }
        }
        // Присваивание элементов этого типа не требуется
        ThrowOnCopy& operator=(const ThrowOnCopy& rhs) = delete;
        // Адрес счётчика обратного отсчёта. Если не равен nullptr, то уменьшается при каждом копировании.
        // Как только обнулится, конструктор копирования выбросит исключение
        int* countdown_ptr = nullptr;
    };

    // Проверка обеспечения строгой гарантии безопасности исключений
    {
        bool exception_was_thrown = false;
        for (int max_copy_counter = 10; max_copy_counter >= 0; --max_copy_counter) {
            SingleLinkedList<ThrowOnCopy> list{ThrowOnCopy{}, ThrowOnCopy{}, ThrowOnCopy{}};
            try {
                int copy_counter = max_copy_counter;
                list.InsertAfter(list.cbegin(), ThrowOnCopy(copy_counter));
                assert(list.GetSize() == 4u);
            } catch (const std::bad_alloc&) {
                exception_was_thrown = true;
                assert(list.GetSize() == 3u);
                break;
            }
        }
        assert(exception_was_thrown);
    }

    // Удаление элементов после указанной позиции
    {
        {
            SingleLinkedList<int> lst{1, 2, 3, 4};
            const auto& const_lst = lst;
            const auto item_after_erased = lst.EraseAfter(const_lst.cbefore_begin());
            assert((lst == SingleLinkedList<int>{2, 3, 4}));
            assert(item_after_erased == lst.begin());
        }
        {
            SingleLinkedList<int> lst{1, 2, 3, 4};
            const auto item_after_erased = lst.EraseAfter(lst.cbegin());
            assert((lst == SingleLinkedList<int>{1, 3, 4}));
            assert(item_after_erased == (++lst.begin()));
        }
        {
            SingleLinkedList<int> lst{1, 2, 3, 4};
            const auto item_after_erased = lst.EraseAfter(++(++lst.cbegin()));
            assert((lst == SingleLinkedList<int>{1, 2, 3}));
            assert(item_after_erased == lst.end());
        }
        {
            SingleLinkedList<DeletionSpy> list{DeletionSpy{}, DeletionSpy{}, DeletionSpy{}};
            auto after_begin = ++list.begin();
            int deletion_counter = 0;
            after_begin->deletion_counter_ptr = &deletion_counter;
            assert(deletion_counter == 0u);
            list.EraseAfter(list.cbegin());
            assert(deletion_counter == 1u);
        }
    }
}

int main() {
    Test4();
} 