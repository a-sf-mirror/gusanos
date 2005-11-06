#include "grammar.h"

#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <map>
#include <algorithm>
#include <set>
#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>
#include "omfgutil_macros.h"

using std::cout;
using std::endl;
using boost::array;
using boost::lexical_cast;

struct Handler
{
	Handler(std::istream& str_)
	: str(str_), tokenNumber(1), setNumber(1), assumeToken(-1)
	{
		Token* t = new Token("EOF");
		t->idx = 0;
		t->def = "\"\\000\"";
		tokens.push_back(t);
		next();
	}
	
	int cur()
	{
		return c;
	}
	
	void next()
	{
		c = str.get();
		if(c == std::istream::traits_type::eof())
			c = -1;
	}
	
	void addRule(Rule* r)
	{
		rules_[r->name] = r;
	}
	
	void addToken(Token* t)
	{
		t->idx = tokenNumber++;
		tokens.push_back(t);
	}
	
	Token* findToken(std::string const& name)
	{
		foreach(i, tokens)
		{
			if((*i)->name == name)
				return *i;
		}
		
		return 0;
	}

	bool firstSet(Element& self, std::set<int>& s)
	{
		if(self.sub)
			firstSet(*self.sub, s);
		else if(!self.child.empty())
		{
			let_(i, rules_.find(self.child));
			if(i != rules_.end())
				firstSet(*i->second, s);
			else
			{
				let_(j, findToken(self.child));
				if(j)
					s.insert(j->idx);
				else
					throw SyntaxError("Identifier not declared");
			}
		}
		else
			return false;
	}
	
	void firstSet(Option& self, std::set<int>& s)
	{
		foreach(i, self.elements)
		{
			if(firstSet(**i, s))
				return;
		}
	}
	
	void firstSet(Def& self, std::set<int>& s)
	{
		foreach(i, self.options)
		{
			firstSet(**i, s);
		}
	}
	
	void firstSet(Rule& self, std::set<int>& s)
	{
		firstSet(*self.def, s);
	}
	
	std::string firstSetTest(Element& self)
	{
		if(self.sub)
			return firstSetTest(*self.sub);
		else if(!self.child.empty())
		{
			let_(i, rules_.find(self.child));
			if(i != rules_.end())
				return firstSetTest(*i->second);
			else
			{
				let_(j, findToken(self.child));
				if(j)
				{
					return "(cur == " + lexical_cast<std::string>(j->idx) + ")";
				}
				else
					throw SyntaxError("Identifier not declared");
			}
		}
		else
			return "";
	}
	
	std::string firstSetTest(Option& self)
	{
		foreach(i, self.elements)
		{
			std::string s = firstSetTest(**i); 
			if(!s.empty())
				return s;
		}
		
		return "";
	}
	
	std::string firstSetTest(Def& self)
	{
		if(self.options.size() == 1)
		{
			return firstSetTest(**self.options.begin());
		}
		
		if(!self.firstSet.empty())
			return self.firstSet;
		
		std::string setTest = "set_" + lexical_cast<std::string>(setNumber++);
		self.firstSet = setTest + "[cur]";
		
		firstSet(self, sets[setTest]);
		
		return self.firstSet;
	}
		
	std::string firstSetTest(Rule& self)
	{
		return firstSetTest(*self.def);
	}
	
	void output(std::ostream& s, Element& self)
	{
		if(self.optional)
			s << "if(" << firstSetTest(self) << ") {\n";
			
		switch(self.mult)
		{
			case Element::Repeat0: s << "while(" << firstSetTest(self) << ") {\n"; break;
			case Element::Repeat1: s << "do {\n"; break;
			case Element::Once: break;
		}
				
		if(self.sub)
			output(s, *self.sub);
		
		else if(!self.child.empty())
		{
			let_(i, rules_.find(self.child));
			if(i != rules_.end())
				s << "rule_" << i->first << "(" << self.code << ");\n";
			else
			{
				let_(j, findToken(self.child));
				if(j)
				{
					Token& token = *j;
					s << "if(cur != " << token.idx << ") syntaxError();\n";
					if(!self.code.empty())
					{
						if(!token.code.empty())
							//s << self.code << ".reset(static_cast<" << token.name << " *>(curData.release());\n";
							s << "auto_ptr<" << token.name << "> " << self.code << "(static_cast<" << token.name << " *>(curData.release()));\n";
						else
							throw SyntaxError("Token does not return any data");
					}
					s << "next();\n";
				}
				else
					throw SyntaxError("Identifier not declared");
			}
		}
		else
		{
			s << self.code << endl;
		}
		
		switch(self.mult)
		{
			case Element::Repeat0: s << "}\n"; break;
			case Element::Repeat1: s << "} while(" << firstSetTest(self) << ");\n"; break;
			case Element::Once: break;
		}
		
		if(self.optional)
			s << "}" << endl;
	}
	
	void output(std::ostream& s, Option& self)
	{
		foreach(i, self.elements)
		{
			output(s, **i);
		}
	}
	
	void output(std::ostream& s, Def& self)
	{
		
		if(self.options.size() > 1)
		{
			bool first = true;
			
			foreach(i, self.options)
			{
				if(first)
					first = false;
				else
					s << "else ";
				s << "if(" << firstSetTest(**i) << ") {\n";
				output(s, **i);
				s << "}\n";
			}
			
			s << "else syntaxError();\n";
		}
		else
		{
			output(s, **self.options.begin());
		}
	}
	
	void output(std::ostream& s, Rule& self)
	{
		s << "void rule_" << self.name << "(" << self.args << ") {\n";
		output(s, *self.def);
		s << "}\n";
	}
	
