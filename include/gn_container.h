#pragma once
#include <type_traits>
#include <utility>


template<typename Container, typename IterBase, typename Key, typename Value, typename KVPair>
struct ContainerWrapper {
public:
    inline ContainerWrapper(Container container): m_container(std::move(container)) {}

    inline IterBase k_insert(KVPair&& val) { return m_container.insert(std::move(val)); }

    inline IterBase k_find(const Key& key) { return m_container.find(key); }
    inline IterBase k_lower_bound(const Key& key) { return m_container.lower_bound(key); }
    inline IterBase k_upper_bound(const Key& key) { return m_container.upper_bound(key); }
    inline bool     k_exists(const IterBase& iter) const { return m_container.exists(iter); }

    inline IterBase k_begin() { return m_container.begin(); }
    inline IterBase k_end()   { return m_container.end(); }

    inline void   k_forward (IterBase& path)  { m_container.forward(path); }
    inline void   k_backward(IterBase& path) { m_container.backward(path); }
    inline KVPair k_deleteIter(IterBase iter) {return m_container.deleteIter(iter); }
    inline KVPair k_getHolder(IterBase iter) { return m_container.getHolder(iter); }

    using valueref = decltype(std::declval<Container>().getHolderValue(std::declval<IterBase>()));
    inline valueref k_getHolderValue(IterBase iter) { return m_container.getHolderValue(iter); }

    template<typename T>
    inline void k_setHolderValue(IterBase iter, T val) { return m_container.setHolderValue(iter, val); }

private:
    Container m_container;
};


template<bool reverse, bool const_iterator, typename Container, typename IterBase, typename Key, typename Value, typename KVPair, typename IterCategory>
class RBTreeImplIterator {
    public:
        using iterator_category = IterCategory;
        using value_type        = Value;
        using difference_type   = long;
        using pointer           = value_type*;
        using reference         = typename std::conditional<const_iterator,const value_type&,value_type&>::type;
        using const_reference   = const value_type&;

    private:
        const ContainerWrapper<Container,IterBase,Key,Value,KVPair>& m_container;
        IterBase m_iter;

    protected:
        template<
            typename _Key, typename _Value, bool multi, bool keep_position_info,
            typename Compare,
            typename Alloc>
        friend class generic_container;

    public:
        const IterBase& iter() const { return this->m_iter; }

        explicit operator bool() const { return m_container.k_exists(m_iter); }

        RBTreeImplIterator& operator++() {
            assert(m_container.k_exists(m_iter));
            m_container.k_forward(m_iter);
            return *this;
        }

        RBTreeImplIterator operator++(int) {
            auto ans = *this;
            this->operator++();
            return ans;
        }

        RBTreeImplIterator& operator--() {
            m_container.k_backward(m_iter);
            return *this;
        }

        RBTreeImplIterator operator--(int) {
            auto ans = *this;
            this->operator--();
            return ans;
        }

        bool operator==(const RBTreeImplIterator& oth) const {
            auto t1 = this->check_version(), t2 = oth.check_version();
            return t1 == t2 && oth.node == this->node;
        }

        bool operator!=(const RBTreeImplIterator& oth) const {
            return !this->operator==(oth);
        }

        bool operator<(const RBTreeImplIterator& oth) const {
            auto t1 = this->check_version(), t2 = oth.check_version();
            if (t1 != t2) {
                throw std::logic_error("it's invalid to compare iterators from different container");
            }
            const auto idx1 = t1->indexof(this->node), idx2 = t1->indexof(oth.node);
            return reverse ? idx1 > idx2 :  idx1 < idx2;
        }

        bool operator>(const RBTreeImplIterator& oth) const {
            auto t1 = this->check_version(), t2 = oth.check_version();
            if (t1 != t2) {
                throw std::logic_error("it's invalid to compare iterators from different container");
            }
            const auto idx1 = t1->indexof(this->node), idx2 = t1->indexof(oth.node);
            return reverse ? idx1 < idx2 :  idx1 > idx2;
        }

