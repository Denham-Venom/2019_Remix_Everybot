/*******************************************************************************
 *
 * File: Exception.cpp
 * 	Generic System Interface class for throwing Error messages and data
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsi/Exception.h"

#if defined(__APPLE__) || defined (LINUX)
#include <execinfo.h>
#include <cxxabi.h>

#elif defined (_WINDOWS)
#include "windows.h"
#include <dbghelp.h>
// must link with DbgHelp.Lib

#endif

#include <string.h>

namespace gsi
{

/*******************************************************************************
 *
 * Create an instance of an Exception to be thrown.
 *
 * @param	msg		the text to be included in the exception
 *
 * @param	code	a user based code that could add meaning to the
 * 					exception, if the exception is the result of a failed
 * 					system call it's a good idea to pass this errno
 *
 * @param	file	defaults to an empty string if not specified, but should
 * 					always be passed the macro __FILE__
 *
 * @param	line	defaults to zero if not specified but should always
 * 					be passed the macro __LINE__
 *
 ******************************************************************************/
Exception::Exception(const std::string msg, int32_t code,
    const std::string file, int32_t line) throw() : runtime_error(msg)
{
	exception_message = msg;
	exception_error_code = code;
	exception_file = file;
	exception_line = line;

	buildStackTrace();
}

/*******************************************************************************
 *
 * Create an instance of an Exception to be thrown because another Exception
 * was caught, the message should add information to the original Exception
 *
 * @param	msg		the text to be included in the exception
 *
 * @param	code	a user based code that could add meaning to the
 * 					exception, if the exception is the result of a failed
 * 					system call it's a good idea to pass this errno
 *
 * @param	cause	the exception that caused this Exception to be thrown,
 * 					this allows for tracing to the root, not just the location
 * 					at which this Exception is thrown.
 *
 * @param	file	defaults to an empty string if not specified, but should
 * 					always be passed the macro __FILE__
 *
 * @param	line	defaults to zero if not specified but should always
 * 					be passed the macro __LINE__
 *
 ******************************************************************************/
Exception::Exception(const std::string msg, int32_t code, Exception &cause,
    const std::string file, int32_t line) throw() : runtime_error(msg)
{
	exception_message = msg;
	exception_error_code = code;
	exception_file = file;
	exception_line = line;

	exception_stack = cause.getStack();
}

/*******************************************************************************
 *
 * Release an resources allocated for this Exception
 *
 ******************************************************************************/
Exception::~Exception(void) throw()
{
}

/*******************************************************************************
 *
 * @return the message provided when this Exception was thrown
 *
 ******************************************************************************/
std::string Exception::getMessage(void)
{
	return exception_message;
}

/*******************************************************************************
 *
 * @return the code that was provided when this message was thrown
 *
 ******************************************************************************/
int32_t Exception::getCode(void)
{
	return exception_error_code;
}

/*******************************************************************************
 *
 * @return the filename that was provided when this Exception was thrown
 *
 ******************************************************************************/
std::string Exception::getFile(void)
{
	return exception_file;
}

/*******************************************************************************
 *
 * @return the line number that was provided when this Exception was thrown
 *
 ******************************************************************************/
int32_t Exception::getLine(void)
{
	return exception_line;
}

/*******************************************************************************
 *
 * @return 	the stack trace to the location that caused this exception to be
 * 			thrown. Note that the availability of this may be dependent on
 * 			the operating system and compile time options
 *
 ******************************************************************************/
std::string Exception::getStack(void)
{
	return exception_stack;
}

/*******************************************************************************
 *
 * @return 	a string representation that includes the message, code, filename,
 * 			and line number, this string does not include the stack trace so
 * 			it can be appropriate to display to the user but may provide a
 * 			little more information than most users need.
 *
 ******************************************************************************/
std::string Exception::toString(void)
{
	std::stringstream ss;
	ss << exception_message << " (" << exception_error_code << ")" ;

	if (exception_file.length() > 1)
	{
		ss << "\t" << exception_file << ":" << exception_line;
	}

	return ss.str();
}

/*******************************************************************************
 *
 * This method makes a best effort to build the stack trace at the time the
 * Exception is created. The available information is dependent on the
 * operating system, compiler, linked libraries, and compile time options so
 * the results are not as reliable or consistent as one might hope for.
 *
 * @TODO: 	Need more information about compiler options to get stack trace
 * 			documented here or someplace that this references
 *
 ******************************************************************************/
#if defined(LINUX) // // message format -->  module(name+offset) [address]
void Exception::buildStackTrace(void)
{
	std::stringstream ss;
	void *array[50];
	size_t size;
	char **messages = NULL;

	// get void*'s for all entries on the stack
	size = backtrace(array, 10);
	if (size > 2)
	{
		messages = backtrace_symbols(array, size);
	}

	if (messages != NULL)
	{
		// 0 will be this function
		// 1 will be the Exception constructor
		for (size_t i = 2; i < size; ++i)
		{
			char *addr = NULL;
			char *name = NULL;
			char *offset = NULL;
			char *module = NULL;
			char *p = messages[i];

			while (*p == ' ') p++; // any space

			module = p;
			while (*p != ' ' && *p != '(' && *p != 0) p++; // module name
			while (*p == ' ' || *p == '(') {*p = '\0'; p++;}  // any space

			name = p;
			while (*p != ' ' && *p != '+' && *p != 0) p++; // name
			while (*p == ' ' || *p == '+') {*p = '\0'; p++;}  // any space

			offset = p;
			while (*p != ' ' && *p != ')' && *p != 0) p++; // offset
			while (*p == ' ' || *p == ')' || *p == '[') {*p = '\0'; p++;}  // any space

			addr = p;
			while (*p != ' ' && *p != ']' && *p != 0) p++; // offset
			*p = 0;

			addToStack(ss, module, addr, offset, name);
		}

		free (messages);
	}
	else
	{
		ss << "empty stack message";
	}

	exception_stack = ss.str();
}

