#include "omfg_script.h"
#include "util/macros.h"
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <iostream>
#include "util/stringbuild.h"
#include "util/text.h"
#include <allegro.h>
using std::auto_ptr;
using std::cout;
using std::endl;

class BaseAction;

namespace OmfgScript
{
	
void Location::print(std::string const& msg) const
{
	std::cerr << file << ':' << line << ": " << msg << '\n';
}

struct ParamDef
{
	struct Param
	{
		Param(std::string const& name_, bool optional_ = true)
		: name(name_), optional(optional_)
		{
		}
		
		std::string name;
		bool optional;
	};
	
	//ParamDef& operator()(std::string const& name_)
	void addParam(std::string const& name, bool optional = true)
	{
		params.push_back(Param(name, optional));
		//return *this;
	}
	
	std::vector<Param> params;
};

struct Parameters
{
	enum
	{
		PosParamAfterNamed = (1<<0),
		MaxReachParam = (1<<1),
		Error = (1<<2),
	};
	
	Parameters(ParamDef* def, Location loc_)
	: paramDef(def), params(def->params.size(), 0)
	, cur(0), loc(loc_), flags(0)
	{
	}
	
	~Parameters()
	{
		foreach(i, params)
		{
			delete *i;
		}
	}
	
	void addParam(std::string const& name, auto_ptr<OmfgScript::TokenBase> v, Location loc_)
	{
		size_t i = 0;
		for(; i < paramDef->params.size(); ++i)
		{
			if(paramDef->params[i].name == name)
				break;
		}
		
		if(i >= paramDef->params.size())
		{
			int minDist = 1000000;
			int minParam = -1;
			for(i = 0; i < paramDef->params.size(); ++i)
			{
				if(!params[i])
				{
					int dist = levenshteinDistance(name, paramDef->params[i].name);
					if(dist < minDist)
					{
						minDist = dist;
						minParam = i;
					}
				}
			}
			
			if(minParam >= 0 && minDist <= (name.size() + 1) / 2)
				error("Unknown parameter '" + name + "'. Did you mean '" + paramDef->params[minParam].name + "'?", loc_);
			else
				error("Unknown parameter '" + name + "'", loc_);
			return; //TODO: Error
		}
		
		if(params[i])
		{
			error("Parameter '" + name + "' already defined", loc_);
			return; //TODO: Error, multi-definition
		}
		
		params[i] = v.release();
		cur = -1;
	}
	
	void addParam(auto_ptr<OmfgScript::TokenBase> v, Location loc_)
	{
		if(cur < 0 && !(flags & PosParamAfterNamed))
		{
			flags |= PosParamAfterNamed;
			error("Positional parameters not allowed after named parameters", loc_);
			return;
		}
		if(cur >= params.size() && !(flags & MaxReachParam))
		{
			flags |= MaxReachParam;
			error(S_("Too many parameters. Maximum allowed is ") << params.size() << '.', loc_);
			return;
		}
		
		params[cur] = v.release();
		++cur;
	}
	
	void checkParams()
	{
		for(size_t i = 0; i < params.size(); ++i)
		{
			if(!params[i])
			{
				if(!paramDef->params[i].optional)
					error(S_("Parameter '") << paramDef->params[i].name << "' (number " << (i+1) << ") not defined and not optional");
				else
					params[i] = new TokenBase(loc);
			}
		}
	}
	
	void error(std::string const& msg)
	{
		error(msg, loc);
	}
	
	void error(std::string const& msg, Location loc_)
	{
		loc_.print(msg);
		flags |= Error;
	}
	
	int cur;
	int flags;
	ParamDef* paramDef;
	std::vector<OmfgScript::TokenBase*> params;
	Location loc;
};

struct ActionDef
{
	ActionDef(ParamDef* paramDef_, ActionFactory::CreateFunc create_)
	: paramDef(paramDef_), create(create_)
	{
	}
	
	~ActionDef()
	{
		delete paramDef;
	}
	
	ParamDef* paramDef;
	ActionFactory::CreateFunc create;
};

struct EventDef
{
	EventDef(std::string const& name_, ParamDef* paramDef_, int type_)
	: name(name_), paramDef(paramDef_), type(type_)
	{
	}
	
	~EventDef()
	{
		delete paramDef;
	}
	
