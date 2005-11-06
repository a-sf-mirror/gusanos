#ifndef GRAMMAR_H
#define GRAMMAR_H

/*
	block:        [A-Za-z0-9_]
	restblock:    ';', epsilon
	command:      [A-Za-z0-9_]
	argumentList: '{', '"', [A-Za-z0-9_], epsilon
	argument:     '{', '"', [A-Za-z0-9_]
	char:         [^"], epsilon
	escape:       '\', '{', '}', '"'
	ident:        [A-Za-z0-9_]
*/

#include <cctype>
#include <list>
#include <string>
#include <exception>

struct SyntaxError : public std::exception
{
	SyntaxError(char const* desc_)
	: desc(desc_)
	{
	}
	
	virtual char const* what() const throw()
	{
		return desc;
	}
	
private:
	char const* desc;
};

struct Def;

struct Element
{
	Element()
	: sub(0), optional(false), mult(Once)
	{
	}
	
	enum RepeatType
	{
		Once,
		Repeat0,
		Repeat1,
	};
	
	RepeatType mult;
	bool optional;
	Def* sub;
	std::string code;
	std::string child;
};

struct Option
{
	void addElement(Element* el)
	{
		elements.push_back(el);
	}
	
	std::list<Element *> elements;
};

struct Def
{
	Def()
	{
	}
	
	void addOption(Option* option)
	{
		options.push_back(option);
	}

	std::string firstSet;
	std::list<Option *> options;
};

struct Rule
{
	Rule(std::string name_)
	: def(0), name(name_)
	{
	}
	
	std::string args;
	std::string name;
	Def* def;
};

struct Token
{
	Token(std::string name_)
	: name(name_)
	{
	}
	
	std::string name;
	int idx;
	std::string def;
	std::string code;
};

template<class BaseT>
struct Grammar : public BaseT
{
	Grammar(BaseT const& base)
	: BaseT(base)
	{
	}
	
	int cur()
	{
		return BaseT::cur();
	}
	
	int next()
	{
		char old = cur();
		do
		{
			BaseT::next();
		} while(firstSkip());
		
		return old;
	}
	
	int nextInLexeme()
	{
		char old = cur();
		BaseT::next();
	
		return old;
	}
	
	void doneLexeme()
	{
		while(firstSkip())
		{
			BaseT::next();
		}
	}
	
	void syntaxError(char const* str = "Syntax error")
	{
		throw SyntaxError(str);
	}
	
	bool firstSkip()
	{
		return cur() == ' ' || cur() == '\n' || cur() == '\r' || cur() == '\t';
	}
	
	bool firstIdent()
	{
		return isalnum(cur());
	}
	
	std::string ident()
	{
		if(firstIdent())
		{
			std::string i;
			for(; firstIdent(); nextInLexeme())
			{
				i += cur();
			}
			
			return i;
		}
		else
			syntaxError("Expected identifier");
		
		return "";
	}

	template<char B, char E>
	void subcode(std::string& s)
	{
		while(true)
		{
			char c = (char)cur();
			s += c;
			nextInLexeme();
			if(c == E)
			{
				return;
			}
			else
			{
				if(c == B)
					subcode<B, E>(s);
				else if(c == -1)
					syntaxError("Missing code block terminator");
			}
		}
	}
	
	template<char B, char E>
	void code(std::string& s)
	{
		if(cur() != B) syntaxError("Expected code block");
		nextInLexeme();
		
		while(cur() != E)
		{
			char c = (char)cur();
			nextInLexeme();
			s += c;
			if(c == B)
				subcode<B, E>(s);
			else if(c == -1)
				syntaxError("Missing code block terminator");
		}
		
		nextInLexeme();
	}
	
	Rule* rulei()
	{
		std::auto_ptr<Rule> rule(new Rule(ident()));
		doneLexeme();
		
		if(cur() == '<')
		{
			code<'<', '>'>(rule->args);
			doneLexeme();
		}
		
		return rule.release();
	}
	
	bool firstElement()
	{
		return firstIdent()
			| cur() == '('
			| cur() == '['
			| cur() == '{';
	}
	
	Element* element()
	{
		std::auto_ptr<Element> el(new Element);

		switch(cur())
		{
			case '(':
				next();
				el->sub = def();
				if(cur() != ')') syntaxError("Expected ')'");
				next();
			break;
			
			case '[':
				next();
				el->sub = def();
				el->optional = true;
				if(cur() != ']') syntaxError("Expected ']'");
				next();
			break;
			
			case '{':
				code<'{', '}'>(el->code);
				doneLexeme();
			break;
			
			default:
				el->child = ident();
				if(cur() == '<')
				{
					code<'<', '>'>(el->code);
				}
				doneLexeme();
			break;
		}
		
		return el.release();
	}
	
	Option* option()
	{
		std::auto_ptr<Option> op(new Option);
		
		do
		{
			std::auto_ptr<Element> el(element());
			switch(cur())
			{
				case '+': el->mult = Element::Repeat1; next(); break;
				case '*': el->mult = Element::Repeat0; next(); break;
				default :  el->mult = Element::Once; break;
			}
			op->addElement(el.release());
		} while(firstElement());
		
		return op.release();
	}
	
	Def* def()
	{
		std::auto_ptr<Def> d(new Def);
		
		d->addOption(option());
		while(cur() == '|')
		{
			next();
			d->addOption(option());
		}
		
		return d.release();
	}
		
	void rules()
	{
		if(cur() == '{')
		{
			code<'{', '}'>(this->prolog);
			doneLexeme();
		}
		
		while(firstIdent())
		{
			std::string id = ident();
			doneLexeme();
			
			if(cur() == '<')
			{
				std::auto_ptr<Rule> rule(new Rule(id));
				
				code<'<', '>'>(rule->args);
				doneLexeme();
				
				if(cur() != '=') syntaxError("Expected '=' in rule");
				next();
				
				rule->def = def();
				
				if(cur() != ';') syntaxError("Expected ';' after rule");
				next();
				
				this->addRule(rule.release());
			}
			else if(cur() == ':')
			{
				next();
				
				std::auto_ptr<Token> token(new Token(id));
				
				bool inStr = false, ignore = false;

				while(true)
				{
					int c = cur();
					
					if(c == '\\')
					{
						if(inStr)
							ignore = true;
					}
					else if(c == '\"')
					{
						if(!ignore)
						{
							inStr = !inStr;
						}
						else
							ignore = false;
					}
					else if(c == '{' || c == ';')
					{
						if(!inStr)
							break;
					}
					
					nextInLexeme();
					
					token->def += (char)c;
				}
				
				if(cur() == '{')
				{
					code<'{', '}'>(token->code);
					doneLexeme();
				}
				
				if(cur() != ';') syntaxError("Expected ';' after token");
				next();
				
				this->addToken(token.release());
			}
			else
				syntaxError("Expected rule parameters or token definition");
		}
		//std::list<Node *>& nodes
	}
};


#endif //GRAMMAR_H