#elif defined(__APPLE__) || defined(LINUX) // message format -->  idx  module  address  name  +  offset
void Exception::buildStackTrace(void)
{
	std::stringstream ss;
	void *array[50];
	size_t size;
	char **messages = NULL;

	// get void*'s for all entries on the stack
	size = backtrace(array, 10);
	if (size > 2)
	{
		messages = backtrace_symbols(array, size);
	}

	if (messages != NULL)
	{
		// 0 will be this function
		// 1 will be the Exception constructor
		// 2 for some reason is also the Exception constructor, but I don't know if it will always be
		for (size_t i = 2; i < size; ++i)
		{
			char *addr = NULL;
			char *name = NULL;
			char *offset = NULL;
			char *module = NULL;
			char *p = messages[i];


			while (*p == ' ') p++; // any space
			while (*p != ' ' && *p != 0) p++; // idx
			while (*p == ' ') p++; // any space

			module = p;
			while (*p != ' ' && *p != 0) p++; // module name
			while (*p == ' ') {*p = '\0'; p++;}  // any space

			addr = p;
			while (*p != ' ' && *p != 0) p++; // address
			while (*p == ' ') {*p = '\0'; p++;} // any space

			name = p;
			while (*p != ' ' && *p != 0) p++; // name
			while (*p == ' ' || *p == '+') {*p = '\0'; p++;} // any space

			offset = p;

			addToStack(ss, module, addr, offset, name);
		}

		free (messages);
	}
	else
	{
		ss << "empty stack message";
	}

	exception_stack = ss.str();
}

#elif defined (_WINDOWS)
void Exception::buildStackTrace(void)
{
	std::stringstream ss;
    void         * stack[ 50 ];
    unsigned short size;
    SYMBOL_INFO  * symbol;
    HANDLE         process;
	char addr[20];

    process = GetCurrentProcess();

    SymInitialize( process, NULL, TRUE );

    size = CaptureStackBackTrace( 0, 50, stack, NULL );
	if (size > 2)
	{
	    symbol = ( SYMBOL_INFO * )calloc( sizeof( SYMBOL_INFO ) + 256 * sizeof( char ), 1 );
	    symbol->MaxNameLen = 255;
	    symbol->SizeOfStruct = sizeof( SYMBOL_INFO );
	}

	if (symbol != NULL)
	{
		// 0 will be this function
		// 1 will be the Exception constructor
		for(unsigned short i = 2; i < size; i++ )
		{
			SymFromAddr( process, ( DWORD64 )( stack[ i ] ), 0, symbol );

            sprintf(addr, "%016lluX", (uint64_t)(symbol->Address));
			addToStack(ss, NULL, addr, NULL, symbol->Name);
		}

		free( symbol );
	}
	else
	{
		ss << "empty stack message";
	}

	exception_stack = ss.str();
}

#else
void Exception::buildStackTrace(void)
{
	exception_stack = "unable to build stack trace for unknown platform";
}

#endif

/*******************************************************************************
 *
 *	This is a helper method used to build the stack trace in a somewhat
 *	consistent mannor
 *
 ******************************************************************************/
void Exception::addToStack(std::stringstream &ss, char *module,	char *addr,
	char *offset, char *name)
{
	ss << '\t';

	if (module != NULL)
	{
		ss << module << "\t";
	}
	else
	{
		ss << "---\t";
	}

	if (addr != NULL)
	{
		ss << addr << "\t";
	}
	else
	{
		ss << "---\t";
	}

	if (offset != NULL)
	{
		ss << offset << "\t";
	}
	else
	{
		ss << "---\t";
	}

	if (name != NULL)
	{
#if defined(__APPLE__) || defined (LINUX)
		int status = -1;
		char * dname = abi::__cxa_demangle(name, 0, 0, &status);

		if (status == 0)
		{
			ss << dname << "\t";
			free(dname);
		}
		else
		{
			ss << name << "\t";
		}
#else
		ss << name << "\t";
#endif
	}
	else
	{
		ss << "?\t";
	}

	ss << std::endl;
}

} // namespace gsi

/*******************************************************************************
 *
 * This allows for the use of c++ stream based output, it just passes the
 * results of lh.toString() to the provided stream
 *
 * @param	the stream that should be written to
 * @param	the Exception that should be written to the stream
 *
 ******************************************************************************/
std::ostream& operator<<(std::ostream &ost, gsi::Exception &lh)
{
	ost << lh.toString();
	return ost;
}
