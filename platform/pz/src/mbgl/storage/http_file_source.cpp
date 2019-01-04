#include <mbgl/storage/http_file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {

	class HTTPFileSource::Impl {
	public:
	};

	/////

	class HTTPRequest : public AsyncRequest {
	public:
		HTTPRequest(HTTPFileSource::Impl*, Resource, FileSource::Callback) {}
		~HTTPRequest() override {}
	};

	/////

	HTTPFileSource::HTTPFileSource()
		: impl(std::make_unique<Impl>()) {
	}

	HTTPFileSource::~HTTPFileSource() = default;

	std::unique_ptr<AsyncRequest> HTTPFileSource::request(const Resource& resource, Callback callback) {
		return std::make_unique<HTTPRequest>(impl.get(), resource, callback);
	}

}