        inline bool operator<=(const RBTreeImplIterator& oth) const {
            return oth.operator>(*this);
        }

        inline bool operator>=(const RBTreeImplIterator& oth) const {
            return oth.operator<(*this);
        }

        RBTreeImplIterator(std::weak_ptr<rbtree_t> tree, nodeptr_t node, size_t version): tree(tree), node(node), version(version) {}
};

template<
    typename _Key, typename _Value, bool multi, bool keep_position_info,
#if __cplusplus >= 202002
    C_KeyCompare<_Key> Compare = default_compare_t<_Key>,
#else
    typename Compare = default_compare_t<_Key>,
#endif // __cplusplus >= 202002
    typename Alloc = default_allocato_t<_Key,_Value>>
class generic_container_x {
    protected:
        using rbtree_t = RBTreeImpl<_Key,_Value,multi,keep_position_info,Compare,Alloc>;
        std::shared_ptr<rbtree_t> rbtree;

    public:
        using rbtree_storage_type = typename rbtree_t::storage_type;
        using rbtree_storage_type_base = typename rbtree_t::storage_type::storage_type_base;

        using key_type               = typename rbtree_t::key_type;
        using mapped_type            = typename rbtree_t::mapped_type;
        using value_type             = typename rbtree_t::value_type;
        using size_type              = typename rbtree_t::size_type;
        using difference_type        = typename rbtree_t::difference_type;
        using key_compare            = typename rbtree_t::key_compare;
        using allocator_type         = typename rbtree_t::allocator_type;
        using reference              = typename rbtree_t::reference;
        using const_reference        = typename rbtree_t::const_reference;
        using pointer                = typename rbtree_t::pointer;
        using const_pointer          = typename rbtree_t::const_pointer;
        using iterator               = RBTreeImplIterator<false,false,rbtree_t>;
        using const_iterator         = RBTreeImplIterator<false,true,rbtree_t>;
        using reverse_iterator       = RBTreeImplIterator<true,false,rbtree_t>;
        using reverse_const_iterator = RBTreeImplIterator<true,true,rbtree_t>;

    private:
        class node_type_generic {
        public:
            using key_type       = typename generic_container::key_type;
            using value_type     = typename generic_container::value_type;
            using mapped_type    = typename generic_container::mapped_type;
            using allocator_type = typename generic_container::allocator_type;

        private:
            using storage_allocator_ = typename std::allocator_traits<allocator_type>::template rebind_alloc<typename rbtree_t::rbtree_node_type>;
            storage_allocator_ _allocator;

        protected:
            using nodeptr_t        = typename rbtree_t::nodeptr_t;
            using rbtree_node_type = typename rbtree_t::rbtree_node_type;
            nodeptr_t _node;
            friend class generic_container;

            inline nodeptr_t get() {
                if (this->_node == nullptr) {
                    throw std::logic_error("get empty node");
                }

                auto ans = this->_node;
                this->_node = nullptr;
                return ans;
            }

            inline void restore(nodeptr_t node) {
                if (this->_node != nullptr) {
                    throw std::logic_error("restore to an non-empty node");
                }
                this->_node = node;
            }

            node_type_generic(nodeptr_t node, const storage_allocator_& alloc): _node(node), _allocator(alloc) {
            }

            inline const rbtree_node_type& get_node() const {
                if (this->_node == nullptr) {
                    throw std::logic_error("access empty node");
                }
                return *this->_node;
            }

        public:
            constexpr node_type_generic(): _node(nullptr) {}
            node_type_generic(const node_type_generic& oth) = delete;
            node_type_generic(node_type_generic&& oth): _node(oth._node), _allocator(oth._allocator)
            {
                oth._node = nullptr;
            }

            node_type_generic& operator=(const node_type_generic& oth) = delete;
            node_type_generic& operator=(node_type_generic&& oth) {
                this->~node_type_generic();
                this->_node = oth._node;
                oth._node = nullptr;
                return *this;
            }

#if __cplusplus >= 202002
            [[ nodiscard ]] bool empty() const noexcept
#else
            bool empty() const noexcept
#endif // __cplusplus >= 202002
            {
                return this->_node == nullptr;
            }

