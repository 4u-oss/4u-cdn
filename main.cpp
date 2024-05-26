#include "App.h" /* Main uWebSockets file */
#include <Magick++.h>
#include <aws/core/Aws.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/S3Client.h>
#include <iostream>

int main() {
  uWS::SSLApp({
    .key_file_name = "key.pem",
    .cert_file_name = "certificate.pem",
  }).get("/*", [](uWS::HttpResponse<true> *res, uWS::HttpRequest *req) {
    const std::string host = std::string{req->getHeader("host")};
    Aws::SDKOptions options;
    Aws::InitAPI(options);

    Aws::Client::ClientConfiguration config;
    config.region = "us-east-1";
    Aws::S3::S3Client s3Client(config);

    Aws::S3::Model::GetObjectRequest s3Req;
    s3Req.SetBucket("4u-cdn");
    s3Req.SetKey("/" + host.substr(0, host.find_first_of(".")) + std::string{req->getUrl()});
    std::cout << "/" + host.substr(0, host.find_first_of(".")) + std::string{req->getUrl()} << std::endl;

    Aws::S3::Model::GetObjectOutcome s3Outcome = s3Client.GetObject(s3Req);

    if (s3Outcome.IsSuccess()) {
      const Aws::IOStream &body = s3Outcome.GetResult().GetBody();

      std::stringstream ss;
      ss << body.rdbuf();

      Magick::Image image;
      image.read(ss.str());

      Magick::Blob imgBlob;
      image.write(&imgBlob);

      res->writeHeader("Content-Type", s3Outcome.GetResult().GetContentType().c_str())->end(imgBlob.base64());
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
      std::cout << "CDN could not be started." << "\n" << "Make sure you have the correct certificate files (certificate.pem and key.pem)." << std::endl;
    }
  }).run();

  return 0;
}