#ifndef OMFGCONSOLE_CONSOLEGRAMMAR_H
#define OMFGCONSOLE_CONSOLEGRAMMAR_H

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



template<class BaseT>
struct ConsoleGrammar : public BaseT
{
	ConsoleGrammar(BaseT const& base)
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
		
	bool firstSkip()
	{
		return cur() == ' ' || cur() == '\n' || cur() == '\r' || cur() == '\t';
	}
	
	bool firstIdent()
	{
		return cur() != '}' && cur() != ';' && cur() != '"' && cur() != ' ' && cur() != -1;
	}
	
	bool firstArgument()
	{
		return cur() == '"' || firstIdent();
	}
	
	bool firstCharacter()
	{
		return cur() != '"' && cur() != -1;
	}

	std::string ident()
	{
		if(firstIdent())
		{
			std::string i;
			for(; firstIdent(); nextInLexeme())
			{
				switch(cur())
				{
					case '{':
					{
						next();
						std::string ret = command();
						if(cur() != '}')
							throw SyntaxError("Expected '}'");

						i += ret;
					}
					break;
					
					default:
						i += cur();
				}
			}
			doneLexeme();
			return i;
		}
		else
			throw SyntaxError("Expected identifier");
	}
	
	std::string character()
	{
		switch(cur())
		{
			case '\\':
			{
				nextInLexeme();
				return std::string(1, (char)escape());
			}
			break;
			
			case '{':
			{
				next();
				std::string ret = command();
				if(cur() != '}')
					throw SyntaxError("Expected '}'");
				nextInLexeme();
				
				return ret;
			}
			break;
			
			default:
			{
				return std::string(1, (char)nextInLexeme());
			}
		}
	}
	
	char escape()
	{
		switch(cur())
		{
			case '\\':
			case '{':
			case '}':
			case '"':
			{
				return (char)nextInLexeme();
			}
			break;
			
			default:
			{
				throw SyntaxError("Invalid escape sequence");
			}
		}
	}
	
	void argument(std::list<std::string>& arguments)
	{
		/*if(cur() == '{') //Handled by ident() now
		{
			next();
			std::string ret = command();
			if(cur() != '}')
				throw SyntaxError("Expected '}'");
			next();

			arguments.push_back(ret);
		}
		else*/
		if(cur() == '"')
		{
			nextInLexeme();
			std::string arg;
			while(firstCharacter())
				arg += character();
			if(cur() != '"')
				throw SyntaxError("Expected '\"'");
			next();
				
			arguments.push_back(arg);
		}
		else if(firstIdent())
		{
			arguments.push_back(ident());
		}
	}
	
	std::string command()
	{
		std::string name = ident();
		std::list<std::string> arguments;
		while(firstArgument())
			argument(arguments);
		return BaseT::invoke(name, arguments);
	}
	
	std::string block()
	{
		
		std::string ret;
		if(firstIdent())
		{
			std::string r = command();

			if(!r.empty())
				ret += '<' + r + '>';
		}
		while(cur() == ';')
		{
			next();
			if(firstIdent())
			{
				std::string r = command();
				if(!r.empty())
					ret += '<' + r + '>';
			}
		}
		
		if(cur() != -1)
			throw SyntaxError("Expected EOF");
			
		return ret;
	}
};


#endif //OMFGCONSOLE_CONSOLEGRAMMAR_H
