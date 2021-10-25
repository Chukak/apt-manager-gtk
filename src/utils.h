#ifndef UTILS_H
#define UTILS_H

#include "type.h"

#include <apt-pkg/configuration.h>
#include <apt-pkg/pkgsystem.h>
#include <sstream>

#include <gtkmm/builder.h>

#define _GNUC_VER(major, minor) __GNUC__ >= major&& __GNUC_MINOR__ >= minor

#if __cplusplus > 201703L && _GNUC_VER(11, 1)
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

/**
 * @brief The LogFlag enum
 */
enum LogFlag : int
{
	LogLine = 1 << 0, //! a line
	LogFunc = 1 << 1, //! a function or method
	LogFile = 1 << 2, //! a file
	LogVerbose = 1 << 3,
	LogDefault = LogLine | LogFile | LogFunc | LogVerbose
};

/**
 * @brief SetLogFlags
 * Sets flags to log object. See: the LogFlag enum.
 * @param flags Log flags.
 */
void SetLogFlags(int flags);
/**
 * @brief RedirectLogOutputToTemporaryFile
 * Redirects stdout and stderr to the temporarily file.
 * @param enable Redirecting enable
 */
void RedirectLogOutputToTemporaryFile(bool enable);

namespace debug
{
class Debug
{
  public:
#if __cplusplus > 201703L && _GNUC_VER(11, 1)
    explicit Debug(std::source_location loc, bool verbose = false);
#else
	explicit Debug(int line = -1,
				   std::string file = "",
				   std::string funcname = "",
				   bool verbose = false);
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
    bool _verbose;
};

// gcc, clang, msvc
#if defined(__GNUC__) || defined(__clang__)
#define __CURRENT_FUNCTION_NAME__ __PRETTY_FUNCTION__
#else
#define __CURRENT_FUNCTION_NAME__ "unknown"
#endif

#if __cplusplus > 201703L && _GNUC_VER(11, 1)
#define DEBUG() (utils::debug::Debug(std::source_location::current(), true))
#define INFO() (utils::debug::Debug(std::source_location::current()))
#else
#define DEBUG()                                                                          \
	(utils::debug::Debug(__LINE__,                                                       \
						 std::strrchr(__FILE__, '/') ? std::strrchr(__FILE__, '/') + 1   \
													 : __FILE__,                         \
						 __CURRENT_FUNCTION_NAME__,                                      \
						 true))
#define INFO()                                                                           \
	(utils::debug::Debug(__LINE__,                                                       \
						 std::strrchr(__FILE__, '/') ? std::strrchr(__FILE__, '/') + 1   \
													 : __FILE__,                         \
						 __CURRENT_FUNCTION_NAME__))
#endif // __cplusplus
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
 * Get a widget from the builder by type.
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
/**
 * @brief GetCustomWidget
 * Get a custom widget. Is widget is not wxists, initialize this.
 * @tparam Widget class
 * @param widgetID Widget ID
 * @return The widget object.
 */
template<typename TYPE>
TYPE* GetCustomWidget(const std::string& widgetID)
{
	static std::map<std::string, std::unique_ptr<TYPE>> _customWidgets;

	typename decltype(_customWidgets)::iterator found = _customWidgets.find(widgetID);
	if(found != _customWidgets.end()) return found->second.get();

	_customWidgets.insert(std::make_pair(widgetID, std::make_unique<TYPE>()));
	return GetCustomWidget<TYPE>(widgetID);
}

namespace widget
{
namespace property
{
using Val = void*;
/**
 * @brief Enable
 * @return The enabled property value.
 */
Val Enable();
/**
 * @brief Disable
 * @return The disable property value.
 */
Val Disable();

/**
 * @brief SetSensitiveSkip
 * The sensitive property.
 */
static const Glib::ustring SetSensitiveSkip = "widget-property-sensitive-skip";
} // namespace property

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
			property::Val prop = w->get_data(property::SetSensitiveSkip);
			if(!prop) w->set_sensitive(enable);
		}
	}
}
} // namespace widget
} // namespace utils

#endif // UTILS_H
