#include <memory>
#include <algorithm>
#include <iostream>

template <class T>
class Vec {
    public:
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef size_t size_type;
        typedef T value_type;
        typedef T& reference;
        typedef const T& const_reference;

        /* 
            Constructor, Deconstructor, Assignment
        */
        Vec() {
            create();
        }
        explicit Vec(size_type n, const T& t = T()) {
            create(n, t);
        }
        Vec(const Vec& v) {
            create(v.begin(), v.end());
        }
        Vec& operator=(const Vec&);
        ~Vec() {
            uncreate();
        }

        T& operator[](size_type i) {
            return this->data[i];
        }
        const T& operator[](size_type i) const {
            return this->data[i];
        }

        void push_back(const T& t) {
            // if our allocated memory storage is exceeded by constructed objects, we will grow a new & copy current vals
            if (this->avail == this->limit) {
                grow();
            }
            this->unchecked_append(t);
        }

        size_type size() const {
            return this->avail - this->data;
        }

        /*
            Iterator methods
        */
        iterator begin() {
            return this->data;
        }
        const_iterator begin() const {
            return this->data;
        }
        
        iterator end() {
            return this->avail;
        }
        const_iterator end() const {
            return this->avail;
        }

    private:
        iterator data;
        iterator avail;
        iterator limit;

        /*
            Facilities for memory allocation
        */
        std::allocator<T> alloc;

        void create();
        void create(size_type, const T&);
        void create(const_iterator, const_iterator);

        void uncreate();
        void grow();
        void unchecked_append(const T&);
};

template <class T>
void Vec<T>::create() {
    this->data = this->avail = this->limit = 0;
}
template <class T>
void Vec<T>::create(size_type n, const T& val) {
    this->data = this->alloc.allocate(n);
}
template <class T>
void Vec<T>::create(const_iterator i, const_iterator j) {
    this->data = this->alloc.allocate(j - i);
    this->limit = this->avail = unitialized_copy(i, j, this->data);
}

template <class T>
void Vec<T>::uncreate() {
    if (this->data) {
        // destroy each element that was constructed in reverse order
        iterator it = this->avail;
        while (it != this->data) {
            this->alloc.destroy(--it);
        }

        // free up the allocated space
        this->alloc.deallocate(this->data, this->limit - this->data);
    }
    // reset pointers
    this->data = this->limit = this->avail = 0;
}

template <class T>
void Vec<T>::grow() {
    // when growing, allocate twice the space currently in use to have flexibility with adding new elements
    size_type new_size = std::max(2 * (this->limit - this->data), ptrdiff_t(1));

    // allocate new space and copy existing elements to new space
    iterator new_data = this->alloc.allocate(new_size);
    iterator new_avail = std::uninitialized_copy(this->data, this->avail, new_data);

    this->uncreate();

    // reset pointers to newly allocated space
    this->data = new_data;
    this->avail = new_avail;
    this->limit = this->data + new_size;
}

template <class T>
void Vec<T>::unchecked_append(const T& val) {
    this->alloc.construct(this->avail++, val);
}
