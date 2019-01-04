#include <mbgl/storage/mbtiles_file_source.hpp>
#include <mbgl/storage/file_source_request.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/sqlite3.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/url.hpp>

#include <unordered_map>
#include <numeric>
#include <sstream>

namespace {

const std::string fileProtocol = "mbtiles://";

} // namespace

namespace mbgl {

class MBTilesFileSource::Impl {
public:
	using Database = mapbox::sqlite::Database;

	class MBDatabase {
	public:
		MBDatabase(Database&& db)
			: db(std::move(db))
		{}

		mapbox::sqlite::Statement& getStatement(const char* sql) {
			auto it = statements.find(sql);
			if (it == statements.end()) {
				it = statements.emplace(sql, std::make_unique<mapbox::sqlite::Statement>(db, sql)).first;
			}
			return *it->second;
		}

		Database db;
		std::unordered_map<const char*, std::unique_ptr<mapbox::sqlite::Statement>> statements;
	};

	using DatabasePtr = std::unique_ptr<MBDatabase>;

	Impl(ActorRef<Impl>) {}

    void request(const std::string& url, ActorRef<FileSourceRequest> req) {
        if (!acceptsURL(url)) {
            Response response;
            response.error = std::make_unique<Response::Error>(Response::Error::Reason::Other,
                                                               "Invalid mbtiles URL");
            req.invoke(&FileSourceRequest::setResponse, response);
            return;
        }

        const auto path = mbgl::util::percentDecode(url.substr(fileProtocol.size()));
        requestTile(path, std::move(req));
    }

	void requestTile(const std::string& path, ActorRef<FileSourceRequest> req) {
		// C:/foo/bar/filename.mbtiles/z/x/y
		auto elems = split(path, '/');
		if (elems.size() < 4) {
            error("Invalid mbtiles URL", req);
			return;
		}
		int z, x, y;
		try {
			z = std::stoi(elems[elems.size() - 3]);
			x = std::stoi(elems[elems.size() - 2]);
			y = std::stoi(elems[elems.size() - 1]);
		}
		catch (const std::invalid_argument& e) {
            error("Invalid mbtiles URL", req);
			return;
		}
		std::string fileName = std::accumulate(elems.begin() + 1, elems.end() - 3, elems[0],
			[](const std::string& result, const std::string& elem) { return result + "/" + elem; });
		auto it = databases.find(fileName);
		if (it == databases.end()) {
			// TODO: sqlite opens UTF-8 fileNames
			auto result = Database::tryOpen(fileName, mapbox::sqlite::ReadOnly);
			if (result.is<mapbox::sqlite::Exception>()) {
				error("Failed to open mbtiles database", req);
				return;
			}
			DatabasePtr db = std::make_unique<MBDatabase>(std::move(result.get<Database>()));
			db->db.setBusyTimeout(Milliseconds::max());
			it = databases.emplace(fileName, std::move(db)).first;
		}
		DatabasePtr& db = it->second;
		try {
			mapbox::sqlite::Query query{ db->getStatement("SELECT tile_data FROM tiles WHERE zoom_level = ? and tile_column = ? and tile_row = ?") };
			query.bind<int>(1, z);
			query.bind<int>(2, x);
			query.bind<int>(3, (1LL << z) - 1 - y);
			if (query.run()) {
				Response response;
				response.data = std::make_shared<std::string>(query.get<std::string>(0));
				req.invoke(&FileSourceRequest::setResponse, response);
				return;
			}
			else {
				Response response;
				response.error = std::make_unique<Response::Error>(Response::Error::Reason::NotFound);
				req.invoke(&FileSourceRequest::setResponse, response);
				return;
			}
		}
		catch (const mapbox::sqlite::Exception& ex) {
			error("sqlite exception", req);
			return;
		}
	}

	void error(const std::string& msg, ActorRef<FileSourceRequest> req) {
		Response response;
		response.error = std::make_unique<Response::Error>(Response::Error::Reason::Other);
		req.invoke(&FileSourceRequest::setResponse, response);
	}

	std::vector<std::string> split(const std::string& s, char delim) {
		std::vector<std::string> elems;
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}

	std::unordered_map<std::string, DatabasePtr> databases;
};

MBTilesFileSource::MBTilesFileSource()
    : impl(std::make_unique<util::Thread<Impl>>("MBTilesFileSource")) {
}

MBTilesFileSource::~MBTilesFileSource() = default;

std::unique_ptr<AsyncRequest> MBTilesFileSource::request(const Resource& resource, Callback callback) {
    auto req = std::make_unique<FileSourceRequest>(std::move(callback));

    impl->actor().invoke(&Impl::request, resource.url, req->actor());

    return std::move(req);
}

bool MBTilesFileSource::acceptsURL(const std::string& url) {
    return 0 == url.rfind(fileProtocol, 0);
}

} // namespace mbgl
