#include "router.hpp"
#include "chat_controllers.hpp"
#include "call_controllers.hpp"
#include "doctest.h"
#include "server_commands.hpp"
#include "string_utils.hpp"
#include "text_data.hpp"

#include <tuple>

Router::Router()
    : m_controllers({
          /* user */
          {ServerCommand::List, UserControllers::list},
          {ServerCommand::Create, UserControllers::create},
          {ServerCommand::Login, UserControllers::login},
          {ServerCommand::Search, UserControllers::search},
          {ServerCommand::Add, UserControllers::add},
          {ServerCommand::Remove, UserControllers::remove},
          {ServerCommand::Available, UserControllers::available},
          {ServerCommand::Exit, UserControllers::exit},

          /* chat */
          {ServerCommand::Send, ChatControllers::send},
          {ServerCommand::Pending, ChatControllers::pending},
          {ServerCommand::Chat, ChatControllers::chat},
          {ServerCommand::Delivered, ChatControllers::delivered},

          /* call */
          {ServerCommand::Connect, CallControllers::connect},
          {ServerCommand::Accept, CallControllers::accept},
          {ServerCommand::Hangup, CallControllers::hangup},
          {ServerCommand::Reject, CallControllers::reject},
          {ServerCommand::Ping, CallControllers::ping},

          /* unexisting */
          {ServerCommand::None, UserControllers::none}, 
      }){};

void Router::route(Request &t_req) {

  if (!t_req.m_valid) { // invalid requests are not routed
    invalid_command(t_req);
    return;
  }

  auto [command, token, arguments] = parse(t_req);

  /* validate token */
  if (!is_loggedin(command, token, t_req)) { 
    return;
  }

  if (validate_argument(command, arguments)) {
    t_req.set_token(std::move(token)); // token to the request struct
    m_controllers[command](arguments, t_req);

  } else {
    invalid_command(t_req);
  }
}

Router::CmdTuple Router::parse(Request &t_req) {

  std::string raw_msg = TextData::to_string(t_req.data());
  auto [command, body] = StringUtils::split_first(raw_msg);
  auto cmd = ServerCommand::get(command);

  if (require_login(cmd)) { // will parse token only for command that require user to be logged in
    auto [token, arguments] = StringUtils::split_first(body);
    return std::make_tuple(cmd, token, arguments);
  }

  if(cmd == ServerCommand::Exit){
  return std::make_tuple(cmd, body, ""); // exit has only token
  }

  return std::make_tuple(cmd, "", body);
}

bool Router::validate_argument(ServerCommand::name t_cmd, std::string &t_arg) {

  if (ServerCommand::has_zero_or_more_arguments(t_cmd)) {
    return true;
  }

  if (ServerCommand::has_argument(t_cmd)) {
    return !t_arg.empty();
  } else {

    return true;
  }
}

bool Router::is_loggedin(ServerCommand::name t_cmd, std::string &t_token, Request &t_req) {

  if (require_login(t_cmd)) {
      return UserControllers::is_valid_token(t_token, t_req);
  }

  return true;
}

void Router::invalid_command(Request &t_req) {
  std::string reply = Reply::get_message(Reply::r_501);
  t_req.set_data(new TextData(reply));
}

/* these are the only commands that don't require user to be logged in */
bool Router::require_login(ServerCommand::name t_cmd) {
  return t_cmd != ServerCommand::Login
         && t_cmd != ServerCommand::Create
         && t_cmd != ServerCommand::Exit;
}

/** TESTS **/