	std::string name;
	ParamDef* paramDef;
	int type;
};

int TokenBase::toColor(int r, int g, int b)
{
	if(isDefault())
		return makecol(r, g, b);
	if(assertList())
	{
		std::list<TokenBase*> const& l = toList();
		if(l.size() != 3)
			loc.print("Expected three components in colour");
		let_(i, l.begin());
		r = (*i)->toInt(); ++i;
		g = (*i)->toInt(); ++i;
		b = (*i)->toInt(); ++i;
		return makecol(r, g, b);
	}
	loc.print("Expected colour");
	return makecol(r, g, b);
}

struct List : public TokenBase
{
	List(Location loc_)
	: TokenBase(loc_)
	{
	}
	
	void add(std::auto_ptr<TokenBase> el)
	{
		elements.push_back(el.release());
	}

	virtual std::list<TokenBase*> const& toList()
	{ return elements; }
	
	virtual std::ostream& output(std::ostream& s)
	{
		s << "[";
		bool first = true;
		foreach(i, elements)
		{
			if(!first)
				s << ", ";
			else
				first = false;
			(*i)->output(s);
		}
		s << "]";
		return s;
	}
	
	virtual Type::type type()
	{ return Type::List; }
	
	~List()
	{
		foreach(i, elements)
			delete *i;
	}
	
	std::list<TokenBase *> elements;
};

Function::Function(Location loc_, std::string const& name_)
: TokenBase(loc_), name(name_)
{
}

Function::~Function()
{
	foreach(i, params)
		delete *i;
}

TokenBase* Function::operator[](size_t i) const
{
	if(i >= params.size())
	{
		loc.print(S_("Function has no parameter #") << (i+1));
		return &Parser::globalDefault;
	}
	return params[i];
}

struct Func : public Function
{
	Func(Location loc_, std::string const& name_)
	: Function(loc_, name_)
	{
	}
	
	void add(std::auto_ptr<TokenBase> el)
	{
		params.push_back(el.release());
	}
	
	virtual Function const* toFunction()
	{ return this; }
	
	virtual Type::type type()
	{ return Type::Func; }
};

struct BinOp : public TokenBase
{
	BinOp(Location loc_, TokenBase* a_, TokenBase* b_)
	: TokenBase(loc_), a(a_), b(b_)
	{
	}
	
	virtual Type::type type()
	{
		Type::type at = a->type();
		Type::type bt = b->type();
		if(at == bt)
		{
			return at;	
		}
		else
		if((at == Type::Double && bt == Type::Int)
		|| (at == Type::Int && bt == Type::Double))
			return Type::Double;
		return Type::Unknown;
	}
	
	~BinOp()
	{
		delete a;
		delete b;
	}
	
	TokenBase* a;
	TokenBase* b;
};

struct Add : public BinOp
{
	Add(Location loc_, TokenBase* a_, TokenBase* b_)
	: BinOp(loc_, a_, b_)
	{
	}
	
	virtual double toDouble()
	{
		return a->toDouble() + b->toDouble();
	}
	
	virtual int toInt()
	{
		if(a->type() == Type::Int && b->type() == Type::Int)
			return a->toInt() + b->toInt();
		return static_cast<int>(a->toDouble() + b->toDouble());
	}
};

struct Sub : public BinOp
{
	Sub(Location loc_, TokenBase* a_, TokenBase* b_)
	: BinOp(loc_, a_, b_)
	{
	}
	
	virtual double toDouble()
	{
		return a->toDouble() - b->toDouble();
	}
	
	virtual int toInt()
	{
		if(a->type() == Type::Int && b->type() == Type::Int)
			return a->toInt() - b->toInt();
		return static_cast<int>(a->toDouble() - b->toDouble());
	}
};

struct Mul : public BinOp
{
	Mul(Location loc_, TokenBase* a_, TokenBase* b_)
	: BinOp(loc_, a_, b_)
	{
	}
	
	virtual double toDouble()
	{
		return a->toDouble() * b->toDouble();
	}
	
	virtual int toInt()
	{
		if(a->type() == Type::Int && b->type() == Type::Int)
			return a->toInt() * b->toInt();
		return static_cast<int>(a->toDouble() * b->toDouble());
	}
};

struct Div : public BinOp
{
	Div(Location loc_, TokenBase* a_, TokenBase* b_)
	: BinOp(loc_, a_, b_)
	{
	}
	
