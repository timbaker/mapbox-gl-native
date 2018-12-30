#include <mbgl/storage/file_source_request.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/io.hpp>

#include <sys/types.h>
#include <sys/stat.h>

#if defined(_WINDOWS) && !defined(S_ISDIR)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

#ifdef _WINDOWS // tnb
// https://stackoverflow.com/questions/30829364/open-utf8-encoded-filename-in-c-windows
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <Windows.h>
static std::wstring toWString(const std::string& str)
{
	std::wstring ret;
	int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0);
	if (len > 0) {
		ret.resize(len);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &ret[0], len);
	}
	return ret;
}
#endif

namespace mbgl {

void requestLocalFile(const std::string& path, ActorRef<FileSourceRequest> req) {
    Response response;
#ifdef _WINDOWS // tnb
	struct _stat buf;
	std::wstring wpath = toWString(path);
	int result = _wstat(wpath.c_str(), &buf);
#else
    struct stat buf;
    int result = stat(path.c_str(), &buf);
#endif

    if (result == 0 && (S_IFDIR & buf.st_mode)) {
        response.error = std::make_unique<Response::Error>(Response::Error::Reason::NotFound);
    } else if (result == -1 && errno == ENOENT) {
        response.error = std::make_unique<Response::Error>(Response::Error::Reason::NotFound);
    } else {
        auto data = util::readFile(path);
        if (!data) {
            response.error = std::make_unique<Response::Error>(
                Response::Error::Reason::Other,
                std::string("Cannot read file ") + path);
        } else {
            response.data = std::make_shared<std::string>(std::move(*data));
        }
    }

    req.invoke(&FileSourceRequest::setResponse, response);
}

} // namespace mbgl
