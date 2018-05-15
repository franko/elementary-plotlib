#pragma once

/* Meant to be used like: unique_ptr_container<agg::pod_bvector, pod_class_type>.

   The unsigned... template arguments are needed because some containers like agg::pod_bvector takes
   some unsigned template parameters to define some container-specific sizes. */
template <template<typename, unsigned...> typename PodContainer, typename T, unsigned... ContainerSizes>
class unique_ptr_container {
public:
    unique_ptr_container() { }

    ~unique_ptr_container() {
        delete_objects();
    }

    unsigned size() const { return m_array.size(); }

    void clear() {
        delete_objects();
        m_array.clear();
    }

    void push_back(T* v) { m_array.push_back(v); }

    const T* operator [] (unsigned i) const { return m_array[i]; }
          T* operator [] (unsigned i)       { return m_array[i]; }
    const T* at(unsigned i) const           { return m_array[i]; }
          T* at(unsigned i)                 { return m_array[i]; }

private:
    void delete_objects() {
        for (unsigned i = 0; i < m_array.size(); i++) {
            delete m_array[i];
        }
    }

    PodContainer<T*, ContainerSizes...> m_array;
};
