/*------------------------------------------------------------------------*/
/*                                                                        */
/*  VECTLIST.H                                                             */
/*                                                                        */
/*------------------------------------------------------------------------*/
#ifndef VECTLIST_H
#define VECTLIST_H

#include <assert.h>
#include "memory.h"
#include "common.h"



const UINT MAX_VECTLIST_INDEX = 0xFFFF;
const UINT NOT_FOUND = MAX_VECTLIST_INDEX;

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  template <class T> class vectList                                 */
/*                                                                        */
/*  Implements a vector of objects of type T.  Assumes that               */
/*  T has meaningful copy semantics and a default constructor.            */
/*                                                                        */
/*------------------------------------------------------------------------*/

template <class T> class vectIterator;

// ===========================================================================
// vectList class ============================================================
// ===========================================================================
template <class T>
class vectList
{
  friend class vectIterator<T>;

  public:
    vectList() : data(0), lim(0) { }

    vectList(UINT sz) : data(sz ? new T[sz] : 0), lim(sz) {   }

    vectList(const vectList<T>&); 
    const vectList<T>& operator=(const vectList<T>&);

    ~vectList() { if (data) delete [] data; }

    T& operator[](UINT index) const
    {
      assert(lim > 0 && data != 0 && index < lim);
      return data[index];
    }

    UINT limit() const    { return lim; }

    virtual UINT top() const  { return lim; }

    void resize(UINT, UINT = 0);

    void forEach(void (*f)(T&, void*), void* args)
    { forEach(f, args, 0, lim); }

    void forEach(void (*)(T&, void*), void*, UINT, UINT);

    T* firstThat(int (*)(const T&, void*), void*, UINT, UINT) const;

    T* firstThat(int (*cond)(const T&, void*), void* args) const
    { return(firstThat(cond, args, 0, lim)); }

    T* lastThat(int (*)(const T&, void*), void*, UINT, UINT) const;

    T* lastThat(int (*cond)(const T&, void*), void *args) const
    { return(lastThat(cond, args, 0, lim)); }

    virtual UINT getDelta() const   { return(0); }

  protected:

    T* data;
    UINT lim;

    virtual void zero(UINT, UINT)   { }
    virtual void removeData(T)      { }
};


template <class T>
vectList<T>::vectList(const vectList<T>& v) : data(new T[v.lim]), lim(v.lim)
{
  assert(lim == 0 || (data != 0 && v.data != 0));
  for (UINT i = 0; i < lim; i++)
    data[i] = v.data[i];
}

template <class T>
const vectList<T>& vectList<T>::operator =(const vectList<T>& v)
{
  if (data != v.data)
  {
    delete [] data;
    data = new T[v.lim];
    assert(data != 0);
    for(UINT i = 0, lim = v.lim; i < lim; i++)
      data[i] = v.data[i];
  }

  return *this;
}


inline UINT nextDelta(UINT sz, UINT delta)
{
  return((sz%delta) ? ((sz+delta)/delta)*delta : sz);
}


template <class T>
void vectList<T>::resize(UINT newSz, UINT offset)
{
  if (newSz <= lim || getDelta() == 0)
    return;

  UINT sz = lim + nextDelta(newSz - lim, getDelta());
  T* temp = new T[sz];
  UINT last = min(sz - offset, lim);
  
  for( UINT i = 0; i < last; i++ ) // should replace with a mepcpy
    temp[i + offset] = data[i];

  if (data)
    delete [] data;

  data = temp;
  lim = sz;
  zero(last + offset, sz);
}


template <class T>
void vectList<T>::forEach(void (*f)(T&, void*), void* args, UINT start,
                          UINT stop)
{
  for(UINT cur = start; cur < stop; cur++)
    f(data[cur], args);
}


template <class T>
T* vectList<T>::firstThat(int(*cond)(const T&, void*), void* args, 
                          UINT start, UINT stop) const
{
  for (UINT cur = start; cur < stop; cur++)
  {
    if (cond(data[cur], args) != 0)
      return &data[cur];
  }
  return NULL;
}


template <class T>
T* vectList<T>::lastThat(int (*cond)(const T&, void*), void *args,
                         UINT start, UINT stop) const
{
  T* res = 0;
  for (UINT cur = start; cur < stop; cur++ )
  {
    if (cond(data[cur], args) != 0)
      res = &data[cur];
  }
  return res;
}



/*------------------------------------------------------------------------*/
/*                                                                        */
/*  template <class T> class CvectList                                */
/*                                                                        */
/*  Implements a counted vector of objects of type T.  Assumes that       */
/*  T has meaningful copy semantics and a default constructor.            */
/*                                                                        */
/*------------------------------------------------------------------------*/

template <class T>
class CvectList : public vectList<T>
{
  public:
    CvectList() : count_(0), delta(8)  {    }

    CvectList( UINT sz, UINT d = 8 ) : vectList<T>(sz), count_(0), delta(d) {}

    void add(T);
    void add(T*, UINT num);
    void addAt(T, UINT);

    void detach(UINT loc, int del);
    void detach(T t)                { detach(find(t), 0); }
    void detachAll()                { count_ = 0;         }
    void push(T t)                  { add(t);             }
    T pop();
    T getTop();

    int isEmpty() const   { return(count_ == 0);  }

    void forEach(void (*f)(T&, void*), void* args)
    { forEach(f, args, 0, count_); }

    void forEach(void (*func)(T&, void*), void* args, UINT low, UINT high)
    { vectList<T>::forEach( func, args, low, high ); }

    T* firstThat(int (*cond)(const T&, void*), void* args) const
    { return firstThat(cond, args, 0, count_); }

