/* 
// Copyright 2007 Alexandros Panagopoulos
//
// This software is distributed under the terms of the GNU Lesser General Public Licence
//
// This file is part of BeGUI library.
//
//    BeGUI is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    BeGUI is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with BeGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _CALLBACK_H42631_INCLUDED_
#define _CALLBACK_H42631_INCLUDED_

#pragma once

class FunctorBase {
public:
	typedef void (FunctorBase::*_MemFunc)();

	FunctorBase() : m_callee(0), m_func(0) { }
	FunctorBase(const void *callee, const void *func, size_t sz) {
		if (callee) {
			m_callee = (void*)callee;
			memcpy(m_memFunc, func, sz);
		}
		else {
			m_func = func;
		}
	}

	//for evaluation in conditions
	//will be changed to bool when bool exists
	operator bool()const{return m_func||m_callee;}
	
	class DummyInit{
	};

//private:
	union {
		const void *m_func;
		char m_memFunc[sizeof(_MemFunc)];
	};
	void *m_callee;
};

class Functor0 : protected FunctorBase
{
public:
	Functor0() { };

	void operator () () const {
		if (m_callee || m_func)
			(*thunk)(*this);
	}
	FunctorBase::operator bool;

protected:
	typedef void (*Thunk)(const FunctorBase &);
	Functor0(Thunk t,const void *c,const void *f,size_t sz):
		FunctorBase(c,f,sz),thunk(t){}
private:
	Thunk thunk;

};

template <class Func>
class FunctionTranslator0:public Functor0{
public:
	FunctionTranslator0(Func f):Functor0(thunk,0,f,0){}
	static void thunk(const FunctorBase &ftor)
	{
		if (ftor.m_func)
			(Func(ftor.m_func))();
	}
};

template <class Callee, class MemFunc>
class MemberTranslator0:public Functor0{
public:
	MemberTranslator0(Callee &c,const MemFunc &m):
		Functor0(thunk,&c,&m,sizeof(MemFunc)){}
	static void thunk(const FunctorBase &ftor)
	{
		Callee *callee = (Callee *)ftor.m_callee;
		if (!callee)
			return;
		MemFunc &memFunc(*(MemFunc*)(void *)(ftor.m_memFunc));
		(callee->*memFunc)();
	}
};

template <class TRT>
inline FunctionTranslator0<TRT (*)()>
makeFunctor(Functor0*,TRT (*f)())
{
	return FunctionTranslator0<TRT (*)()>(f);
}

template <class Callee,class TRT,class CallType>
inline MemberTranslator0<const Callee,TRT (CallType::*)()const>
makeFunctor(/*Functor0*,*/const Callee &c,TRT (CallType::* const &f)()const)
	{
	typedef TRT (CallType::*MemFunc)()const;
	return MemberTranslator0<const Callee,MemFunc>(c,f);
	}

template <class Callee,class TRT,class CallType>
inline MemberTranslator0<Callee,TRT (CallType::*)()>
makeFunctor(/*Functor0*,*/Callee &c,TRT (CallType::* const &f)())
	{
	typedef TRT (CallType::*MemFunc)();
	return MemberTranslator0<Callee,MemFunc>(c,f);
	}

/************************* one arg - no return *******************/
template <class P1>
class Functor1:protected FunctorBase{
public:
	Functor1(DummyInit * = 0){}
	void operator()(P1 p1)const
	{
		if (m_callee || m_func)
			thunk(*this,p1);
	}
	FunctorBase::operator bool;
protected:
	typedef void (*Thunk)(const FunctorBase &,P1);
	Functor1(Thunk t,const void *c,const void *f,size_t sz):
		FunctorBase(c,f,sz),thunk(t){}
private:
	Thunk thunk;
};


template <class P1,class Func>
class FunctionTranslator1:public Functor1<P1>{
public:
	FunctionTranslator1(Func f):Functor1<P1>(thunk,0,f,0){}
	static void thunk(const FunctorBase &ftor,P1 p1)
	{
		if (ftor.m_func)
			(Func(ftor.m_func))(p1);
	}
};


template <class P1,class Callee, class MemFunc>
class MemberTranslator1:public Functor1<P1>{
public:
	MemberTranslator1(Callee &c,const MemFunc &m):
		Functor1<P1>(thunk,&c,&m,sizeof(MemFunc)){}
	static void thunk(const FunctorBase &ftor,P1 p1)
		{
		Callee *callee = (Callee *)ftor.m_callee;
		if (!callee)
			return;
		MemFunc &memFunc(*(MemFunc*)(void *)(ftor.m_memFunc));
		(callee->*memFunc)(p1);
		}
};

template <class P1,class TRT,class TP1>
inline FunctionTranslator1<P1,TRT (*)(TP1)>
makeFunctor(Functor1<P1>*,TRT (*f)(TP1))
	{
	return FunctionTranslator1<P1,TRT (*)(TP1)>(f);
	}


template </*class P1,*/class Callee,class TRT,class CallType,class TP1>
inline MemberTranslator1<TP1/*was P1*/,Callee,TRT (CallType::*)(TP1)>
makeFunctor(/*Functor1<P1>*,*/Callee &c,TRT (CallType::* const &f)(TP1))
	{
	typedef TRT (CallType::*MemFunc)(TP1);
	return MemberTranslator1<TP1/*was P1*/,Callee,MemFunc>(c,f);
	}

template </*class P1,*/class Callee,class TRT,class CallType,class TP1>
inline MemberTranslator1<TP1/*was P1*/, Callee,TRT (CallType::*)(TP1) const>
makeFunctor(/*Functor1<P1>*,*/Callee &c,TRT (CallType::* const &f)(TP1) const)
	{
	typedef TRT (CallType::*MemFunc)(TP1) const;
	return MemberTranslator1<TP1/*was P1*/,Callee,MemFunc>(c,f);
	}

#endif