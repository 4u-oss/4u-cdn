#include <App.h> /* Main uWebSockets file */
#include <Magick++.h>
#include <aws/core/Aws.h>
#include <aws/core/utils/HashingUtils.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/S3Client.h>
#include <iostream>
#include <filesystem>

namespace CDN {
  class Utils {
    public:
    static int stringToInt(const std::string &str) {
      int val;

      try {
        val = std::stoi(str);
      } catch (const std::invalid_argument& e) {
        if (str.length() > 0) {
          throw e;
        } else {
          val = 0;
        };
      };

      return val;
    };
  };
};

int main() {
  uWS::SSLApp({
    .key_file_name = "../key.pem",
    .cert_file_name = "../certificate.pem",
  }).get("/*", [](uWS::HttpResponse<true> *res, uWS::HttpRequest *req) {
    const std::string host = std::string{req->getHeader("host")};
    
    int width;
    int height;
    int quality;
    std::string format;
    try {
      width = CDN::Utils::stringToInt(std::string{req->getQuery("w")});
      height = CDN::Utils::stringToInt(std::string{req->getQuery("h")});
      quality = CDN::Utils::stringToInt(std::string{req->getQuery("q")});
      format = std::string{req->getQuery("format")};
    } catch (const std::invalid_argument& e) {
      res->writeHeader("Content-Type", "application/json")->writeStatus("400 Bad Request")->end("Invalid Parameters");
    };

    Aws::SDKOptions options;
    Aws::InitAPI(options);

    Aws::Client::ClientConfiguration config;
    config.region = "us-east-1";
    Aws::S3::S3Client s3Client(config);

    Aws::S3::Model::GetObjectRequest s3Req;
    s3Req.SetBucket("4u-cdn");
    s3Req.SetKey("/" + host.substr(0, host.find_first_of(".")) + std::string{req->getUrl()});

    Aws::S3::Model::GetObjectOutcome s3Outcome = s3Client.GetObject(s3Req);

    if (s3Outcome.IsSuccess()) {
      const auto &result = s3Outcome.GetResult();
      const std::string mime = result.GetContentType().c_str();
      Aws::IOStream &body = result.GetBody();

      res->writeHeader("Content-Type", mime);

      if (width > 0 || height > 0 || quality > 0 || format.length() > 0) {
        // Convert body to a vector of unsigned chars
        std::istreambuf_iterator<char> begin(body), end;
        std::vector<unsigned char> bodyData(begin, end);

        // Convert vector to ImageMagick blob, and then ImageMagick image
        Magick::Blob blob(bodyData.data(), bodyData.size());

        Magick::Image img;
        img.read(blob);

        // Resize image
        const Magick::Geometry &imgSize = img.size();
        Magick::Geometry geometry;
        if (width > 0 || height > 0) {
          geometry = Magick::Geometry{width > 0 ? width : imgSize.width(), height > 0 ? height : imgSize.height()};
        };
        if (geometry.width() > 0 && geometry.height() > 0) {
          img.resize(geometry);
        };

        // Set quality
        if (quality > 0) {
          img.quality(quality);
        };

        // Set format
        if (format.length() > 0) {
          img.magick(format);
          res->writeHeader("Content-Type", "image/" + format);
        };
        
        // Convert ImageMagick image back to a string
        img.write(&blob);
        Aws::Utils::ByteBuffer hash = Aws::Utils::HashingUtils::Base64Decode(blob.base64());
        std::string imgStr(reinterpret_cast<const char*>(hash.GetUnderlyingData()), hash.GetLength());

        res->end(imgStr);
      } else {
        // Convert body to a string
        std::stringstream ss;
        ss << body.rdbuf();
        res->end(ss.str());
      };
    } else {
      const Aws::S3::S3Error &s3Err = s3Outcome.GetError();
      if (s3Err.GetErrorType() == Aws::S3::S3Errors::NO_SUCH_KEY) {
        res->writeHeader("Content-Type", "application/json")->writeStatus("404 Not Found")->end("Not found");
      } else {
        res->writeHeader("Content-Type", "application/json")->writeStatus("500 Internal Server Error")->end("Error");
      }
    }

    Aws::ShutdownAPI(options);
  }).listen(443, [](auto *listenSocket) {
    if (listenSocket) {
      std::cout << "CDN has started listening on port 443." << std::endl;
    } else {
      if (!std::filesystem::exists("certificate.pem") || !std::filesystem::exists("key.pem")) {
        std::cout << "CDN could not be started." << "\n" << "Make sure you have the correct certificate files (certificate.pem and key.pem)." << std::endl;
      } else {
        std::cout << "CDN could not be started." << "\n" << "An unknown error occured." << std::endl;
      };
    }
  }).run();

  return 0;
}