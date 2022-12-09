#include <getopt.h>
#include <errno.h>
#include <string.h>  // for strerror

#include <iostream>
#include <fstream>
#include <string>

#include <google/protobuf/text_format.h>
#include <json/json.h>
#include <mastodonpp/mastodonpp.hpp>
#include "config.pb.h"

static void usage() {
  std::cerr << "Usage: query_profile -c config acc@ount" << std::endl;
}

int main(int argc, char* argv[]) {
  std::string config_path;
  char current_opt;

  GOOGLE_PROTOBUF_VERIFY_VERSION;

  while ((current_opt = getopt(argc, argv, "c:")) != -1) {
    switch (current_opt) {
      case 'c':
        config_path = std::string(optarg);
        break;
      default:
        usage();
        return 1;
    }
  }

  argc -= optind;
  argv += optind;

  if (config_path.empty()) {
    usage();
    return 1;
  }

  std::ifstream config_stream(config_path);
  if (!config_stream.is_open()) {
    std::cerr << "Unable to read config file " << config_path << ": "
        << strerror(errno) << std::endl;
    return 1;
  }
  std::string config_contents(
      (std::istreambuf_iterator<char>(config_stream)),
      std::istreambuf_iterator<char>());

  congratbot::CongratBotConfig config;
  if (!google::protobuf::TextFormat::ParseFromString(
      config_contents, &config)) {
    std::cerr << "Error parsing " << config_path << std::endl;
  }

  try {
    mastodonpp::Instance instance{config.instance_url(), config.access_token()};
    mastodonpp::Connection conn{instance};

    mastodonpp::parametermap query_params{
      {"q", std::string_view(argv[0])},
      {"limit", "10"},
      {"resolve", "true"},
      {"following", "false"},
    };

    std::cout << "Querying " << argv[0] << std::endl;
    for (const auto& param : query_params) {
      std::cout << param.first << ": " << std::get<std::string_view>(param.second) << std::endl;
    }
    std::cout << std::endl;

    auto answer{conn.get(mastodonpp::API::v1::accounts_search, query_params)};
    if (answer) {
      Json::Reader reader;
      Json::Value root;
      Json::StreamWriterBuilder wbuilder;
      wbuilder["indentation"] = "  ";

      reader.parse(answer.body, root);
      std::cout << Json::writeString(wbuilder, root) << std::endl;
    } else {
      if (answer.curl_error_code == 0) {
        std::cerr << "HTTP status: " << answer.http_status << ": " << answer.body << std::endl;
      } else {
        std::cerr << "libcurl error " << std::to_string(answer.curl_error_code)
            << ": " << answer.error_message << std::endl;
      }
    }
  } catch (const mastodonpp::CURLException &e) {
    std::cerr << e.what() << std::endl;
  }
}