	virtual double toDouble()
	{
		return a->toDouble() / b->toDouble();
	}
	
	virtual int toInt()
	{
		if(a->type() == Type::Int && b->type() == Type::Int)
			return static_cast<int>(a->toInt() / b->toDouble());
		return static_cast<int>(a->toDouble() / b->toDouble());
	}
};

}

#include "omfg_script_parser.h"

namespace OmfgScript
{
	
struct ActionFactoryImpl
{
	~ActionFactoryImpl()
	{
		foreach(i, actionDefs)
			delete i->second;
	}
	
	std::map<std::string, ActionDef*> actionDefs;
};

ParamProxy::ParamProxy(ParamDef* paramDef_)
: paramDef(paramDef_)
{
}

ParamProxy const& ParamProxy::operator()(std::string const& name, bool optional) const
{
	paramDef->addParam(name, optional);
	return *this;
}

ActionFactory::ActionFactory()
: Pimpl<ActionFactoryImpl>(new ActionFactoryImpl())
{
}
	
ActionFactory::~ActionFactory()
{
	delete pimpl;
}

ParamProxy ActionFactory::add(std::string const& name, ActionFactory::CreateFunc createFunc)
{
	ParamDef* paramDef = new ParamDef;
	pimpl->actionDefs[name] = new ActionDef(paramDef, createFunc);
	return ParamProxy(paramDef);
}

ActionDef* ActionFactory::operator[](std::string const& name)
{
	let_(i, pimpl->actionDefs.find(name));
	if(i == pimpl->actionDefs.end())
		return 0;
	return i->second;
}

struct ParserImpl : public TGrammar<ParserImpl>
{
	struct Event
	{
		Event(EventDef* def_, Parameters* params_, std::vector<BaseAction*>& actions_)
		: def(def_), params(params_)
		{
			actions.swap(actions_);
		}
		
		~Event()
		{
			delete params;
		}
		
		//std::string name;
		EventDef* def;
		Parameters* params;
		std::vector<BaseAction*> actions;
	};
	
	struct Property
	{
		Property(Location loc_, TokenBase* value_)
		: loc(loc_), value(value_), touched(false)
		{
		}
		
		~Property()
		{
			delete value;
		}
		
		Location   loc;
		bool       touched;
		TokenBase* value;
	};
	
	ParserImpl(std::istream& str_, ActionFactory& actionFactory_, std::string const& fileName_)
	: str(str_), actionFactory(actionFactory_), fileName(fileName_)
	{
		this->next();
	}
	
	~ParserImpl()
	{
		if(!error) // Don't bother doing this if there were errors
		{
			foreach(i, properties)
			{
				if(!i->second->touched)
					semanticWarning("Property '" + i->first + "' defined but not used", i->second->loc);
			}
		}
		
		foreach(i, properties)
			delete i->second;
		foreach(i, eventDef)
			delete i->second;
		foreach(i, events)
			delete *i;
	}
	
	size_t read(char* p, size_t s)
	{
		str.read(p, s);
		return str.gcount();
	}
	
	void reportError(std::string const& error, Location loc)
	{
		loc.print(error);
	}
	
	Location getLoc()
	{
		return Location(fileName, line);
	}
	
	void property(std::string const& name, TokenBase* t, Location loc)
	{
		properties[name] = new Property(loc, t);
	}
	
	TokenBase* getRawProperty(std::string const& name)
	{
		let_(i, properties.find(name));
		if(i != properties.end())
		{
			i->second->touched = true;
			return i->second->value;
		}
		return 0;
	}
	
	EventDef* getEventDef(std::string const& name)
	{
		let_(i, eventDef.find(name));
		if(i != eventDef.end())
		{
			return i->second;
		}
		
		int minDist = 1000000;
		std::string minEvent;
		foreach(i, eventDef)
		{
			int dist = levenshteinDistance(name, i->first);
			if(dist < minDist)
			{
				minDist = dist;
				minEvent = i->first;
			}
		}
		
		if(!minEvent.empty() && minDist <= name.size() / 2)
			semanticError("Unknown event '" + name + "'. Did you mean '" + minEvent + "'?");
		else
			semanticError("Unknown event '" + name + "'");
		return 0;
	}
	
