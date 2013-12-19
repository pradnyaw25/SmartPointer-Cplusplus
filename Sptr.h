#ifndef _SPTR_HPP
#define _SPTR_HPP

#include <pthread.h>

namespace cs540 {
  class MyClass {
    private:
      int referencectr;
      pthread_mutex_t lock;

    public:
      MyClass() : referencectr(1) { pthread_mutex_init(&lock, 0); }
      virtual ~MyClass() {}

      int decrement() {
        int r;
        pthread_mutex_lock(&lock);
        if(referencectr > 1) {
          r = --referencectr;
        } else {
          r = referencectr;
        }

        pthread_mutex_unlock(&lock);
        return r;
      }

      int increment() {
int r=0;
        pthread_mutex_lock(&lock);
if(referencectr>0){
         r = ++referencectr;
}
        pthread_mutex_unlock(&lock);
        return r;
      }
  };

  template <typename U>
  class MyClass_t : public MyClass {
    private:
      U* object;

    public:
      MyClass_t(U* o) : object(o) {}
      virtual ~MyClass_t() { delete object;}
  };

  template <typename T>
  class Sptr {
    typedef void (Sptr<T>::*unspecified_bool_type)() const;
    void nosupportcomparisons() const {}

    template <typename A> friend class Sptr;

    public:
      T* object;

    private:
      MyClass* objct;

    public:
      Sptr() : object(0), objct(0) {}

      ~Sptr() {
        if(objct != 0 && objct->decrement() == 0) delete objct;
      }

      template <typename U>
      Sptr(U* o) : object(o), objct(new MyClass_t<U>(o)) {}

      Sptr(const Sptr& s) : object(s.object), objct(s.objct) {
        objct->increment();
      };

      template <typename U>
      Sptr(const Sptr<U>& s) : object(static_cast<T*>(s.object)), objct(s.objct) {
        objct->increment();
      }

      template <typename U>
      Sptr(const Sptr<U>& s, int) : object(dynamic_cast<T*>(s.object)), objct(s.objct) {
        objct->increment();
      }

      Sptr& operator=(const Sptr& s) {
        if(!(*this == s)) {
          if(objct != 0 && objct->decrement() == 0) { delete objct; objct = 0; object = 0; }
          object = s.object;
          objct = s.objct;
          objct->increment();
        }
        return *this;
      }

      template <typename U>
      Sptr<T>& operator=(const Sptr<U>& s) {
        if(objct != 0 && objct->decrement() == 0) { delete objct; objct = 0; object = 0; }
        object = static_cast<T*>(const_cast<U*>(s.object));
        objct = s.objct;
        objct->increment();
        return *this;
      }

      void reset() {
        if(objct != 0 && objct->decrement() == 0) { delete objct; objct = 0; object = 0; }
        object = 0;
        objct = 0;
      }
      T* operator->() const { return object; }
      T& operator*() const { return *object; }


      T* get() const { return object; }

      operator unspecified_bool_type() const {
        return object != 0 ? &Sptr<T>::nosupport_comparisons : 0;
      }
  };

  template <typename T1, typename T2>
  bool operator==(const Sptr<T1>& one, const Sptr<T2>& two) {
    return one.object == two.object;
  }

  template <typename T, typename U>
  Sptr<T> static_pointer_cast(const Sptr<U>& s) {
    return Sptr<T>(s);
  }

  template <typename T, typename U>
  Sptr<T> dynamic_pointer_cast(const Sptr<U>& s) {
    return Sptr<T>(s, 1);
  }
}

#endif