            explicit operator bool() const noexcept {
                return this->_node != nullptr;
            }

            allocator_type get_allocator() const {
                return this->_allocator;
            }

            void swap(node_type_generic& oth) noexcept(
                    std::allocator_traits<allocator_type>::propagate_on_container_swap::value ||
                    std::allocator_traits<allocator_type>::is_always_equal::value)
            {
                const bool propagate_on_container_swap = 
                    std::allocator_traits<allocator_type>::propagate_on_container_swap;

                if (!(this->_node == nullptr || oth._node == nullptr || 
                      propagate_on_container_swap || this->_allocator == oth._allocator))
                {
                    throw std::logic_error("undefined behavior");
                }

                if (propagate_on_container_swap || this->_allocator == oth._allocator)
                {
                    std::swap(this->_node, oth._node);
                    std::swap(this->_allocator, oth._allocator);
                } else {
                    nodeptr_t pp1 = nullptr, pp2 = nullptr;
                    if (this->_node != nullptr) {
                        auto ptr1 = this->_allocator.allocate(1);
                        pp1 = new (ptr1) rbtree_node_type(*oth._node);
                    }

                    if (oth._node != nullptr) {
                        auto ptr2 = oth._allocator.allocate(1);
                        pp2 = new (ptr2) rbtree_node_type(*this->_node);
                    }

                    if (this->_node) this->~node_type_generic();
                    if (oth._node)   oth.~node_type_generic();

                    this->_node = pp1;
                    oth._node = pp2;
                }
            }

            ~node_type_generic() {
                if (this->_node != nullptr) {
#if __cplusplus >= 201703
                    std::destroy_n(this->_node, 1);
#else
                    _node->~rbtree_node_type();
#endif // __cplusplus >= 201703
                    this->_allocator.deallocate(this->_node, 1);
                    this->_node = nullptr;
                }
            }
        };
        class node_type_set: public node_type_generic {
        public:
            using key_type       = typename node_type_generic::key_type;
            using value_type     = typename node_type_generic::value_type;
            using allocator_type = typename node_type_generic::allocator_type;

            template <typename ... Args>
            explicit node_type_set(Args&&... args): node_type_generic(std::forward<Args>(args)...) { }

            value_type& value() const {
                return const_cast<value_type&>(this->get_node().get());
            }
        };
        class node_type_map: public node_type_generic {
        public:
            using key_type       = typename node_type_generic::key_type;
            using mapped_type    = typename node_type_generic::mapped_type;
            using allocator_type = typename node_type_generic::allocator_type;

            template <typename ... Args>
            explicit node_type_map(Args&&... args): node_type_generic(std::forward<Args>(args)...) { }

            key_type& key() const {
                return const_cast<key_type&>(this->get_node().get().first);
            }

            mapped_type& mapped() const {
                return const_cast<mapped_type&>(this->get_node().get().second);
            }
        };

    public:
        using node_type              = typename std::conditional<std::is_same<_Value,void>::value,node_type_set,node_type_map>::type;
        using insert_return_type     = struct insert_return_type {
            iterator position;
            bool inserted;
            node_type node;

            insert_return_type(iterator pos, bool inserted, node_type&& nh): position(pos), inserted(inserted), node(std::move(nh)) {}
        };

        generic_container(): rbtree(std::make_shared<rbtree_t>()) {}
        explicit generic_container(const Compare& cmp, const Alloc& alloc = Alloc()): rbtree(std::make_shared<rbtree_t>(cmp, alloc)) {
        }
        explicit generic_container(const Alloc& alloc): rbtree(std::make_shared<rbtree_t>(alloc)) {
        }