	ActionDef* getAction(std::string const& name)
	{
		let_(i, actionFactory[name]);
		if(i)
		{
			return i;
		}
		
		
		int minDist = 1000000;
		std::string minAction;
		foreach(i, actionFactory.pimpl->actionDefs)
		{
			int dist = levenshteinDistance(name, i->first);
			if(dist < minDist)
			{
				minDist = dist;
				minAction = i->first;
			}
		}
		
		if(!minAction.empty() && minDist <= name.size() / 2)
			semanticError("Unknown action '" + name + "'. Did you mean '" + minAction + "'?");
		else
			semanticError("Unknown action '" + name + "'");

		return 0;
	}
	
	BaseAction* createAction(ActionDef* action, std::auto_ptr<Parameters> params)
	{
		return action->create(params->params);
	}
	
	void addEvent(EventDef* event, std::auto_ptr<Parameters> params, std::vector<BaseAction*>& actions)
	{
		events.push_back(new Event(event, params.release(), actions));
	}
	
	std::istream& str;
	std::string fileName;
	std::map<std::string, Property*> properties;
	std::list<Event*> events;
	
	//
	std::map<std::string, EventDef*> eventDef;
	ActionFactory& actionFactory;
};

struct EventData
{
	typedef std::list<ParserImpl::Event*>::const_iterator Iter;
	EventData(Iter i_, Iter end_)
	: i(i_), end(end_)
	{
	}
	
	Iter i;
	Iter end;
};

TokenBase Parser::globalDefault(Location("<nowhere>", 0));

#define self (static_cast<EventData *>(data))
Parser::EventIter::~EventIter()
{
	delete self;
}
	
Parser::EventIter& Parser::EventIter::operator++()
{
	++self->i;
	return *this;
}

Parser::EventIter::operator bool()
{
	return self->i != self->end;
}

int Parser::EventIter::type()
{
	return (*self->i)->def->type;
}
	
std::vector<TokenBase*> const& Parser::EventIter::params()
{
	return (*self->i)->params->params;
}

std::vector<BaseAction*>& Parser::EventIter::actions()
{
	return (*self->i)->actions;
}

Parser::EventIter::EventIter(Parser& parser)
: data(new EventData(parser.pimpl->events.begin(), parser.pimpl->events.end()))
{
	
}

#undef self

Parser::Parser(std::istream& str, ActionFactory& actionFactory, std::string const& fileName)
: Pimpl<ParserImpl>(new ParserImpl(str, actionFactory, fileName))
{
}
	
Parser::~Parser()
{
	delete pimpl;
}

ParamProxy Parser::addEvent(std::string const& name, int type)
{
	ParamDef* paramDef = new ParamDef;
	pimpl->eventDef[name] = new EventDef(name, paramDef, type);
	return ParamProxy(paramDef);
}

double Parser::getDouble(std::string const& name, double def)
{
	return getProperty(name)->toDouble(def);
}

std::string const& Parser::getString(std::string const& name, std::string const& def)
{
	return getProperty(name)->toString(def);
}

std::list<TokenBase*> const& Parser::getList(std::string const& name, std::list<TokenBase*> const& def)
{
	return getProperty(name)->toList(def);
}

Function const* Parser::getFunction(std::string const& name)
{
	TokenBase* v = getProperty(name);
	if(v->isFunction())
		return v->toFunction();
	return 0;
}

int Parser::getInt(std::string const& name, int def)
{
	return getProperty(name)->toInt(def);
}

bool Parser::getBool(std::string const& name, bool def)
{
	return getProperty(name)->toBool(def);
}

TokenBase* Parser::getProperty(std::string const& a)
{
	if(TokenBase* p = pimpl->getRawProperty(a)) return p;
	return &globalDefault;
}

TokenBase* Parser::getProperty(std::string const& a, std::string const& b)
{
	if(TokenBase* p = pimpl->getRawProperty(a)) return p;
	else if(TokenBase* p = pimpl->getRawProperty(b)) return p;
	return &globalDefault;
}

bool Parser::run()
{
	pimpl->rule_lines();
	return pimpl->full();
}

void Parser::error(std::string const& msg)
{
	pimpl->semanticError(msg);
}

}

