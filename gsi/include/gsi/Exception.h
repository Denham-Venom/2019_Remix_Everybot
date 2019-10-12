/*******************************************************************************
 *
 * File: Exception.h
 * 	Generic System Interface class for throwing Error messages and data
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <sstream>
#include <string>

#include <exception>
#include <stdexcept>

namespace gsi
{

/*******************************************************************************
 *
 * This class provides a platform independent exception that contains more
 * information than a standard exception. When thrown, one of these exceptions
 * will include an error message, an error code, the source file and line number,
 * and a stack trace to the location of the throw.
 *
 * Note: on some platforms the availability of the stack trace will depend
 * on compile time options and libraries.
 *
 ******************************************************************************/
class Exception : public std::runtime_error
{
	public:
		Exception(const std::string msg, int32_t code,
			const std::string file = "", int32_t line = 0) throw();

		Exception(const std::string msg, int32_t code, Exception &cause,
			const std::string file = "", int32_t line = 0) throw();

		virtual ~Exception(void) throw();

		std::string getMessage();
		int32_t	getCode();
		std::string getFile();
		int32_t	getLine();
		std::string getStack();

		std::string toString(void);

	private:
		std::string exception_message;
		int32_t	exception_error_code;
		std::string exception_file;
		int32_t	exception_line;
		std::string exception_stack;

		void buildStackTrace(void);
		void addToStack(std::stringstream &ss, char *module,	char *addr,
			char *offset, char *name);
};

} // namespace gsi

std::ostream& operator<<(std::ostream & ost, gsi::Exception& lh);
