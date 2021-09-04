#include "utils.h"

#include <apt-pkg/init.h>
#include <apt-pkg/error.h>
#include <chrono>
#include <iomanip>

namespace utils
{
static std::ostream* LogOutput(&std::cout);
static std::ostream* ErrorLogOutput(&std::cerr);

bool InitPkgConfiguration()
{
	static bool confIsInitialized(false);
	if(!confIsInitialized) {
		if(!pkgInitConfig(*_config)) {
			PrintPkgError();
			return false;
		}

		if(!pkgInitSystem(*_config, _system)) {
			PrintPkgError();
			return false;
		}
	}
	confIsInitialized = true;
	return true;
}

void PrintPkgError()
{
	if(_error->PendingError()) {
		_error->DumpErrors(*ErrorLogOutput);
	}
}

Configuration& GetPkgConfig()
{
	return *_config;
}

std::ostream& GetLog()
{
	return *LogOutput;
}

void SetLog(std::ostream* out)
{
	LogOutput = out;
}

std::ostream& GetErrorLog()
{
	return *ErrorLogOutput;
}

void SetErrorLog(std::ostream* out)
{
	ErrorLogOutput = out;
}

std::string GetNowStr(std::string format, bool addMs)
{
	std::stringstream result;

	if(format.empty()) {
		format = "%H:%M:%S";
		addMs = true;
	}

	std::chrono::time_point<std::chrono::system_clock> now =
		std::chrono::system_clock::now();
	std::time_t tm = std::chrono::system_clock::to_time_t(now);

	result << std::put_time(std::localtime(&tm), format.c_str());

	if(addMs) {
		result << "."
			   << (std::chrono::duration_cast<std::chrono::milliseconds>(
					   now.time_since_epoch()) %
				   1000)
					  .count();
	}

	return result.str();
}

pkgSystem& GetPkgSystem()
{
	return *_system;
}

static bool LogIncludeLine = true, LogIncludeFunc = true, LogIncludeFile = true;

void SetLogFlags(int flags)
{
	LogIncludeFile = LogIncludeLine = LogIncludeFunc = false;

	if(flags & LogLine) {
		LogIncludeLine = true;
	}

	if(flags & LogFunc) {
		LogIncludeFunc = true;
	}

	if(flags & LogFile) {
		LogIncludeFile = true;
	}
}

namespace debug
{
#if __cplusplus > 201703L
Debug::Debug(std::source_location loc)
{
	_buf << GetNowStr() << ":" << file << " -> " << loc.function_name() << ":"
		 << loc.line() << ": ";
}
#else
Debug::Debug(int line, std::string file, std::string funcname)
{
	_buf << GetNowStr() << ": " << (LogIncludeFile ? file : "")
		 << (LogIncludeFunc ? " -> [" + funcname + "]" : "")
		 << (LogIncludeLine ? ":" + std::to_string(line) : "") << ": ";
}
#endif

Debug::~Debug()
{
	*LogOutput << _buf.str() << std::endl;
}
} // namespace debug

static ObjPtr<Gtk::Builder> BuilderUI;

ObjPtr<Gtk::Builder> GetBuilderUI()
{
	if(!(BuilderUI.get()))
		WRAP_EXCPT(std::exception,
				   { BuilderUI = Gtk::Builder::create_from_resource(UI_FILENAME); });

	return BuilderUI;
}
} // namespace utils