TEST_CASE(
    "Router & Controllers (Postgres must be installed to run these tests)") {

  Router router;
  Postgres pg; // just to check was written to database

  SUBCASE("LIST Users contacts") {
    Request req;
    req.m_address = "123.453.3.1"; // must match user IP
    req.m_valid = true;
    req.set_data(new TextData("LIST"));

    router.route(req);

    std::string expected(
        "201 id:2,username:mario,password:,online:false,address:1.453.32.1 "
        "id:3,username:shakira,password:,online:false,address:53.423.4.1");
    auto reply = TextData::to_string(req.data());
    CHECK(reply == expected);
  }

  SUBCASE("CREATE a user") {

    Request req;
    req.m_address = "127.0.0.1";
    req.m_valid = true;
    req.set_data(new TextData("CREATE pedro 1234"));

    // route to the correct controler Controllers::create
    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_200));

    User u = pg.search_user_by("pedro", "username");
    CHECK(u.username() == "pedro");

    // remove tests entry from the database
    pg.remove_user(u);
  }

  SUBCASE("LOGIN user") {

    User u = User(0, "marcos", "1234", true, "127.0.0.1");
    pg.add_user(u); // add user for testing and not mess around with seed data.

    Request req;
    req.m_address = "127.0.0.1"; //  must match
    req.m_valid = true;
    req.set_data(new TextData("LOGIN marcos 1234"));
    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_200));

    User marcos = pg.search_user_by("marcos", "username");
    pg.remove_user(marcos);
  }

  SUBCASE("Invalid LOGIN") {

    Request req;
    req.m_address = "127.0.0.1";
    req.m_valid = true;
    req.set_data(new TextData("LOGIN shakira wrong_pass"));
    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_300));
  }

  SUBCASE("SEARCH user") {

    Request req;
    req.m_valid = true;
    req.set_data(new TextData("SEARCH mario"));
    router.route(req);

    auto reply = TextData::to_string(req.data());

    CHECK(reply == Reply::append_message(Reply::r_201,
                                         "id:2,username:mario,password:1234,"
                                         "online:false,address:1.453.32.1"));
  }

  SUBCASE("SEARCH unexisting user") {

    Request req;
    req.m_valid = true;
    req.set_data(new TextData("SEARCH abdulah"));
    router.route(req);

    auto reply = TextData::to_string(req.data());

    CHECK(reply == Reply::get_message(Reply::r_301));
  }

  SUBCASE("ADD and REMOVE User contact") {

    // add
    Request req;
    req.m_valid = true;
    req.m_address =
        "33.53.3.1"; // ip for dubius. must match in the database to find user.
    req.set_data(new TextData("ADD shakira")); // shakira is now dubius friend.
    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_200));

    User dubius = pg.search_user_by("dubius", "username");
    Users contacts = pg.list_user_contacts(dubius);

    // dubius now have a contacts
    CHECK(contacts.empty() == false);
    CHECK(contacts.at(0).to_string() ==
          "id:3,username:shakira,password:,online:false,address:53.423.4.1");

    //  remove
    req.set_data(new TextData("REMOVE shakira"));
    router.route(req);

    reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_200));

    dubius = pg.search_user_by("dubius", "username");
    contacts = pg.list_user_contacts(dubius);

    // dubius has no friends.
    CHECK(contacts.empty() == true);
  }

  SUBCASE("ADD unexisting contact") {
    Request req;
    req.m_valid = true;
    req.m_address = "33.53.3.1";                    // dubius IP address
    req.set_data(new TextData("ADD I_dont_exist")); //
    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_301));
  }

  SUBCASE("REMOVE unexisting contact") {
    Request req;
    req.m_valid = true;
    req.m_address = "33.53.3.1";                       // dubius IP address
    req.set_data(new TextData("REMOVE I_dont_exist")); //
    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_301));
  }

  SUBCASE("PING user") {
    Request req;
    req.m_valid = true;
    req.m_address = "127.0.0.1"; // Martha IP. she is online.
    req.set_data(new TextData("PING"));

    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_200));
  }

  SUBCASE("PING offline user") {
    Request req;
    req.m_valid = true;
    req.m_address = "123.453.3.1"; // khalil IP address
    req.set_data(new TextData("PING"));

    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_202));
  }

  SUBCASE("PING unexisting user") {
    Request req;
    req.m_valid = true;
    req.m_address = "0.0.0.0"; // not  unexisting IP address
    req.set_data(new TextData("PING"));

    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_301));
  }

  SUBCASE("AVAILABLE online user") {
    Request req;
    req.m_valid = true;
    req.set_data(new TextData("AVAILABLE martha"));

    router.route(req);
    auto reply = TextData::to_string(req.data());

    CHECK(reply == Reply::append_message(Reply::r_201,
                                         "id:5,username:martha,password:1234,"
                                         "online:true,address:127.0.0.1"));
  }

  SUBCASE("AVAILABLE offline user") {

    Request req;
    req.m_valid = true;
    req.set_data(new TextData("AVAILABLE khalil"));

    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_300));
  }

  SUBCASE("AVAILABLE unexisting user") {

    Request req;
    req.m_valid = true;
    req.set_data(new TextData("AVAILABLE jonas"));

    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_301));
  }

  SUBCASE("Bad command") {
    Request req;
    req.m_valid = true;
    req.set_data(new TextData("BAD COMMAND"));
    router.route(req);

    auto reply = TextData::to_string(req.data());
    CHECK(reply == Reply::get_message(Reply::r_500));
  }
}