        generic_container(const generic_container& oth): rbtree(std::make_shared<rbtree_t>(oth.rbtree->cmp_object(), oth.rbtree->get_allocator()))
        {
            oth.rbtree->copy_to(*this->rbtree);
        }
        generic_container(const generic_container& oth, const Alloc& alloc): rbtree(std::make_shared<rbtree_t>(oth.rbtree->cmp_object(), alloc))
        {
            oth.rbtree->copy_to(*this->rbtree);
        }

        generic_container(generic_container&& oth): rbtree(std::make_shared<rbtree_t>(oth.rbtree->cmp_object(), oth.rbtree->get_allocator()))
        {
            std::swap(oth.rbtree, this->rbtree);
        }
        generic_container(generic_container&& oth, const Alloc& alloc): rbtree(std::make_shared<rbtree_t>(oth.rbtree->cmp_object(), alloc))
        {
            std::swap(oth.rbtree, this->rbtree);
        }

#if __cplusplus >= 202002
        template<std::forward_iterator InputIt>
            requires std::constructible_from<rbtree_storage_type,typename std::iterator_traits<InputIt>::value_type>
#else
        template<
            typename InputIt, 
            typename std::enable_if<
                std::is_constructible<rbtree_storage_type,typename std::iterator_traits<InputIt>::value_type>::value &&
                std::is_convertible<typename std::iterator_traits<InputIt>::iterator_category,std::forward_iterator_tag>::value,
                bool>::type = true>
#endif // __cplusplus >= 202002
        generic_container(InputIt begin, InputIt end, const Compare& cmp = Compare(), const Alloc& alloc = Alloc()):
            rbtree(std::make_shared<rbtree_t>(cmp, alloc))
        {
            this->insert(begin, end);
        }

#if __cplusplus >= 202002
        template<std::forward_iterator InputIt>
            requires std::constructible_from<rbtree_storage_type,typename std::iterator_traits<InputIt>::value_type>
#else
        template<
            typename InputIt, 
            typename std::enable_if<
                std::is_constructible<rbtree_storage_type,typename std::iterator_traits<InputIt>::value_type>::value &&
                std::is_convertible<typename std::iterator_traits<InputIt>::iterator_category,std::forward_iterator_tag>::value,
                bool>::type = true>
#endif // __cplusplus >= 202002
        generic_container(InputIt begin, InputIt end, const Alloc& alloc):
            rbtree(std::make_shared<rbtree_t>(alloc))
        {
            this->insert(begin, end);
        }

#if __cplusplus >= 202002
        template<typename T> requires std::constructible_from<rbtree_storage_type,T>
#else
        template<typename T, typename std::enable_if<std::is_constructible<rbtree_storage_type,T>::value, bool>::type = true>
#endif // __cplusplus >= 202002
        generic_container(std::initializer_list<T> init, const Compare& cmp = {}, const Alloc& alloc = {}): rbtree(std::make_shared<rbtree_t>(cmp, alloc)) {
            this->insert(init);
        }

#if __cplusplus >= 202002
        template<typename T> requires std::constructible_from<rbtree_storage_type,T>
#else
        template<typename T, typename std::enable_if<std::is_constructible<rbtree_storage_type,T>::value, bool>::type = true>
#endif // __cplusplus >= 202002
        generic_container(std::initializer_list<T> init, const Alloc& alloc): rbtree(std::make_shared<rbtree_t>(alloc)) {
            this->insert(init);
        }

        generic_container& operator=(const generic_container& oth) {
            oth.rbtree->copy_to(*this->rbtree);
            return *this;
        }
        generic_container& operator=(generic_container&& oth) {
            this->rbtree->clear();
            std::swap(this->rbtree, oth.rbtree);
            this->rbtree->touch();
            oth.rbtree->touch();
            return *this;
        }

        Alloc get_allocator() const noexcept {
            return this->rbtree->get_allocator();
        }
        Compare key_comp() const {
            return this->rbtree->cmp_object();
        }
        Compare value_comp() const {
            return this->rbtree->cmp_object();
        }

        inline iterator begin() { return iterator(this->rbtree, this->rbtree->begin(), this->rbtree->version()); }
        inline iterator end() { return iterator(this->rbtree, nullptr, this->rbtree->version()); }

