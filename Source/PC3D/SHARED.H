#ifndef SHARED_H
#define SHARED_H
// .......... a simple class for sharing objects. Note the equality operator (==) is supported
// .......... The object contained by the share wrapper must be dynamically allocated.
template <class T> class shared
	{
	struct shareRec {
		T *sharedObj;
		int refcount;
		~shareRec() {delete sharedObj;}
		};

	T *		t;
	shareRec *	rec;
public:
			shared(shared<T>& init) {rec = init.rec; rec->refcount++; t = rec->sharedObj;}
			shared(T *contains ) : t(contains) {
							assert(contains);
							rec = new shareRec();
							rec->sharedObj = contains;
							rec->refcount = 1;
							}

			~shared() {if (!(--(rec->refcount))) delete rec; }

	int 		getRefCount() const {return rec->refcount;}

	BOOL		operator ==(shared<T>& s) {return rec == s.rec;}
	void		operator =(shared<T>& s) {assert(0);/*cannot call assignment operator*/}

	operator T &() const {return *t;}
	};

#endif
