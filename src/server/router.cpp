#include "router.hpp"
#include "doctest.h"
#include "server_commands.hpp"
#include "string_utils.hpp"
#include "text_data.hpp"
#include <tuple>

Router::Router()
    : m_controllers({
          {ServerCommand::Create, Controllers::create},
          {ServerCommand::Login, Controllers::login},
          {ServerCommand::Search, Controllers::search},
          {ServerCommand::Add, Controllers::add},
          {ServerCommand::Remove, Controllers::remove},
          {ServerCommand::Ping, Controllers::ping},
          {ServerCommand::Available, Controllers::available},
          {ServerCommand::None, Controllers::none }, // this when calling unexisting command
      }){};

void Router::route(Request &t_req) {

  auto [command, arguments] = parse(t_req);
  m_controllers[command](arguments, t_req);

}

Router::CmdTuple Router::parse(Request &t_req) {

  std::string raw_msg = TextData::to_string(t_req.data());
  auto [command, arguments] = StringUtils::split_first(raw_msg);

  return std::make_tuple(ServerCommand::get(command), arguments);
}


/** TESTS **/

// TEST_CASE("Router") {
//
//   SUBCASE("Parse commands") {
//
//     std::vector<std::pair<ServerCommand::name, std::string>> commands{
//         {ServerCommand::Create, "CREATE chris"},
//         {ServerCommand::Login, "LOGIN chris 1234"},
//         {ServerCommand::List, "LIST"},
//         {ServerCommand::Search, "SEARCH chris"},
//         {ServerCommand::Add, "ADD chris"},
//         {ServerCommand::Remove, "REMOVE chris"},
//         {ServerCommand::Ping, "PING"},
//         {ServerCommand::Available, "AVAILABLE chris"},
//         {ServerCommand::None, "BADCOMMAND"}};
//     for (auto &command : commands) {
//
//       Request req;
//       Router router;
//
//       req.set_data(new TextData(command.second));
//       router.route(req);
//
//       CHECK(router.command() == command.first);
//     }
//   }
//
//   SUBCASE("Parse Argument") {
//
//     Request req;
//     Router router;
//
//     req.set_data(new TextData("CREATE chris"));
//     router.route(req);
//
//     CHECK(router.arguments() == "chris");
//   }
//
//   SUBCASE("Parse No argument") {
//
//     Request req;
//     Router router;
//
//     req.set_data(new TextData("CREATE"));
//     router.route(req);
//
//     CHECK(router.arguments() == "");
//   }
// }