        inline const_iterator begin() const { return const_iterator(this->rbtree, this->rbtree->begin(), this->rbtree->version()); }
        inline const_iterator end() const { return const_iterator(this->rbtree, nullptr, this->rbtree->version()); }

        inline const_iterator cbegin() const { return const_iterator(this->rbtree, this->rbtree->begin(), this->rbtree->version()); }
        inline const_iterator cend() const { return const_iterator(this->rbtree, nullptr, this->rbtree->version()); }

        inline reverse_iterator rbegin() { return reverse_iterator(this->rbtree, this->rbtree->rbegin(), this->rbtree->version()); }
        inline reverse_iterator rend() { return reverse_iterator(this->rbtree, nullptr, this->rbtree->version()); }

        inline reverse_const_iterator rbegin() const { return reverse_const_iterator(this->rbtree, this->rbtree->rbegin(), this->rbtree->version()); }
        inline reverse_const_iterator rend() const { return reverse_const_iterator(this->rbtree, nullptr, this->rbtree->version()); }

        inline reverse_const_iterator crbegin() const { return reverse_const_iterator(this->rbtree, this->rbtree->rbegin(), this->rbtree->version()); }
        inline reverse_const_iterator crend() const { return reverse_const_iterator(this->rbtree, nullptr, this->rbtree->version()); }

        template<typename _K>
        iterator lower_bound(const _K& key) {
            auto lb = this->rbtree->lower_bound(key);
            return iterator(this->rbtree, lb, this->rbtree->version());
        }

        template<typename _K>
        const_iterator lower_bound(const _K& key) const {
            auto lb = this->rbtree->lower_bound(key);
            return const_iterator(this->rbtree, lb, this->rbtree->version());
        }

        template<typename _K>
        iterator upper_bound(const _K& key) {
            auto ub = this->rbtree->upper_bound(key);
            return iterator(this->rbtree, ub, this->rbtree->version());
        }

        template<typename _K>
        const_iterator upper_bound(const _K& key) const {
            auto ub = this->rbtree->upper_bound(key);
            return const_iterator(this->rbtree, ub, this->rbtree->version());
        }

        template<typename _K>
        iterator find(const _K& key) {
            auto node = this->rbtree->find(key);
            return iterator(this->rbtree, node, this->rbtree->version());
        }

        template<typename _K>
        const_iterator find(const _K& key) const {
            auto node = this->rbtree->find(key);
            return const_iterator(this->rbtree, node, this->rbtree->version());
        }

        template<typename _K>
        std::pair<iterator,iterator> equal_range(const _K& key) {
            return std::make_pair(this->lower_bound(key), this->upper_bound(key));
        }

        template<typename _K>
        std::pair<const_iterator,const_iterator> equal_range(const _K& key) const {
            return std::make_pair(this->lower_bound(key), this->upper_bound(key));
        }

        template<typename _K>
        size_t count(const _K& key) const {
            return this->rbtree->count(key);
        }

        template<typename _K>
        bool contains(const _K& key) const {
            return this->find(key) != this->end();
        }

