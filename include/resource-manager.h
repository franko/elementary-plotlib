#pragma once

class manage_owner {
public:
    template <class T>
    static void acquire(T* p) { };

    template <class T>
    static void dispose(T* p) {
        delete p;
    };
};

class manage_not_owner {
public:
    template <class T>
    static void acquire(T* p) { };

    template <class T>
    static void dispose(T* p) { };
};