	void output(std::ostream& s, Token& self)
	{
		if(!self.code.empty())
		{
			s
			<< "struct " << self.name << " : public Token {\n"
			<< "typedef std::auto_ptr<" << self.name << "> ptr;\n"
			<< "#define CONSTRUCT(b_, e_) " << self.name << "(char const* b_, char const* e_)\n"
			<< self.code
			<<
			"\n"
			"#undef CONSTRUCT\n"
			"};\n";
		}
	}
	
	void output(std::ostream& s, std::string name)
	{
		s <<
		"#include <memory>\n"
		"#include <cstddef>\n"
		"#include <cstdlib>\n"
		"#include <cstring>\n";
		
		s << prolog << '\n' <<
		"template<class BaseT>\n"
		"struct " << name << " : public BaseT {\n"
		"~" << name << "() { free(buffer); }\n"
		"struct Token {};\n\n"
		;
		
		foreach(i, tokens)
		{
			Token& token = **i;

			output(s, token);
		}
		
		s <<
		"void next() {\n"
		"#define YYCTYPE char\n"
		"#define YYCURSOR curp\n"
		"#define YYLIMIT limit\n"
		"#define YYMARKER marker\n"
		"#define YYFILL(n) fill(n)\n"
		"retry:\n"
		"begin = curp;\n"
		"/*!re2c\n";
		
		foreach(i, tokens)
		{
			Token& token = **i;
			
			s << token.def;
			if(token.name != "SKIP")
			{
				s << " { cur = " << token.idx << "; ";
				if(!token.code.empty())
					s << "curData.reset(new " << token.name << "(begin, YYCURSOR));";
				/*
				else
					s << "curData.reset();";*/
					
				//s << " cout << \"Token: " << token.idx << "\\n\";";
				s << " return; }\n";
			}
			else
				s << "{ goto retry; }\n";
		}
		
		s <<
		"[\\000-\\377] { goto retry; }\n"
		"*/\n"
		"#undef YYCTYPE\n"
		"#undef YYCURSOR\n"
		"#undef YYLIMIT\n"
		"#undef YYMARKER\n"
		"#undef YYFILL\n"
		"}\n";
		
		s <<
		"void fill(size_t s) {\n"
		"size_t l = limit - begin;\n"
		"if(buffer)\n"
		"	memmove(buffer, begin, l);\n" // Move the beginning of the token to the beginning of the buffer
		"size_t newSize = std::max(static_cast<size_t>(1024), l + s);\n"
		"buffer = (char *)realloc(buffer, newSize);\n"
		"size_t toRead = newSize - l;\n"
		"size_t amountRead = this->read(&buffer[l], toRead);"
		"if(amountRead < toRead) { buffer[l+amountRead] = '\\0'; ++amountRead; }" // Add a null char at the end and increase read size
		"newSize = l + amountRead;\n"
		"ptrdiff_t offs = buffer - begin;\n"
		"curp += offs;\n"
		"marker += offs;\n"
		"begin = buffer;\n"
		"limit = buffer + newSize;\n}\n";
		s <<
		"struct SyntaxError : public std::exception {\n"
		"SyntaxError(char const* msg_) : msg(msg_) {}\n"
		"virtual char const* what() const throw() { return msg; }\n"
		"char const* msg;};\n"
		"void syntaxError(char const* msg = \"Syntax error\") {\n"
		"throw SyntaxError(msg);\n}\n"
		"bool full() { return cur == 0; }\n";

		foreach(i, rules_)
		{
			Rule& rule = *i->second;

			output(s, rule);
		}
		
		foreach(i, sets)
		{
			s << "bool " << i->first << "[" << tokenNumber << "];\n";
		}
		
		s <<
		name << "(BaseT const& base) : BaseT(base), cur(-1), begin(0), buffer(0), curp(0), limit(0) {\n";
		
		foreach(i, sets)
		{
			s << "memset(" << i->first << ", 0, sizeof(bool)*" << tokenNumber << ");\n";
			foreach(j, i->second)
			{
				s << i->first << "[" << *j << "] = true;\n";
			}
		}
		
		s << "next();}\n";
		
		s <<
		"int cur;\n"
		"std::auto_ptr<Token> curData;\n"
		"char* curp;\n"
		"char* limit;\n"
		"char* marker;\n"
		"char* begin;\n"
		"char* buffer;\n"
		"};\n";
	}
	
	int c;
	std::istream& str;
	std::map<std::string, Rule *> rules_;
	std::list<Token *> tokens;
	std::map<std::string, std::set<int> > sets;
	int setNumber;
	int tokenNumber;
	std::string prolog;
	int assumeToken;
	//int maxToken;
};

//#define PARSER

#ifdef PARSER
#include "out2.h"

struct TestParser
{
	TestParser(std::istream& str_)
	: str(str_)
	{
	}
	
	size_t read(char* p, size_t s)
	{
		str.read(p, s);
		return str.gcount();
	}
	
	std::istream& str;
};
#endif

int main(int argc, char const* argv[])
{
#ifndef PARSER
	if(argc < 3)
	{
		cout << "Usage: parsergen <source> <target>\n";
		return 0;
	}
	
	std::ifstream i(argv[1]);
		
	Grammar<Handler> handler((Handler(i)));

	handler.rules();
	
	std::ofstream o(argv[2]);
	handler.output(o, "TGrammar");
#else
	std::stringstream ss("a = 1\nb = 2\n");
	TGrammar<TestParser> parser((TestParser(ss)));
	parser.rule_lines();
#endif
}