        inline size_t size() const { return this->rbtree->size(); }
        inline bool empty() const { return this->size() == 0; }
        inline size_t max_size() const noexcept { return std::numeric_limits<size_t>::max(); }

#if __cplusplus >= 202002
        template<typename ValType> requires std::constructible_from<value_type,ValType&&>
#else
        template<typename ValType, typename std::enable_if<std::is_constructible<value_type,ValType&&>::value,bool>::type = true>
#endif // __cplusplu >= 202002
        std::pair<iterator,bool> insert(ValType&& val)
        {
            auto result = this->rbtree->insert(std::forward<ValType>(val));
            return make_pair(iterator(this->rbtree, result.first, this->rbtree->version()), result.second);
        }

#if __cplusplus >= 202002
        template<typename ValType> requires std::constructible_from<value_type,ValType&&>
#else
        template<typename ValType, typename std::enable_if<std::is_constructible<value_type,ValType>::value,bool>::type = true>
#endif // __cplusplu >= 202002
        iterator insert(iterator hint, ValType&& val)
        {
            return this->emplace_hint(hint, std::forward<ValType>(val));
        }

#if __cplusplus >= 202002
        template<typename ValType> requires std::constructible_from<value_type,ValType&&>
#else
        template<typename ValType, typename std::enable_if<std::is_constructible<value_type,ValType>::value,bool>::type = true>
#endif // __cplusplu >= 202002
        iterator insert(const_iterator hint, ValType&& val)
        {
            return this->emplace_hint(hint, std::forward<ValType>(val));
        }

#if __cplusplus >= 202002
        template<std::forward_iterator InputIt>
            requires std::constructible_from<rbtree_storage_type,typename std::iterator_traits<InputIt>::value_type>
#else
        template<
            typename InputIt, 
            typename std::enable_if<
                std::is_constructible<rbtree_storage_type,typename std::iterator_traits<InputIt>::value_type>::value &&
                std::is_convertible<typename std::iterator_traits<InputIt>::iterator_category,std::forward_iterator_tag>::value,
                bool>::type = true>
#endif // __cplusplus >= 202002
        void insert(InputIt first, InputIt last) {
            for(;first != last;first++) this->insert(*first);
        }

#if __cplusplus >= 202002
        template<typename T> requires std::constructible_from<rbtree_storage_type,T>
#else
        template<typename T, typename std::enable_if<std::is_constructible<rbtree_storage_type,T>::value, bool>::type = true>
#endif // __cplusplus >= 202002
        inline void insert(std::initializer_list<T> list) {
            this->insert(list.begin(), list.end());
        }

        insert_return_type insert(node_type&& nh) {
            if (!nh) {
                return insert_return_type(this->end(), false, std::move(nh));
            }

            if (nh.get_allocator() != this->rbtree->get_allocator()) {
                throw std::logic_error("allocator of node doesn't equal with allocator of container");
            } else {
                auto result = this->rbtree->insert_node(nullptr, nh.get());
                nh.restore(std::get<1>(result));
                iterator iter(this->rbtree, std::get<0>(result), this->rbtree->version());
                return insert_return_type(iter, std::get<2>(result), std::move(nh));
            }
        }

        iterator insert(const_iterator hint, node_type&& nh) {
            if (!nh) return this->end();

            if (nh.get_allocator() != this->rbtree->get_allocator()) {
                throw std::logic_error("allocator of node doesn't equal with allocator of container");
            } else {
                auto result = this->rbtree->insert_node(hint.nodeptr(), nh.get());
                nh.restore(std::get<1>(result));
                return iterator(this->rbtree, std::get<0>(result), this->rbtree->version());
            }
        }

        iterator insert(iterator hint, node_type&& nh) {
            return this->insert(const_iterator(hint), std::move(nh));
        }

#if __cplusplus >= 202002
        template<typename ... Args> requires std::constructible_from<value_type,Args&&...>
#else
        template<typename ... Args, typename std::enable_if<std::is_constructible<value_type,Args&&...>::value,bool>::type = true>
#endif // __cplusplu >= 202002
        inline std::pair<iterator,bool> emplace( Args&&... args ){
            auto result = this->rbtree->emplace(nullptr, std::forward<Args>(args)...);
            return std::make_pair(iterator(this->rbtree, result.first, this->rbtree->version()), result.second);
        }

#if __cplusplus >= 202002
        template<typename ... Args> requires std::constructible_from<value_type,Args&&...>
#else
        template<typename ... Args, typename std::enable_if<std::is_constructible<value_type,Args&&...>::value,bool>::type = true>
#endif // __cplusplu >= 202002
        iterator emplace_hint(const_iterator hint,  Args&&... args ){
            if (!hint.treeid()) {
                throw std::logic_error("hint is an invalid iterator");
            }

            auto result = this->rbtree->emplace(hint.nodeptr(), std::forward<Args>(args)...);
            return iterator(this->rbtree, result.first, this->rbtree->version());
        }

