#include <getopt.h>
#include <errno.h>
#include <string.h>  // for strerror

#include <chrono>
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <thread>

#include <google/protobuf/text_format.h>
#include <json/json.h>
#include <mastodonpp/mastodonpp.hpp>
#include "config.pb.h"

using namespace std::chrono_literals;

static void usage() {
  std::cerr << "Usage: congratbot -c config" << std::endl;
}

int main(int argc, char* argv[]) {
  std::string config_path;
  char current_opt;

  GOOGLE_PROTOBUF_VERIFY_VERSION;
  std::mt19937 generator(std::random_device{}());

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
  std::cout << "Successfully read config " << config_path << std::endl;

  std::uniform_int_distribution<std::size_t> distribution(
    0, config.congrat_variants().congratulation_message_size() - 1);

  try {
    mastodonpp::Instance instance{config.instance_url(), config.access_token()};
    mastodonpp::Connection stream_conn{instance};
    mastodonpp::Connection send_conn{instance};

    // Find out if the streaming service is fine.
    auto answer{stream_conn.get(mastodonpp::API::v1::streaming_health)};
    if (answer && answer.body == "OK") {
      std::cout << "Streaming health OK" << std::endl;

      std::thread stream_thread{[&]
      {
        std::cout << "Invoking streaming_user" << std::endl;
        answer = stream_conn.get(mastodonpp::API::v1::streaming_user);
        if (!answer || answer.body != "OK") {
          if (answer.curl_error_code == 0) {
            std::cerr << "HTTP status: " << answer.http_status << std::endl;
          } else {
            std::cerr << "libcurl error " << std::to_string(answer.curl_error_code)
                << ": " << answer.error_message << std::endl;
          }
        } else {
          std::cout << "streaming_user invoked" << std::endl;
        }
      }};

      for (;;) {
        std::this_thread::sleep_for(1s);
        for (const auto &event : stream_conn.get_new_events()) {
          if (event.type == "notification") {
            Json::Reader reader;
            Json::Value root;

            reader.parse(event.data, root);

            if (root["type"].asString() == "mention") {
              const size_t i = distribution(generator);
              const std::string in_reply_to = root["status"]["id"].asString();
              std::string status = "@" + root["account"]["acct"].asString();

              for (const auto& mention : root["mentions"]) {
                status += "@" + mention["acct"].asString();
              }

              status += " " + config.congrat_variants().congratulation_message(i);

              mastodonpp::parametermap post_params{
                {"status", status},
                {"in_reply_to_id", in_reply_to},
                {"sensitive", root["status"]["sensitive"].asString()},
                {"spoiler_text", root["status"]["spoiler_text"].asString()},
                {"visibility", root["status"]["visibility"].asString()},
                {"language", "en"},
              };

              auto result{send_conn.post(mastodonpp::API::v1::statuses, post_params)};
              if (!result) {
                if (result.curl_error_code == 0) {
                  std::cerr << "HTTP status: " << result.http_status << ": " << result.body << std::endl;
                } else {
                  std::cerr << "libcurl error " << std::to_string(result.curl_error_code)
                      << ": " << result.error_message << std::endl;
                }
              }
            }
          }
        }
      }

      stream_conn.cancel_stream();
      stream_thread.join();
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