    T* firstThat(int (*cond)(const T&, void*), void* args, UINT low,
                 UINT high) const
    {
      return vectList<T>::firstThat( cond, args, low, high );
    }

    T* lastThat(int (*cond)(const T&, void*), void* args) const
    { return lastThat(cond, args, 0, count_); }

    T* lastThat(int (*cond)(const T&, void*), void* args, UINT low,
                UINT high) const
    {
      return vectList<T>::lastThat( cond, args, low, high );
    }


    virtual UINT find(T) const;
    virtual UINT top() const        { return(count_);       }
    virtual UINT getDelta() const   { return(delta);        }
    int count() const               { return((int)count_);  }
    void setDelta(UINT d)           { delta = d;            }

protected:

    UINT count_;
    UINT delta;
};


template <class T>
void CvectList<T>::add(T t)
{
  if (++count_ > lim)
    resize(count_);
  data[count_-1] = t;
}


template <class T>
void CvectList<T>::add(T* pt, UINT num)
{
  UINT loc = count_;
  count_ += num;

  if (count_ > lim)
    resize(count_);

  memcpy(&data[loc], pt, num * sizeof(T));
}


template <class T>
void CvectList<T>::addAt(T t, UINT loc)
{
  if( loc >= lim )
    resize( loc+1 );
  data[loc] = t;
}


template <class T>
void CvectList<T>::detach(UINT loc, int del)
{
  if (loc >= lim) return;
  if (del) removeData(data[loc]);
  if (loc >= count_)
  {
    zero(loc, loc+1); // removing an element that's not
    return;           // in the counted portion
  }

  count_--;
  for (UINT cur = loc; cur < count_; cur++) // should replace this with a memcpy
    data[cur] = data[cur+1];

  zero(count_, count_ + 1);
}


template <class T>
UINT CvectList<T>::find(T t) const
{
  if (count_ != 0)
  {
    for (UINT loc = 0; loc < count_; loc++)
      if (data[loc] == t)
    return(loc);
  }

  return NOT_FOUND;
}


template <class T>
T CvectList<T>::pop()
{
  assert(count_);
  T ret = data[count_-1];
  detach(count_ - 1, 0);
  return ret;
}


template <class T>
T CvectList<T>::getTop()
{
  assert(count_);
  T ret = data[count_ - 1];
  return ret;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  template <class T> class SvectList                                */
/*                                                                        */
/*  Implements a sorted vector of objects of type T.  Assumes that        */
/*  T has meaningful copy semantics, a meaningful < operator,             */
/*  and a default constructor.                                            */
/*                                                                        */
/*------------------------------------------------------------------------*/

template <class T>
class SvectList : public CvectList<T>
{
  public:
    SvectList() { }

    SvectList( UINT sz, UINT d = 0 ) : CvectList<T>( sz, d ) {      }

    void add( T );
    void add( T, UINT nTimes );
    void add( T*, UINT num ) {assert(0); /* not implemented */}

    virtual UINT find( T ) const;

};


template <class T>
void SvectList<T>::add(T t)
{
  UINT loc = count_++;
  if( count_ > lim )
    resize( count_ );

  while( loc > 0 && t < data[loc-1] )
  {
    data[loc] = data[loc-1];
    loc--;
  }

  data[loc] = t;
}


template <class T>
void SvectList<T>::add(T t, UINT nTimes)
{
  if(!nTimes)
    return;

  UINT loc = count_;
  count_ += nTimes;

  if( count_ > lim )
    resize( count_ );

  while( loc > 0 && t < data[loc-1] )
  {
    data[loc+nTimes-1] = data[loc-1];
    loc--;
  }

  while ( nTimes-- )
    data[loc++] = t;
}



template <class T>
UINT SvectList<T>::find( T t ) const
{
  UINT lower = 0;
  UINT upper = count_-1;
  if( count_ != 0 )
  {
    while( lower < upper && upper != MAX_VECTLIST_INDEX )
    {
      UINT middle = (lower+upper)/2;
      if( data[middle] == t )
        return middle;
      if( data[middle] < t )
        lower = middle+1;
      else
        upper = middle-1;
    }
  }

  if( lower == upper && data[lower] == t )
    return lower;
  else

  return MAX_VECTLIST_INDEX;
}



/*------------------------------------------------------------------------*/
/*                                                                        */
/*  template <class T> class vectIterator                         */
/*                                                                        */
/*  Implements a vector iterator.  This iterator works with any direct    */
/*  vector.  For indirect vectors, see IvectIterator.             */
/*                                                                        */
/*------------------------------------------------------------------------*/

// ===========================================================================
template <class T> 
class vectIterator
{
  public:
    vectIterator(const vectList<T>& v) { vect = &v; restart(0,v.limit()); }

    vectIterator( const vectList<T>& v, UINT start, UINT stop)
    { vect = &v; restart( start, stop ); }

    operator int() { return cur < upper; }

    T current() { return (cur < upper) ? (*vect)[cur] : (*vect)[upper-1]; }

    T operator++(int)
    {
      if( cur >= upper ) return((*vect)[upper-1]);
      return((*vect)[cur++]);
    }

    T operator++()
    {
      if( cur < upper )   cur++;
      if( cur >= upper )  return (*vect)[upper-1];
      return (*vect)[cur];
    }

    void restart()    { restart(lower, upper); }

    void restart( UINT start, UINT stop )
    { cur = lower = start; upper = stop; }

  private:

    const vectList<T>  *vect;
    UINT cur;
    UINT lower, upper;

};

#endif /* end of file */
// ===========================================================================