        iterator erase(iterator pos) {
            if (!pos.treeid()) {
                throw std::logic_error("erase an invalid iterator");
            }

            auto  node= pos.nodeptr();
            if (node == nullptr) {
                throw std::logic_error("erase end iterator");
            }
            auto next_ptr = this->rbtree->erase(node, true);
            return iterator(this->rbtree, next_ptr, this->rbtree->version());
        }

        iterator erase(const_iterator pos) {
            if (!pos.treeid()) {
                throw std::logic_error("erase an invalid iterator");
            }

            auto node = pos.nodeptr();
            if (node == nullptr) {
                throw std::logic_error("erase end iterator");
            }
            auto next_ptr = this->rbtree->erase(node, true);
            return iterator(this->rbtree, next_ptr, this->rbtree->version());
        }

        iterator erase(iterator first, iterator last) {
            return this->erase(const_iterator(first), const_iterator(last));
        }

        iterator erase(const_iterator first, const_iterator last) {
            if (first > last) {
                throw std::logic_error("invalid range");
            }

            iterator ans = this->end();
            for (;first!=this->cend() && first!=last;) {
                ans = this->erase(first);
                first = const_iterator(ans);
                last.sync_version();
            }
            return ans;
        }

        size_t erase(const _Key& key) {
            auto first = this->lower_bound(key);
            auto last = this->upper_bound(key);
            auto ans = std::distance(first, last);
            this->erase(first, last);
            return ans;
        }

        node_type extract(const_iterator position) {
            auto node = position.nodeptr();
            if (node == nullptr) {
                throw std::logic_error("extract end iterator");
            }

            auto extracted = this->rbtree->extract(node, false).first;
            return node_type(extracted, this->rbtree->get_allocator());
        }

        node_type extract(const _Key& key) {
            auto pos = this->find(key);
            if (pos == this->end()) {
                return node_type_generic();
            } else {
                return extract(key);
            }
        }

        template <typename C2, bool m>
        inline void merge(generic_container<_Key,_Value,m,keep_position_info,C2,Alloc>&& source) {
            this->merge(source);
        }

#if __cplusplus >= 202002
        template<std::forward_iterator InputIt>
            requires std::constructible_from<rbtree_storage_type,typename std::iterator_traits<InputIt>::value_type>
#else
        template<
            typename InputIt, 
            typename std::enable_if<
                std::is_constructible<rbtree_storage_type,typename std::iterator_traits<InputIt>::value_type>::value &&
                std::is_convertible<typename std::iterator_traits<InputIt>::iterator_category,std::forward_iterator_tag>::value,
                bool>::type = true>
#endif // __cplusplus >= 202002
        inline bool emplace_asc(InputIt begin, InputIt end) {
            return this->rbtree->construct_from_asc_iter(begin, end);
        }

        template <typename C2, bool m>
        void merge(generic_container<_Key,_Value,m,keep_position_info,C2,Alloc>& source) {
            if (this->get_allocator() != source.get_allocator()) {
                throw std::logic_error("allocators don't equal");
            }

            decltype(static_cast<node_type*>(nullptr)->get()) head = nullptr;
            auto node = head;
            size_t n_not_inserted = 0;
            for (;!source.empty();) {
                auto ext_node = source.extract(source.begin());
                auto result = this->insert(std::move(ext_node));
                if (!result.inserted) {
                    n_not_inserted++;
                    auto n = result.node.get();
                    if (node) {
                        node->right = n;
                        node = n;
                    } else {
                        head = n;
                        node = n;
                    }
                    node->right = nullptr;
                }
            }

            if (head != nullptr) {
                source.rbtree->construct_from_nodelist(head);
            }
        }

        void swap(generic_container& oth) noexcept {
            std::swap(this->rbtree, oth.rbtree);
        }

        void clear() {
            this->rbtree->clear();
        }
};
