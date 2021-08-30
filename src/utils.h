#ifndef UTILS_H
#define UTILS_H

#include "type.h"

#include <apt-pkg/configuration.h>
#include <apt-pkg/pkgsystem.h>
#include <sstream>

#include <gtkmm/builder.h>

#if __cplusplus > 201703L
#include <source_location>
#else
#include <cstring>
#endif

namespace utils
{
/**
 * @brief InitPkgConfiguration
 * Initializes the apt-pkg _config object.
 * @return Result of initialization.
 */
bool InitPkgConfiguration();
/**
 * @brief PrintPkgError
 * Prints the last error.
 */
void PrintPkgError();
/**
 * @brief GetPkgConfig
 * @return The configuration object.
 */
Configuration& GetPkgConfig();
/**
 * @brief GetLog
 * @return The main log object.
 */
std::ostream& GetLog();
/**
 * @brief SetLog
 * Sets the main log object.
 * @param out The pointer to the log
 */
void SetLog(std::ostream* out);
/**
 * @brief GetErrorLog
 * @return The error log object.
 */
std::ostream& GetErrorLog();
/**
 * @brief SetErrorLog
 * Sets the error log object.
 * @param out The pointer to the log
 */
void SetErrorLog(std::ostream* out);
/**
 * @brief GetNowStr
 * @param format The format
 * @param addMs Milliseconds in the result string
 * @return The current time as a string.
 */
std::string GetNowStr(std::string format = "", bool addMs = false);
/**
 * @brief GetPkgSystem
 * @return The package system (pkgSystem) object.
 */
pkgSystem& GetPkgSystem();

namespace debug
{
#if defined(NDEBUG) // release build
class DebugNull
{
	template<typename T>
	DebugNull& operator<<(const T& val)
	{
		(void)val;
		return *this;
	}
};
#define DEBUG()
#else // debug build

class Debug
{
  public:
#if __cplusplus > 201703L
    explicit Debug(std::source_location loc = std::source_location::current());
#else
    explicit Debug(int line = -1, std::string file = "", std::string funcname = "");
#endif
    ~Debug();

	template<typename T>
	Debug& operator<<(const T& val)
	{
		_buf << val;
		return *this;
	}

  private:
    std::ostringstream _buf;
};

// gcc, clang, msvc
#if defined(__GNUC__) || defined(__clang__)
#define __CURRENT_FUNCTION_NAME__ __PRETTY_FUNCTION__
#else
#define __CURRENT_FUNCTION_NAME__ "unknown"
#endif

#if __cplusplus > 201703L
#define DEBUG() (debug::Debug())
#else
#define DEBUG()                                                                          \
	(utils::debug::Debug(__LINE__,                                                       \
						 std::strrchr(__FILE__, '/') ? std::strrchr(__FILE__, '/') + 1   \
													 : __FILE__,                         \
						 __CURRENT_FUNCTION_NAME__))
#endif
#endif
} // namespace debug

#define WRAP_EXCPT_MSG(EXCTYPE, CODE, MSG)                                               \
	try {                                                                                \
		CODE                                                                             \
	} catch(const EXCTYPE& e) {                                                          \
		utils::GetLog() << "Handled exception (" << #EXCTYPE << "): " << e.what() << "." \
						<< MSG << std::endl;                                             \
	}

#define WRAP_EXCPT(EXCTYPE, CODE) WRAP_EXCPT_MSG(EXCTYPE, CODE, "")

/**
 * @brief GetBuilderUI
 * @return The GTK.builder object.
 */
ObjPtr<Gtk::Builder> GetBuilderUI();
/**
 * @brief GetWidgetDerived
 * Initialize a widget from the builder by type.
 * @tparam Widget class
 * @param widgetID Widget ID
 * @return The widget object.
 */
template<typename TYPE>
TYPE* GetWidgetDerived(const std::string& widgetID)
{
	TYPE* t = nullptr;
	WRAP_EXCPT_MSG(
		std::exception,
		{ GetBuilderUI()->get_widget_derived(widgetID, t); },
		std::string("\nCheck the '") + UI_FILENAME + std::string("' file."));
	return t;
}

namespace widget
{
/**
 * @brief EnableWidgets
 * Enables or disables widgets, found by name.
 * @param enable Enable widget
 * @param args Widget names
 */
template<typename... Args>
typename std::enable_if_t<std::conjunction_v<std::is_constructible<std::string, Args>...>,
                          void>
EnableWidgets(bool enable, const Args&... args)
{
	for(const std::string& name : {args...}) {
		Gtk::Widget* w = nullptr;
		GetBuilderUI()->get_widget(name, w);

		if(w) {
			w->set_sensitive(enable);
		}
	}
}
} // namespace widget
} // namespace utils

#endif // UTILS_H
