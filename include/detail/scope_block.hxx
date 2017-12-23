/*
    rgb2yuv - Code covered by the MIT License
    Author: mutouyun (http://orzz.org)
*/

////////////////////////////////////////////////////////////////
/// Define default memory allocator
////////////////////////////////////////////////////////////////

struct allocator
{
    static void * alloc(GLB_ size_t size)
    {
        return ( (size == 0) ? NULL : GLB_ operator new(size, STD_ nothrow) );
    }

    static void free(void * ptr)
    {
        GLB_ operator delete(ptr, STD_ nothrow);
    }
};

////////////////////////////////////////////////////////////////
/// The limited garbage collection facility for memory block
////////////////////////////////////////////////////////////////

template <typename T, typename AllocT = R2Y_ALLOC_>
class scope_block
{
    template <typename U, typename A>
    friend class scope_block;

    T *         block_;
    GLB_ size_t size_;
    bool        trust_;

public:
    scope_block(void)
        : block_(NULL), size_(0), trust_(false)
    {}

    explicit scope_block(GLB_ size_t count) : scope_block()
    {
        reset(count);
    }

    scope_block(T * block, GLB_ size_t count) : scope_block()
    {
        reset(block, count);
    }

    scope_block(scope_block const &) = delete;

    template <typename U>
    scope_block(scope_block<U, AllocT> && rhs) : scope_block()
    {
        this->swap(rhs);
    }

    template <typename U>
    scope_block & operator=(scope_block<U, AllocT> && rhs)
    {
        this->swap(rhs);
        return (*this);
    }

    ~scope_block(void)
    {
        if (trust_) AllocT::free(block_);
    }

    void reset(GLB_ size_t count)
    {
        this->~scope_block();
        size_  = count * sizeof(T);
        block_ = static_cast<T *>( AllocT::alloc(this->size()) );
        trust_ = true;
    }

    void reset(T * block, GLB_ size_t count)
    {
        this->~scope_block();
        size_  = count * sizeof(T);
        block_ = block;
        trust_ = false;
    }

    void trust(void) { trust_ = true; }

    void swap(scope_block & rhs)
    {
        STD_ swap(this->block_, rhs.block_);
        STD_ swap(this->size_ , rhs.size_ );
        STD_ swap(this->trust_, rhs.trust_);
    }

    template <typename U>
    void swap(scope_block<U, AllocT> & rhs)
    {
        void * tmp_ptr = this->block_;
        this->block_ = reinterpret_cast<T *>(rhs.block_);
        rhs  .block_ = reinterpret_cast<U *>(tmp_ptr);
        STD_ swap(this->size_ , rhs.size_ );
        STD_ swap(this->trust_, rhs.trust_);
    }

    /*
     * You need to handle the datas from this scope_block object
     * by yourself before you calling this function.
     */
    T * dismiss(void)
    {
        T * data_ret = this->data();
        block_ = NULL;
        size_  = 0;
        trust_ = false;
        return data_ret;
    }

    T *         data (void) const { return block_; }
    GLB_ size_t size (void) const { return size_ ; }
    GLB_ size_t count(void) const { return size_ / sizeof(T); }
    bool   is_trusted(void) const { return trust_; }

    T       & operator[](GLB_ size_t pos)       { return block_[pos]; }
    T const & operator[](GLB_ size_t pos) const { return block_[pos]; }
};

template <typename T, typename U, typename A>
void swap(R2Y_ scope_block<T, A> & a, R2Y_ scope_block<U, A> & b)
{
    a.swap(b);
}